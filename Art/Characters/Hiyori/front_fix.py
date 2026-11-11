"""Hiyori front (B) fixes after the clean-up, as coordinates so they can be rerun (brief.md "정면 보정").

1. tails  the twin tails (columns outside the skirt band) slide inward, 0 dots at TAIL_Y0 rising to TAIL_SHIFT at
          TAIL_Y1 and below. The body is painted back on top, so the tails pass behind the skirt.
          Front width/height goes from 0.92 to the Fina target (0.75, limit 0.78).
2. tone   lumas above KNEE are compressed so the white dress tops out near spec.TONE_LUMA_MAX (day scenes clip from 210).
3. cage   the birdcage frame from concept A on the character's left hip (screen right): 1-dot steel bars and two rings.
Run: python front_fix.py <in.png> <out.png>
"""
import os, sys
import numpy as np
from PIL import Image

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '../../../Tools/HD2D/art'))
import spec

BODY_X0, BODY_X1 = 23, 42      # skirt band, inclusive; columns outside it are tail
TAIL_Y0, TAIL_Y1, TAIL_SHIFT = 44, 50, 4
KNEE, TOP = 175, 208

STEEL = (0x6e, 0x76, 0x9a)
STEEL_LIT = (0xa4, 0xac, 0xcc)
CAGE_BARS = (35, 37, 39, 41)   # columns
CAGE_TOP, CAGE_BOTTOM = 55, 65
CAGE_RINGS = (57, 63)
COLOURS = (STEEL, STEEL_LIT)


def tails(a):
    out = a.copy()
    h, w = a.shape[:2]
    for y in range(TAIL_Y0, h):
        s = round(TAIL_SHIFT * min(1.0, (y - TAIL_Y0) / (TAIL_Y1 - TAIL_Y0)))
        if not s:
            continue
        row = a[y]
        new = np.zeros_like(row)
        new[BODY_X0:BODY_X1 + 1] = 0
        left, right = row[:BODY_X0], row[BODY_X1 + 1:]
        new[s:BODY_X0 + s] = left                     # left tail moves right
        new[BODY_X1 + 1 - s:w - s] = right            # right tail moves left
        body = row[BODY_X0:BODY_X1 + 1]
        keep = body[:, 3] > 0
        band = new[BODY_X0:BODY_X1 + 1]
        band[keep] = body[keep]                       # skirt in front of the tails
        out[y] = new
    return out


def tone(a):
    out = a.copy()
    rgb = out[..., :3].astype(float)
    lum = spec.luma(rgb[..., 0], rgb[..., 1], rgb[..., 2])
    hi = (lum > KNEE) & (out[..., 3] > 0)
    target = KNEE + (lum - KNEE) * (TOP - KNEE) / (255 - KNEE)
    scale = np.where(hi, target / np.maximum(lum, 1), 1.0)
    out[..., :3] = np.clip(rgb * scale[..., None], 0, 255).round().astype(np.uint8)
    return out


def cage(a):
    out = a.copy()
    for y in range(CAGE_TOP, CAGE_BOTTOM + 1):
        for x in CAGE_BARS:
            out[y, x] = (*(STEEL_LIT if x == CAGE_BARS[0] else STEEL), 255)
    for y in CAGE_RINGS:
        for x in range(CAGE_BARS[0], CAGE_BARS[-1] + 1):
            out[y, x] = (*STEEL_LIT, 255) if x % 2 == 0 else (*STEEL, 255)
    return out


# Waist flower band (design sheet): small lilies from the screen-left waist down to the screen-right hip.
# Petals stay at luma <= 210 (white dress rule); a pink centre and pink corners make them read on the white dress.
PETAL = (0xd2, 0xc8, 0xdc)
PETAL_SHADE = (0xa8, 0x98, 0xc0)
FLOWER_PINK = (0xe0, 0x78, 0x9f)
FLOWER_RIM = (0xf0, 0xa8, 0xc4)
FLOWERS = ((24, 51), (28, 52), (33, 54), (38, 56))   # centres
FLOWER_COLOURS = (PETAL, PETAL_SHADE, FLOWER_PINK, FLOWER_RIM)


def flowers(a):
    out = a.copy()
    for cx, cy in FLOWERS:
        for dx, dy, c in ((0, 0, FLOWER_PINK), (-1, 0, PETAL), (1, 0, PETAL), (0, -1, PETAL), (0, 1, PETAL_SHADE),
                          (-1, -1, FLOWER_RIM), (1, -1, FLOWER_RIM), (1, 1, PETAL_SHADE)):
            if out[cy + dy, cx + dx, 3]:
                out[cy + dy, cx + dx] = (*c, 255)
    return out


def fix(a, shift=TAIL_SHIFT, draw_cage=True, waist_flowers=False):
    global TAIL_SHIFT
    TAIL_SHIFT = shift
    a = tone(tails(a))
    if waist_flowers:
        a = flowers(a)
    return cage(a) if draw_cage else a


if __name__ == '__main__':
    import argparse
    p = argparse.ArgumentParser()
    p.add_argument('src')
    p.add_argument('dst')
    p.add_argument('--shift', type=int, default=TAIL_SHIFT, help='dots the tails move inward')
    p.add_argument('--no-cage', action='store_true', help='the sprite already has the cage (sheet fronts C, D)')
    p.add_argument('--waist-flowers', action='store_true', help='draw the waist flower band (sheet front D)')
    o = p.parse_args()
    a = np.array(Image.open(o.src).convert('RGBA'))
    Image.fromarray(fix(a, o.shift, not o.no_cage, o.waist_flowers), 'RGBA').save(o.dst)
