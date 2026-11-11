"""Hiyori 8 directions from the Pro Flash rotation of the front master (brief.md "8방향").

  1 detached  keep only the largest 8-connected shape; the rotation drew a pink loop above the head in five views
  2 east/west the generator mirrored the sides: the lily (character's left) showed in west. New east = mirror of
              west, new west = mirror of east (as hiyori_master_v1 did); diagonals are left as drawn
  3 cell      80x80 canvas -> 64x80 (columns 8..71, where the front master sits), soles on spec.SOLE_ROW
  4 tone      colours above luma 208 scaled to 206, same hue (front_master.tone), eye glints only in the front
  5 palette   one palette for all 8 at the hero's 40 colours; the front master's colours are protected
Output: <out>/Hiyori_Idle_<DIR>.png in the spec's direction names. The front stays pixel-identical to the master.
Run: python finalize_rot.py <rotations dir> <out dir>
"""
import os, sys
import numpy as np
from PIL import Image
from scipy import ndimage

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '../../../Tools/HD2D/art'))
import spec
from merge_colors import merge
import front_master

DIRS = {'south': 'FR', 'south-west': 'FLS', 'west': 'LS', 'north-west': 'BLS', 'north': 'BK',
        'north-east': 'BRS', 'east': 'RS', 'south-east': 'FRS'}   # spec.DIRECTIONS, screen sides
X0 = 8


def fit(a):
    """fit_to_cell.fit on an array: soles on spec.SOLE_ROW, feet centred on the pivot, no resampling."""
    solid = a[..., 3] > 0
    sole = np.where(solid.any(1))[0].max()
    feet = np.where(solid[sole - 2:sole + 1].any(0))[0]
    dy, dx = spec.SOLE_ROW - sole, spec.PIVOT[0] - (feet.min() + feet.max() + 1) // 2
    out = np.zeros((spec.CELL_H, spec.CELL_W, 4), np.uint8)
    ys, xs = np.where(solid)
    out[ys + dy, xs + dx] = a[ys, xs]
    return out


def largest_only(a):
    lab, n = ndimage.label(a[..., 3] > 0, structure=np.ones((3, 3)))
    if n <= 1:
        return a, 0
    sizes = ndimage.sum(np.ones_like(lab), lab, range(1, n + 1))
    keep = 1 + int(np.argmax(sizes))
    out = a.copy()
    out[lab != keep] = 0
    return out, int(sizes.sum() - sizes.max())


def main(src, dst):
    os.makedirs(dst, exist_ok=True)
    raw = {}
    for d in DIRS:
        a = np.array(Image.open(os.path.join(src, d + '.png')).convert('RGBA'))
        a[..., 3] = np.where(a[..., 3] >= 128, 255, 0)
        a[a[..., 3] == 0] = 0
        raw[d], removed = largest_only(a)
        print('%-10s detached dots removed: %d' % (d, removed))
    raw['east'], raw['west'] = raw['west'][:, ::-1].copy(), raw['east'][:, ::-1].copy()
    front = raw['south'][:, X0:X0 + spec.CELL_W].copy()
    out = {}
    for d, a in raw.items():
        cell = a[:, X0:X0 + spec.CELL_W]
        assert a[:, :X0, 3].max() == 0 and a[:, X0 + spec.CELL_W:, 3].max() == 0, d + ' spills out of the cell'
        out[d] = front if d == 'south' else front_master.tone(fit(cell))
    keep = {tuple(int(v) for v in c) for c in front[front[..., 3] > 0][:, :3]}
    names = list(out)
    merged, before, after = merge([out[d] for d in names], spec.ROLES['hero']['colors'], keep)
    print('palette %d -> %d (front colours protected: %d)' % (before, after, len(keep)))
    for d, a in zip(names, merged):
        Image.fromarray(a, 'RGBA').save(os.path.join(dst, 'Hiyori_Idle_%s.png' % DIRS[d]))


if __name__ == '__main__':
    main(sys.argv[1], sys.argv[2])
