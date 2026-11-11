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


# Stage 2 (2026-11-11): match the Fina body reference (brief.md "레퍼런스").
# Face: Fina's eyes start 7 rows above the chin and her bangs stop on the lash row. D3's eyes started 5 rows
# above the chin (chin row 42), so the face looked squashed under a deep fringe. Raise the eye rows by 2 inside the
# face columns and fill the two freed rows with the cheek row: eyes 35-37, cheeks 38-41, chin 42.
FACE_X0, FACE_X1 = 25, 35
EYE_ROWS_OLD, RAISE = (37, 38, 39), 2
CHEEK_ROW = 40
# Value: Fina's luma spread is 73 (std) against D3's 53; the white dress used shade steps only 15-25 apart.
# Push mid and shade tones below MID_LUMA further down so the 4-step ramp reads, without touching the lights
# (luma rule) or the outlines (below LOW_LUMA).
MID_LUMA, LOW_LUMA, GAIN = 175, 60, 1.35


def raise_eyes(a):
    out = a.copy()
    for y in EYE_ROWS_OLD:
        out[y - RAISE, FACE_X0:FACE_X1 + 1] = a[y, FACE_X0:FACE_X1 + 1]
    for y in range(EYE_ROWS_OLD[-1] - RAISE + 1, CHEEK_ROW):
        out[y, FACE_X0:FACE_X1 + 1] = a[CHEEK_ROW, FACE_X0:FACE_X1 + 1]
    return out


def deepen(a, keep=()):
    out = a.copy()
    rgb = out[..., :3].astype(float)
    lum = spec.luma(rgb[..., 0], rgb[..., 1], rgb[..., 2])
    target = MID_LUMA - (MID_LUMA - lum) * GAIN
    band = (lum < MID_LUMA) & (lum >= LOW_LUMA) & (out[..., 3] > 0)
    for c in keep:
        band &= ~np.all(out[..., :3] == c, axis=-1)
    target = np.maximum(target, LOW_LUMA * 0.8)
    scale = np.where(band, target / np.maximum(lum, 1), 1.0)
    out[..., :3] = np.clip(rgb * scale[..., None], 0, 255).round().astype(np.uint8)
    return out


# Fina has a warm dot under each eye; after the raise the cheek rows are one flat skin block without it.
BLUSH = (0xe5, 0xa0, 0xba)
BLUSH_DOTS = ((28, 38), (34, 38))


# Legs: below the skirt Fina shows 18 of 50 rows (36%), D3 only 12 (24%). Take LEG_ROWS rows out of the torso and
# skirt (the ones most like the row above, so the cut is least visible) and repeat a bare-leg row above the shoes.
# Height and the head stay as they are; everything between the cut and the shoes moves up.
TORSO_Y0, TORSO_Y1 = 44, 63      # rows that may be removed (below the collar, above the skirt hem)
LEG_ROW, SHOE_Y0 = 73, 74        # a row with only the two legs; shoes start here
LEG_ROWS = 4
KEEP_ROWS = range(50, 58)       # waist flower band and skirt stars: never cut


def lengthen_legs(a, n=LEG_ROWS):
    diff = {y: int(np.abs(a[y].astype(int) - a[y - 1].astype(int)).sum()) for y in range(TORSO_Y0, TORSO_Y1 + 1) if y not in KEEP_ROWS}
    cut = []
    for y in sorted(diff, key=diff.get):
        if all(abs(y - c) > 2 for c in cut):  # spread the cuts so no shape loses two rows in a row
            cut.append(y)
        if len(cut) == n:
            break
    rows = [y for y in range(TORSO_Y0, SHOE_Y0) if y not in cut] + [LEG_ROW] * n
    out = a.copy()
    out[TORSO_Y0:SHOE_Y0] = a[rows]
    return out, sorted(cut)


def fina(a):
    import eyes
    out = deepen(raise_eyes(a), keep=eyes.COLOURS + (BLUSH,))
    for x, y in BLUSH_DOTS:
        out[y, x] = (*BLUSH, 255)
    out, cut = lengthen_legs(out)
    print('rows moved to the legs:', cut)
    return out


# Stage 3 (2026-11-11): face structure from Ayaka (FFBE, measured only; brief.md "얼굴 레퍼런스").
# Ayaka: eyes touch the side locks on both sides with 3 skin dots between them; the inner dot of the eye's middle
# row is a dark pupil and only the bottom row carries the light iris; the skin is lightest in the middle and steps
# to a shade at the sides and a warm dark at the chin corners. D4 had a flat skin block one column too wide on the
# right (eyes off centre). Rows and columns below are D4's.
LIGHT = (0xe2, 0xc8, 0xbe)       # luma 207, under the 210 rule
SHADE = (0xd8, 0xbf, 0xb6)
WARM = (0xc8, 0x9a, 0x94)
CHIN_DARK = (0xa0, 0x6a, 0x6e)
PUPIL = (0x5e, 0x22, 0x44)       # Ayaka uses her lash colour; a dark berry keeps a pink hint
SIDE_LOCK = (0x86, 0x6e, 0x71)
JAW = (0x74, 0x5b, 0x63)
FACE_COLOURS = (LIGHT, SHADE, WARM, CHIN_DARK, PUPIL)
L, S, W, D, B = LIGHT, SHADE, WARM, CHIN_DARK, BLUSH
AYAKA_ROWS = {                   # row: colours for columns 28..34 (None = keep)
    35: (None, None, S, S, S, None, None),
    36: (None, PUPIL, S, S, S, PUPIL, None),
    37: (None, None, L, L, L, None, None),
    38: (B, L, L, L, L, L, B),
    39: (S, L, L, L, L, L, S),
    40: (W, S, L, L, L, S, W),
    41: (JAW, W, S, S, S, W, JAW),
    42: (None, None, D, S, D, None, None),
}


def ayaka(a):
    out = a.copy()
    for y, row in AYAKA_ROWS.items():
        for i, c in enumerate(row):
            if c is not None:
                out[y, 28 + i] = (*c, 255)
    for y in range(35, 42):          # right side lock moves in one column so the face is symmetric
        out[y, 35] = (*SIDE_LOCK, 255)
    return out


if __name__ == '__main__':
    import argparse, os
    sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '../../../Tools/HD2D/art'))
    import spec
    p = argparse.ArgumentParser()
    p.add_argument('src')
    p.add_argument('dst')
    p.add_argument('--fina', action='store_true', help='stage 2: raise the eyes and deepen the shade steps')
    p.add_argument('--ayaka', action='store_true', help='stage 3: Ayaka face structure, then back to the palette limit')
    o = p.parse_args()
    a = np.array(Image.open(o.src).convert('RGBA'))
    if o.ayaka:
        import eyes
        from merge_colors import merge
        b = ayaka(a)
        keep = set(eyes.COLOURS) | set(FACE_COLOURS) | {BLUSH, SIDE_LOCK, JAW}
        (b,), before, after = merge([b], spec.ROLES['hero']['colors'], keep)
        print('colours %d -> %d' % (before, after))
    else:
        b = fina(a) if o.fina else polish(a)
    Image.fromarray(b, 'RGBA').save(o.dst)
