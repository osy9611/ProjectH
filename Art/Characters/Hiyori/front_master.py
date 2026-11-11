"""Hiyori front master from edit G (brief.md "정면 마스터"), following the hiyori_master_v1 recipe: the front is
finished here, then given to PixelLab only as the first frame to rotate.

G keeps D6's proportions (height 50, legs 18) and already has an FFBE face (chin->eye 7, 2x3 eyes, symmetric),
so the face shape is left alone. Only colours change:
  1 hair     G drifted to cream-yellow; map the hair ramp back to the concept's ash beige (#cbbcad family)
  2 outline  near-black (luma < 30) -> plum: silhouette edge #3b2347, lashes #3e1a33, inner lines #5a3f6c
             (master_v1: no black, inner lines one step lighter)
  3 tone     every colour brighter than luma 208 keeps its hue and is scaled down to luma 206; only the eye
             glints stay bright (spec: bright <= 1%). A blend toward lavender was tried first and tinted the
             whole dress purple
  4 palette  back to the hero's 40 colours, eye colours protected
Run: python front_master.py <G raw.png> <out.png>
"""
import os, sys
import numpy as np
from PIL import Image

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '../../../Tools/HD2D/art'))
import spec
from merge_colors import merge

HAIR = {  # G colour -> ash beige (luma <= 206)
    (0xef, 0xe0, 0xb7): (0xd6, 0xca, 0xbe), (0xf3, 0xee, 0xdd): (0xdc, 0xd2, 0xca),
    (0xd3, 0xc0, 0xa3): (0xc0, 0xb2, 0xa8), (0xc3, 0xac, 0x9e): (0xae, 0x9e, 0x98),
    (0xa6, 0x90, 0x82): (0x96, 0x84, 0x82), (0xe4, 0xcf, 0xbc): (0xd0, 0xc2, 0xb8),
    (0xc0, 0xc0, 0xae): (0xb8, 0xb4, 0xae),
}
EDGE, LASH, INNER = (0x3b, 0x23, 0x47), (0x3e, 0x1a, 0x33), (0x5a, 0x3f, 0x6c)
FACE_BOX = (26, 34, 37, 39)          # x0, y0, x1, y1: lashes inside this box
GLINTS = ((28, 37), (28, 38), (35, 37), (35, 38))
TONE_MAX, TONE_TARGET = 208, 206


def lum(c):
    return spec.luma(*[float(v) for v in c])


def recolour_hair(a):
    out = a.copy()
    for src, dst in HAIR.items():
        out[np.all(a[..., :3] == src, axis=-1) & (a[..., 3] > 0)] = (*dst, 255)
    return out


def outline(a):
    out = a.copy()
    h, w = a.shape[:2]
    op = a[..., 3] > 0
    L = spec.luma(*[a[..., i].astype(float) for i in range(3)])
    x0, y0, x1, y1 = FACE_BOX
    for y in range(h):
        for x in range(w):
            if not op[y, x] or L[y, x] >= 30:
                continue
            edge = any(not (0 <= x + dx < w and 0 <= y + dy < h) or not op[y + dy, x + dx]
                       for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1)))
            c = EDGE if edge else (LASH if x0 <= x <= x1 and y0 <= y <= y1 else INNER)
            out[y, x] = (*c, 255)
    return out


def tone(a):
    out = a.copy()
    keep = {(x, y) for x, y in GLINTS}
    cols = np.unique(a[a[..., 3] > 0][:, :3], axis=0)
    mapping = {}
    for c in map(tuple, cols.tolist()):
        if lum(c) <= TONE_MAX:
            continue
        # same hue, lower value: scale toward black (a lavender blend tinted the whole dress purple)
        k = TONE_TARGET / lum(c)
        mapping[c] = tuple(int(round(v * k)) for v in c)
    for y, x in zip(*np.nonzero(a[..., 3] > 0)):
        c = tuple(int(v) for v in a[y, x, :3])
        if c in mapping and (x, y) not in keep:
            out[y, x, :3] = mapping[c]
    return out


def master(a):
    a = tone(outline(recolour_hair(a)))
    eye_cols = {tuple(int(v) for v in a[y, x, :3]) for x0, y0, x1, y1 in [FACE_BOX]
                for y in range(y0, y1 + 1) for x in range(x0, x1 + 1) if a[y, x, 3]}
    (a,), before, after = merge([a], spec.ROLES['hero']['colors'], eye_cols)
    print('colours %d -> %d' % (before, after))
    return a


if __name__ == '__main__':
    a = np.array(Image.open(sys.argv[1]).convert('RGBA'))
    a[..., 3] = np.where(a[..., 3] >= 128, 255, 0)
    a[a[..., 3] == 0] = 0
    Image.fromarray(master(a), 'RGBA').save(sys.argv[2])
