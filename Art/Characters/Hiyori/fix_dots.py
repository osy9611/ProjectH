"""Hand fixes on single dots that generation or inpainting left wrong, listed per frame.

LS 2026-11-09: a three-dot hole between the back of the arm and the back line (column 24, rows 47-49)
showed the background; it is the dress behind the arm, so it takes the dress shade.
BK 2026-11-09: the skin band under the nape becomes the white frilled collar (see BK_COLLAR).
Run: python fix_dots.py <folder>   (edits Hiyori_Idle_<view>.png in place)
"""
import os, sys
from PIL import Image

DRESS_SHADE = (142, 130, 176)
WHITE, CLOTH, FRILL, LINE = (243, 243, 250), (236, 232, 245), (207, 204, 231), (66, 54, 96)

# BK 2026-11-09: rows 42-43 under the nape were a band of skin, as if the back were bare. The concept has a
# pink choker (row 40, kept), a white frilled high collar and then the cape, so the band becomes collar.
BK_COLLAR = ([(28, 42, LINE)] + [(x, 42, WHITE if x in (29, 34) else CLOTH) for x in range(29, 35)] + [(35, 42, FRILL)]
             + [(x, 43, WHITE if x % 2 else FRILL) for x in range(27, 37)])

FIXES = {
    'LS': [(24, 47, DRESS_SHADE), (24, 48, DRESS_SHADE), (24, 49, DRESS_SHADE)],
    'BK': BK_COLLAR,
}

if __name__ == '__main__':
    folder = sys.argv[1]
    for view, dots in FIXES.items():
        path = os.path.join(folder, 'Hiyori_Idle_%s.png' % view)
        im = Image.open(path).convert('RGBA')
        for x, y, colour in dots:
            im.putpixel((x, y), colour + (255,))
        im.save(path)
        print(view, len(dots), 'dots')
