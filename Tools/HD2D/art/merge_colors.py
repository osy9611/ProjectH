"""Brings a character's sprites under the role's colour limit by merging the closest colours.

All sprites of the character share one palette: repeatedly, the rarest unprotected colour is folded into its
nearest neighbour (luma-weighted RGB distance), until the count is at the limit. Protected colours (outlines,
eye colours) never move. Only dots whose colour was merged change.
Run: python merge_colors.py <character folder> [limit] [--protect r,g,b ...]
"""
import glob, json, os, sys
import numpy as np
from PIL import Image

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import spec

WEIGHT = np.array([0.30, 0.59, 0.11])


def merge(images, limit, protect):
    counts = {}
    for a in images:
        cols, n = np.unique(a[a[..., 3] > 0][:, :3], axis=0, return_counts=True)
        for c, k in zip(map(tuple, cols.tolist()), n):
            counts[c] = counts.get(c, 0) + int(k)
    target = {c: c for c in counts}
    alive = dict(counts)
    while len(alive) > limit:
        movable = [c for c in alive if c not in protect]
        if not movable:
            break
        rare = min(movable, key=lambda c: alive[c])
        others = [c for c in alive if c != rare]
        d = [((np.array(rare) - np.array(o)) ** 2 * WEIGHT).sum() for o in others]
        into = others[int(np.argmin(d))]
        alive[into] += alive.pop(rare)
        for c, t in target.items():
            if t == rare:
                target[c] = into
    out = []
    for a in images:
        b = a.copy()
        for y, x in zip(*np.where(a[..., 3] > 0)):
            b[y, x, :3] = target[tuple(int(v) for v in a[y, x, :3])]
        out.append(b)
    return out, len(counts), len(alive)


if __name__ == '__main__':
    folder = sys.argv[1]
    meta = json.load(open(os.path.join(folder, 'character.json'), encoding='utf-8'))
    args = sys.argv[2:]
    limit = spec.ROLES[meta['role']]['colors']
    protect = set()
    if args and not args[0].startswith('--'):
        limit = int(args.pop(0))
    if args and args[0] == '--protect':
        protect = {tuple(int(v) for v in s.split(',')) for s in args[1:]}
    paths = sorted(glob.glob(os.path.join(folder, '%s_*.png' % meta['name'])))
    images = [np.array(Image.open(p).convert('RGBA')) for p in paths]
    out, before, after = merge(images, limit, protect)
    for p, b in zip(paths, out):
        Image.fromarray(b, 'RGBA').save(p)
    print('colours %d -> %d across %d sprites' % (before, after, len(paths)))
