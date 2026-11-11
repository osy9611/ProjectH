"""Hiyori eyes: the Amelia (FFBE map sprite) eye structure, measured only, drawn in Hiyori's colours (brief.md "눈").

Each eye is 2 wide and 3 rows; the eyes sit 3 dots apart.
  row 1  lash, both dots
  row 2  outer: grey-violet, inner: deep iris
  row 3  outer: white glint, inner: bright iris
"Outer" is the side away from the nose. x, y is the top-left dot of the viewer's left eye (its lash row).
Run: python eyes.py <in.png> <out.png> <x> <y>
"""
import sys
import numpy as np
from PIL import Image

LASH = (0x3e, 0x1a, 0x33)
GREY = (0x9a, 0x8c, 0xa3)
DEEP = (0xb2, 0x38, 0x6e)
BRIGHT = (0xf0, 0x8c, 0xb8)
GLINT = (0xf4, 0xee, 0xf4)
GAP = 3
COLOURS = (LASH, GREY, DEEP, BRIGHT, GLINT)


def draw(a, x, y):
    a = a.copy()
    # (outer column, inner column) for the viewer's left eye, then the right eye
    for outer, inner in ((x, x + 1), (x + 2 + GAP + 1, x + 2 + GAP)):
        for col in (outer, inner):
            a[y, col] = (*LASH, 255)
        a[y + 1, outer] = (*GREY, 255)
        a[y + 1, inner] = (*DEEP, 255)
        a[y + 2, outer] = (*GLINT, 255)
        a[y + 2, inner] = (*BRIGHT, 255)
    return a


if __name__ == '__main__':
    src, dst, x, y = sys.argv[1], sys.argv[2], int(sys.argv[3]), int(sys.argv[4])
    a = np.array(Image.open(src).convert('RGBA'))
    Image.fromarray(draw(a, x, y), 'RGBA').save(dst)
