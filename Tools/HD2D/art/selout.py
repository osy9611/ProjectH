"""Selective coloured outline, the way the FFBE map sprites (BEFORE side of the Resonance promo pairs) draw it: every outline dot takes the colour of the
material just inside it, darkened, so a pink tail gets a deep crimson edge and white cloth a deep lavender one.

FFBE map sprites (references/resonance_promo/image-*.png left halves, native resolution) edges sit at 45-66% of the interior lightness (median per
sprite), only 55-78% of edge dots are clearly darker than the inside (the lit side stays light), and none is black. Outline dots here are the silhouette ring plus any dot drawn in a
line colour (LINE_COLOURS: the fixed outline colours of earlier passes and near-black dots); each takes the most
common non-line colour among its eight neighbours (radius 2 when none), with HLS lightness times DARKEN and
saturation raised. Protected colours (eye lash, iris) never change.
Run: python selout.py <in.png> <out.png> [--protect r,g,b ...]
"""
import colorsys, os, sys
import numpy as np
from PIL import Image

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import spec

DARKEN = 0.55
LIT_DARKEN = 0.85  # silhouette edges that face the light (open above or to the left only)
MAX_V = 0.40
MAX_S = 0.7
LINE_COLOURS = {(92, 40, 78), (66, 54, 96), (86, 58, 66), (60, 44, 84), (60, 44, 83), (80, 55, 80), (76, 60, 102)}
NEAR_BLACK = 30  # luma


def darken(rgb, factor=DARKEN):
    # HSV, not HLS: HLS calls a lavender-white highly saturated, which turned white cloth edges royal blue.
    # Value drops to DARKEN of the interior (capped at MAX_V); saturation grows from the colour's real chroma,
    # so white and grey cloth keep a muted dark lavender edge and coloured parts get a deeper tint of themselves.
    h, s, v = colorsys.rgb_to_hsv(*[c / 255 for c in rgb])
    r, g, b = colorsys.hsv_to_rgb(h, min(MAX_S, s * 1.6 + 0.12) if factor < 0.7 else s, min(v * factor, MAX_V if factor < 0.7 else 1.0))
    return tuple(int(round(c * 255)) for c in (r, g, b))


def key(rgb):
    return (int(rgb[0]) << 16) | (int(rgb[1]) << 8) | int(rgb[2])


def unkey(k):
    return ((k >> 16) & 255, (k >> 8) & 255, k & 255)


def selout(a, protect=()):
    a = a.copy()
    solid = a[..., 3] > 0
    H, W = solid.shape
    colour = (a[..., 0].astype(np.int64) << 16) | (a[..., 1].astype(np.int64) << 8) | a[..., 2].astype(np.int64)
    lum = spec.luma(*[a[..., i].astype(float) for i in range(3)])
    p = np.pad(solid, 1)
    ring = solid & ~(p[:-2, 1:-1] & p[2:, 1:-1] & p[1:-1, :-2] & p[1:-1, 2:])
    is_line = np.isin(colour, [key(c) for c in LINE_COLOURS]) & solid
    line = ring | is_line | (solid & (lum < NEAR_BLACK))
    line &= ~np.isin(colour, [key(c) for c in protect])
    out = a.copy()
    changed = 0
    for y, x in zip(*np.where(line)):
        near = []
        for r in (1, 2):
            near = [colour[yy, xx] for yy in range(max(0, y - r), min(H, y + r + 1))
                    for xx in range(max(0, x - r), min(W, x + r + 1))
                    if solid[yy, xx] and not line[yy, xx]]
            if near:
                break
        if not near:
            continue
        vals, counts = np.unique(near, return_counts=True)
        base = unkey(int(vals[counts.argmax()]))
        # Selective outline: a silhouette dot open only above or to the left is lit by the upper-left light,
        # so it stays close to its material; every other outline dot gets the full darkening.
        def open_(yy, xx):
            return not (0 <= yy < H and 0 <= xx < W and solid[yy, xx])
        lit = ring[y, x] and (open_(y - 1, x) or open_(y, x - 1)) and not (open_(y + 1, x) or open_(y, x + 1))
        out[y, x, :3] = darken(base, LIT_DARKEN if lit else DARKEN)
        changed += 1
    return out, changed


if __name__ == '__main__':
    args = sys.argv[1:]
    protect = []
    if '--protect' in args:
        i = args.index('--protect')
        protect = [tuple(int(v) for v in s.split(',')) for s in args[i + 1:]]
        args = args[:i]
    a = np.array(Image.open(args[0]).convert('RGBA'))
    out, n = selout(a, protect)
    Image.fromarray(out, 'RGBA').save(args[1])
    print('%s: %d outline dots recoloured' % (os.path.basename(args[0]), n))
