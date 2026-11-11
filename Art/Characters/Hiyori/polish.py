"""Hiyori front polish: single-dot fixes on the approved front (D2), each with its reason (brief.md "폴리싱").

Every colour used here is already in the sprite's palette, so the colour count does not change.
Run: python polish.py <in.png> <out.png>
"""
import sys
import numpy as np
from PIL import Image

SKIN = (0xd8, 0xbf, 0xb6)
LEG_EDGE = (0x4d, 0x41, 0x66)

FIXES = [
    # generator's own right eye was 3 wide; eyes.py draws 2, so its outer column is left over
    ((35, 37), SKIN, 'leftover lash of the generated right eye'),
    ((35, 38), SKIN, 'leftover iris of the generated right eye'),
    ((35, 39), SKIN, 'leftover glint of the generated right eye'),
    # pink dots between the eyes read as a third eye or a mark at game scale
    ((30, 36), SKIN, 'stray pink dot on the forehead'),
    ((32, 37), SKIN, 'stray pink dot between the eyes'),
    # right leg outer edge turns brown for three rows; the rest of both legs use the violet edge
    ((37, 68), LEG_EDGE, 'brown dot in the violet leg edge'),
    ((37, 69), LEG_EDGE, 'brown dot in the violet leg edge'),
    ((37, 70), LEG_EDGE, 'brown dot in the violet leg edge'),
]


def polish(a):
    out = a.copy()
    for (x, y), c, _ in FIXES:
        assert out[y, x, 3], 'fix at %d,%d lands on an empty dot' % (x, y)
        out[y, x] = (*c, 255)
    return out


if __name__ == '__main__':
    a = np.array(Image.open(sys.argv[1]).convert('RGBA'))
    Image.fromarray(polish(a), 'RGBA').save(sys.argv[2])
