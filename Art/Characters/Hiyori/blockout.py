"""Hiyori front blockout: a clean 50-dot front drawn from the element budget (brief.md "블록아웃"), used as the
edit_image INPUT so PixelLab keeps these proportions and only re-renders the outfit from a reference.

Proportions (Fina): hair top row 27, chin row 41 (head 15), body rows 42-57 (16 + hem), legs rows 58-75 (18).
Face and eyes (Ayaka): 7-column face (28-34), eyes 2x3 at rows 34-36, 3 dots apart, chin->eye 7.
Everything is drawn on the left half and mirrored around column 31; asymmetric parts (lily, waist flowers,
garter flower) are added after the mirror. Colours are flat zones with one shade; luma stays <= 210 except
small lit dots, the outline is plum (no near-black).
Run: python blockout.py <out.png>
"""
import sys
import numpy as np
from PIL import Image

W, H, CX = 64, 80, 31
T = (0, 0, 0, 0)

C = {
    'hair': (0xc4, 0xb4, 0xa6), 'hair_s': (0xa0, 0x8e, 0x88), 'hair_l': (0xd4, 0xc8, 0xba),
    'mint': (0xb0, 0xd4, 0xc0), 'sky': (0xac, 0xc4, 0xe0), 'lemon': (0xdc, 0xd4, 0xa0),
    'pink': (0xe0, 0x8c, 0xaa), 'pink_d': (0xc0, 0x60, 0x88), 'pink_l': (0xf0, 0xb0, 0xc8),
    'skin': (0xe6, 0xcc, 0xc0), 'skin_s': (0xd4, 0xb0, 0xa8), 'blush': (0xe5, 0xa0, 0xba),
    'white': (0xd0, 0xca, 0xdc), 'white_l': (0xe8, 0xe4, 0xee), 'lav': (0xb4, 0xaa, 0xcc), 'lav_d': (0x8f, 0x7f, 0xae),
    'cage': (0x4c, 0x4a, 0x5c), 'line': (0x3b, 0x23, 0x47),
    'lash': (0x3e, 0x1a, 0x33), 'eye_out': (0x9a, 0x8c, 0xa3), 'pupil': (0x5e, 0x22, 0x44),
    'iris': (0xf0, 0x8c, 0xb8), 'glint': (0xf4, 0xee, 0xf4),
}


def blank():
    return np.zeros((H, W, 4), np.uint8)


def put(a, x, y, c):
    if 0 <= x < W and 0 <= y < H:
        a[y, x] = (*C[c], 255)


def span(a, y, x0, x1, c):
    for x in range(x0, x1 + 1):
        put(a, x, y, c)


