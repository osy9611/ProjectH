"""Brings Hiyori's twin tails closer to the concept: the hair outside the body is pulled toward it between the
shoulders and the hips (the curls below keep their width), and the teal band becomes the concept's lavender and
pale gold.

Rows 43-55 keep 70% of their width outside the edge, rows 56-60 ease back to 100%. Edges per view: FR the columns
the mirrored arms end on, BK the dress sides, LS the back of the body (the tail flows out behind it).
Nothing inside the edges moves.
Run: python tails.py <FR|LS|BK> <in.png> <out.png>
"""
import colorsys, sys
import numpy as np
from PIL import Image

EDGES = {'FR': (17, 46), 'BK': (21, 42), 'LS': (None, 30)}  # (left edge, right edge); None = no hair on that side
TEAL_TO = {(165, 210, 212): (200, 185, 225)}  # teal -> concept lavender
GOLD = (238, 218, 196)                         # concept pale gold for light teal tints


def factor(y):
    if y < 43 or y > 60:
        return 1.0
    if y <= 55:
        return 0.7
    return 0.7 + 0.3 * (y - 55) / 5


def squeeze(a, left, right):
    out = a.copy()
    for y in range(a.shape[0]):
        f = factor(y)
        if f == 1.0:
            continue
        if left is not None:
            # x in [0, left) is pulled toward the left edge
            out[y, :left] = 0
            for x in range(left):
                src = left - (left - x) / f
                if src >= 0:
                    out[y, x] = a[y, int(src)]
        if right is not None:
            out[y, right + 1:] = 0
            for x in range(right + 1, a.shape[1]):
                src = right + (x - right) / f
                if src < a.shape[1]:
                    out[y, x] = a[y, int(src)]
    return out


def recolour(a):
    for y, x in zip(*np.where(a[..., 3] > 0)):
        c = tuple(int(v) for v in a[y, x, :3])
        if c in TEAL_TO:
            a[y, x, :3] = TEAL_TO[c]
            continue
        h, l, s = colorsys.rgb_to_hls(*[v / 255 for v in c])
        if 0.42 < h < 0.56 and l > 0.75 and s > 0.1:
            a[y, x, :3] = GOLD
    return a


if __name__ == '__main__':
    view, src, dst = sys.argv[1:4]
    a = np.array(Image.open(src).convert('RGBA'))
    Image.fromarray(recolour(squeeze(a, *EDGES[view])), 'RGBA').save(dst)
