"""Spec clean-up for generated sprites: near-black line dots become a dark tint of the colour they border,
and dots with no four-neighbour are removed. Nothing else in the drawing changes.

Each near-black dot takes one of a few fixed outline colours, picked by the hue of the opaque, non-black dots
around it, so the outline stays a handful of colours and the sprite keeps within the palette limit.
With --ring, bright dots on the silhouette edge also become outline (small floating pieces get one around them).
Run: python recolor_outline.py <in.png> [<out.png>] [--ring]   (writes in place when no output is given)
"""
import colorsys, os, sys
import numpy as np
from PIL import Image

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import spec

BLACK_LUMA = 14
# Dark tints by family, all inside the spec outline range (luma 8..80).
OUTLINES = {
    'plum': (92, 40, 78),     # pink hair, sakura, flames
    'dusk': (66, 54, 96),     # white cloth, lavender, metal
    'umber': (86, 58, 66),    # beige hair, skin
}


def family(rgb):
    h, l, s = colorsys.rgb_to_hls(*[v / 255 for v in rgb])
    if s < 0.18 or 0.55 < h < 0.8:
        return 'dusk'
    if h > 0.8 or h < 0.02:
        return 'plum' if s > 0.35 else 'umber'
    return 'umber'


def clean(a):
    a = a.copy()
    solid = a[..., 3] > 0
    p = np.pad(solid, 1)
    lone = solid & ~(p[:-2, 1:-1] | p[2:, 1:-1] | p[1:-1, :-2] | p[1:-1, 2:])
    a[lone] = 0
    solid &= ~lone
    lum = spec.luma(*[a[..., i].astype(float) for i in range(3)])
    black = solid & (lum < BLACK_LUMA)
    H, W = solid.shape
    out = a.copy()
    for y, x in zip(*np.where(black)):
        near = [a[yy, xx, :3] for yy in range(max(0, y - 1), min(H, y + 2)) for xx in range(max(0, x - 1), min(W, x + 2))
                if solid[yy, xx] and not black[yy, xx]]
        if not near:
            near = [a[yy, xx, :3] for yy in range(max(0, y - 2), min(H, y + 3)) for xx in range(max(0, x - 2), min(W, x + 3))
                    if solid[yy, xx] and not black[yy, xx]]
        tone = np.mean(near, axis=0) if near else (200, 190, 210)
        out[y, x, :3] = OUTLINES[family(tone)]
    return out, int(lone.sum()), int(black.sum())


SMALL_PIECE = 12  # dots; floating pieces up to this size (spirit flames) get an outline around them instead


def ring(a):
    """Gives the whole silhouette an outline: bright edge dots of a large piece take their own material's
    outline colour; a small floating piece keeps its colours and gets a one-dot outline around it."""
    from scipy import ndimage as nd
    a = a.copy()
    solid = a[..., 3] > 0
    lum = spec.luma(*[a[..., i].astype(float) for i in range(3)])
    lab, n = nd.label(solid)
    sizes = nd.sum(solid, lab, range(1, n + 1))
    p = np.pad(solid, 1)
    edge = solid & ~(p[:-2, 1:-1] & p[2:, 1:-1] & p[1:-1, :-2] & p[1:-1, 2:])
    recoloured = added = 0
    for y, x in zip(*np.where(edge & (lum > spec.OUTLINE_LUMA_MAX))):
        if sizes[lab[y, x] - 1] > SMALL_PIECE:
            a[y, x, :3] = OUTLINES[family(a[y, x, :3])]
            recoloured += 1
    small = np.isin(lab, [i + 1 for i, s in enumerate(sizes) if s <= SMALL_PIECE])
    grow = nd.binary_dilation(small) & ~solid
    for y, x in zip(*np.where(grow)):
        a[y, x, :3] = OUTLINES['plum']
        a[y, x, 3] = 255
        added += 1
    return a, recoloured, added


if __name__ == '__main__':
    args = [x for x in sys.argv[1:] if x != '--ring']
    src = args[0]
    out, lone, black = clean(np.array(Image.open(src).convert('RGBA')))
    msg = '%s: removed %d lone dots, recoloured %d near-black dots' % (os.path.basename(src), lone, black)
    if '--ring' in sys.argv:
        out, recoloured, added = ring(out)
        msg += ', outlined %d bright edge dots, added %d dots around small pieces' % (recoloured, added)
    Image.fromarray(out, 'RGBA').save(args[1] if len(args) > 1 else src)
    print(msg)
