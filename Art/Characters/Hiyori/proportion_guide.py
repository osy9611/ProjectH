"""Proportion guide: reshape the concept front to the body reference's proportions before it goes to PixelLab pro,
which keeps the input's proportions (brief.md "비율 가이드").

The concept is an illustration (about 6.9 heads). Each band is rescaled on its own:
  head   hair top .. chin       -> HEAD_SHARE of the height, scaled the same both ways (chibi head)
  torso  chin .. skirt hem      -> TORSO_SHARE
  legs   skirt hem .. soles     -> LEGS_SHARE
Width per band is SX; it blends over BLEND source rows at the band edges so the twin tails do not jump.
Shares are Fina's (head 15, body 17, legs 18 of 50). Row positions are read off concept/sheet_front_clean.png.
Run: python proportion_guide.py <concept.png> <out.png>
"""
import sys
import numpy as np
from PIL import Image

HAIR_TOP, CHIN, HEM, SOLE = 25, 110, 345, 610
HEAD_SHARE, TORSO_SHARE, LEGS_SHARE = 15 / 50, 17 / 50, 18 / 50
HEAD_SCALE = 2.0
SX = {'head': 2.0, 'torso': 1.5, 'legs': 1.5}
BLEND = 15
BG = (197, 197, 197)


def guide(im):
    a = np.array(im.convert('RGB'))
    h, w = a.shape[:2]
    head_px = (CHIN - HAIR_TOP) * HEAD_SCALE
    total = head_px / HEAD_SHARE
    fy = {'head': HEAD_SCALE, 'torso': total * TORSO_SHARE / (HEM - CHIN), 'legs': total * LEGS_SHARE / (SOLE - HEM)}
    band = lambda y: 'head' if y < CHIN else ('torso' if y < HEM else 'legs')

    def sx(y):
        for edge, lo, hi in ((CHIN, 'head', 'torso'), (HEM, 'torso', 'legs')):
            if abs(y - edge) < BLEND:
                t = (y - edge + BLEND) / (2 * BLEND)
                return SX[lo] * (1 - t) + SX[hi] * t
        return SX[band(y)]

    # output row for every source row, then every output row samples the nearest source row
    ys = np.cumsum([fy[band(y)] for y in range(h)])
    out_h, out_w = int(ys[-1]) + 1, int(w * max(SX.values()))
    out = np.zeros((out_h, out_w, 3), np.uint8)
    out[:] = BG
    cx = w / 2
    for oy in range(out_h):
        y = min(int(np.searchsorted(ys, oy)), h - 1)
        s = sx(y)
        row = np.array(Image.fromarray(a[y:y + 1]).resize((max(1, round(w * s)), 1), Image.LANCZOS))[0]
        x0 = int(out_w / 2 - cx * s)
        out[oy, max(0, x0):x0 + row.shape[0]] = row[max(0, -x0):out_w - x0]
    return Image.fromarray(out), fy


if __name__ == '__main__':
    g, fy = guide(Image.open(sys.argv[1]))
    g.save(sys.argv[2])
    print('size %dx%d, vertical scale %s' % (g.width, g.height, {k: round(v, 2) for k, v in fy.items()}))
