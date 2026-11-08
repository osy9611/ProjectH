"""Face edits on Hiyori's Idle FR and LS frames: Resonance 7's eye structure, recoloured from blue to the
concept's rose, and no mouth (the FR mouth dot becomes skin; LS has no separate mouth dot).

Resonance 7 (references/resonance, native resolution) draws each eye 2 dots wide and 3 rows tall:
  row 1  near-black lash over both dots
  row 2  greyed iris on the outer dot, deepest iris on the inner dot (toward the nose)
  row 3  white highlight on the outer dot, brightest iris on the inner dot
with a 3-dot gap between the eyes. Hiyori's eyes already sit on that grid (lash row 36, iris rows 37-38),
so only these dots change. Colours other than DEEP already exist in the sprite, to keep within the palette limit.
Run: python apply_eyes.py <source folder> <output folder>
"""
import os, sys
from PIL import Image

LASH = (47, 25, 58)
GREYED = (172, 132, 160)
DEEP = (160, 40, 100)
BRIGHT = (217, 105, 165)
HIGHLIGHT = (243, 243, 250)
SKIN = (240, 211, 208)

# (x, y, colour) per frame. FR: left eye outer 28 / inner 29, right eye inner 33 / outer 34.
# LS faces left, so the inner (nose) side of its one eye is the left dot, 16.
EYES = {
    'FR': [(x, 36, LASH) for x in (28, 29, 33, 34)]
          + [(28, 37, GREYED), (29, 37, DEEP), (33, 37, DEEP), (34, 37, GREYED)]
          + [(28, 38, HIGHLIGHT), (29, 38, BRIGHT), (33, 38, BRIGHT), (34, 38, HIGHLIGHT)]
          + [(31, 39, SKIN)],
    'LS': [(16, 36, LASH), (17, 36, LASH), (16, 37, DEEP), (17, 37, GREYED), (16, 38, BRIGHT), (17, 38, HIGHLIGHT)],
}

if __name__ == '__main__':
    src, dst = sys.argv[1], sys.argv[2]
    for view, dots in EYES.items():
        im = Image.open(os.path.join(src, 'Hiyori_Idle_%s.png' % view)).convert('RGBA')
        for x, y, colour in dots:
            im.putpixel((x, y), colour + (255,))
        im.save(os.path.join(dst, 'Hiyori_Idle_%s.png' % view))
        print(view, len(dots), 'dots')