def left_half():
    a = blank()
    # twin tail (behind), x by row; colour bands ash -> mint/sky/lemon -> pink
    tail = {33: (21, 23), 34: (20, 23), 35: (20, 23), 36: (20, 23), 37: (19, 22), 38: (19, 22), 39: (19, 22),
            40: (19, 22), 41: (19, 22), 42: (19, 22), 43: (19, 22), 44: (19, 22), 45: (19, 22), 46: (19, 22),
            47: (19, 22), 48: (19, 22), 49: (19, 22), 50: (18, 21), 51: (18, 21), 52: (18, 21), 53: (18, 21),
            54: (18, 21), 55: (18, 21), 56: (18, 21), 57: (18, 21), 58: (18, 21), 59: (18, 21), 60: (19, 22),
            61: (19, 22), 62: (17, 21), 63: (16, 19), 64: (16, 18)}
    for y, (x0, x1) in tail.items():
        c = 'hair' if y < 47 else ('mint' if y < 50 else ('sky' if y < 52 else ('lemon' if y < 54 else 'pink')))
        span(a, y, x0, x1, c)
        put(a, x1, y, 'hair_s' if y < 47 else ('pink_d' if y >= 54 else c))
    # head: hair mass rows 27-41
    head = {27: (26, 31), 28: (24, 31), 29: (23, 31), 30: (23, 31), 31: (22, 31), 32: (22, 31), 33: (22, 31),
            34: (22, 31), 35: (22, 31), 36: (23, 31), 37: (23, 31), 38: (23, 31), 39: (24, 31), 40: (24, 31),
            41: (25, 31)}
    for y, (x0, x1) in head.items():
        span(a, y, x0, x1, 'hair')
    for y in range(28, 31):
        span(a, y, 27, 30, 'hair_l')                      # crown highlight
    # face: columns 28-34 (left half 28-31), rows 34-41; bangs end on the lash row (34)
    for y in range(34, 42):
        span(a, y, 28 if y < 40 else 29, 31, 'skin')
    span(a, 41, 30, 31, 'skin')
    put(a, 29, 41, 'skin_s')
    for y in range(34, 42):
        put(a, 27, y, 'hair_s')                           # side lock edge
    put(a, 28, 37, 'blush')
    # eye (Ayaka 2x3): lash / grey + pupil / glint + iris
    put(a, 28, 34, 'lash'); put(a, 29, 34, 'lash')
    put(a, 28, 35, 'eye_out'); put(a, 29, 35, 'pupil')
    put(a, 28, 36, 'glint'); put(a, 29, 36, 'iris')
    # wing ornament: white ribbon wing with pink trim, top-left of the head
    wing = {26: (20, 21), 27: (20, 23), 28: (20, 24), 29: (21, 24), 30: (21, 23), 31: (21, 23)}
    for y, (x0, x1) in wing.items():
        span(a, y, x0, x1, 'white_l' if y < 29 else 'white')
    span(a, 32, 21, 23, 'pink')
    # neck and collar
    put(a, 30, 42, 'skin_s'); put(a, 31, 42, 'skin_s')
    span(a, 43, 28, 31, 'white'); put(a, 31, 43, 'pink')
    # torso
    for y in range(44, 51):
        span(a, y, 27, 31, 'white')
        put(a, 27, y, 'lav')
    for y in range(44, 49):
        put(a, 29, y, 'pink_l')                           # chest ribbon (1 dot, exaggerated)
    put(a, 31, 47, 'pink')                                # pendant
    # sleeves: bell sleeve from the shoulder, hand at the cuff
    sleeve = {44: (25, 26), 45: (24, 26), 46: (24, 26), 47: (23, 26), 48: (23, 26), 49: (22, 26), 50: (22, 26),
              51: (21, 25), 52: (21, 25), 53: (21, 24)}
    for y, (x0, x1) in sleeve.items():
        span(a, y, x0, x1, 'white')
        put(a, x0, y, 'lav')
    span(a, 53, 22, 24, 'lav')
    put(a, 23, 54, 'skin'); put(a, 24, 54, 'skin')
    # skirt: flares rows 51-57, scalloped hem, pink stars
    skirt = {51: 26, 52: 25, 53: 25, 54: 24, 55: 24, 56: 23, 57: 23}
    for y, x0 in skirt.items():
        span(a, y, x0, 31, 'white')
        put(a, x0, y, 'lav')
    for x in range(23, 32):
        put(a, x, 57, 'white_l' if x % 2 else 'lav')     # scallop
    put(a, 26, 54, 'pink')
    # birdcage crinoline under the hem, outside the legs
    for x in range(22, 27, 2):
        put(a, x, 58, 'cage'); put(a, x, 59, 'cage')
    span(a, 58, 22, 26, 'cage')
    # leg: x 27-30, rows 58-71; skin band, white stocking, inner shade
    for y in range(58, 72):
        c = 'skin' if y == 58 else 'white'
        span(a, y, 27, 30, c)
        put(a, 30, y, 'skin_s' if y == 58 else 'lav')
    span(a, 59, 27, 30, 'white_l')                        # stocking top
    # shoe: 5 wide, strap, sole
    for y, c in ((72, 'white'), (73, 'white'), (74, 'white')):
        span(a, y, 26, 30, c)
    put(a, 28, 72, 'pink'); put(a, 29, 72, 'pink')
    span(a, 75, 27, 30, 'line')
    return a


def mirror(a):
    out = a.copy()
    for x in range(CX + 1, W):
        src = 2 * CX - x
        if 0 <= src < W:
            m = a[:, src, 3] > 0
            out[m, x] = a[m, src]
    return out


def asymmetric(a):
    # big lily on the head, screen right (character's left)
    for x, y, c in ((36, 26, 'pink_l'), (35, 27, 'white_l'), (36, 27, 'pink'), (37, 27, 'white_l'),
                    (34, 28, 'white_l'), (35, 28, 'white_l'), (36, 28, 'pink_d'), (37, 28, 'white_l'), (38, 28, 'white_l'),
                    (35, 29, 'white'), (36, 29, 'white_l'), (37, 29, 'white')):
        put(a, x, y, c)
    # waist flower band: three 3x3 flowers from the screen-left waist down to the screen-right hip
    for cx, cy in ((27, 50), (31, 51), (35, 53)):
        for dx, dy in ((-1, 0), (1, 0), (0, -1), (0, 1)):
            put(a, cx + dx, cy + dy, 'white_l')
        for dx, dy in ((-1, -1), (1, -1), (-1, 1), (1, 1)):
            put(a, cx + dx, cy + dy, 'pink_l')
        put(a, cx, cy, 'pink')
    # garter flower, screen-right thigh
    put(a, 34, 60, 'pink'); put(a, 35, 60, 'pink_l'); put(a, 34, 61, 'pink_l')
    return a


def outline(a):
    """Plum outline on the inside edge of the silhouette (keeps the size), as in the FFBE map sprites."""
    out = a.copy()
    op = a[..., 3] > 0
    for y in range(H):
        for x in range(W):
            if not op[y, x]:
                continue
            edge = any(not (0 <= x + dx < W and 0 <= y + dy < H) or not op[y + dy, x + dx]
                       for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1)))
            if edge and tuple(a[y, x, :3]) not in (C['cage'], C['line']):
                out[y, x] = (*C['line'], 255)
    return out


def build():
    return outline(asymmetric(mirror(left_half())))


if __name__ == '__main__':
    Image.fromarray(build(), 'RGBA').save(sys.argv[1])
