"""Moves a generated sprite onto the spec cell without resampling: soles on spec.SOLE_ROW, feet centred on the pivot.

Run: python fit_to_cell.py <source.png> <out.png>
The feet centre is the middle of the opaque columns in the bottom three rows of the figure.
"""
import os, sys
import numpy as np
from PIL import Image

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import spec


def fit(src):
    a = np.array(Image.open(src).convert('RGBA'))
    solid = a[..., 3] > 0
    rows = np.where(solid.any(1))[0]
    sole = rows.max()
    feet = np.where(solid[sole - 2:sole + 1].any(0))[0]
    centre = (feet.min() + feet.max() + 1) // 2
    out = np.zeros((spec.CELL_H, spec.CELL_W, 4), np.uint8)
    dy, dx = spec.SOLE_ROW - sole, spec.PIVOT[0] - centre
    ys, xs = np.where(solid)
    ty, tx = ys + dy, xs + dx
    inside = (ty >= 1) & (ty < spec.CELL_H) & (tx >= 1) & (tx < spec.CELL_W - 1)
    if not inside.all():
        raise SystemExit('%s: %d dots fall outside the cell' % (src, (~inside).sum()))
    out[ty, tx] = a[ys, xs]
    return out


if __name__ == '__main__':
    Image.fromarray(fit(sys.argv[1]), 'RGBA').save(sys.argv[2])
