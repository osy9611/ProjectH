"""Clears the checkerboard dithering in Hiyori's FR bodice: a dark dot with at least three light four-neighbours
becomes the dress shade colour, so the bodice reads as white cloth with soft lavender folds.

Only dark dots inside the bodice box change; lines (dark dots with dark neighbours), pink trims, the arms
(mirrored, confirmed) and the birdcage frame on the hip below row 58 stay as drawn.
Run: python clean_bodice.py <in.png> <out.png>
"""
import sys
import numpy as np
from PIL import Image

BOX = (23, 43, 40, 57)          # x0, y0, x1, y1 inclusive
SHADE = (194, 168, 194)         # dress shade already in the palette
LIGHT, DARK = 175, 110          # luma thresholds


def luma(a):
    return a[..., :3] @ np.array([0.299, 0.587, 0.114])


if __name__ == '__main__':
    a = np.array(Image.open(sys.argv[1]).convert('RGBA'))
    for _ in range(2):
        lum = luma(a.astype(float))
        out = a.copy()
        x0, y0, x1, y1 = BOX
        for y in range(y0, y1 + 1):
            for x in range(x0, x1 + 1):
                if a[y, x, 3] == 0 or lum[y, x] >= DARK:
                    continue
                light = sum(1 for dy, dx in ((0, 1), (0, -1), (1, 0), (-1, 0))
                            if a[y + dy, x + dx, 3] and lum[y + dy, x + dx] >= LIGHT)
                if light >= 3:
                    out[y, x, :3] = SHADE
        a = out
    Image.fromarray(a, 'RGBA').save(sys.argv[2])
