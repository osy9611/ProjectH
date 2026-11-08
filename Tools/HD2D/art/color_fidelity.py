"""How closely a sprite's colours follow its concept, material by material.

For each named sample point on the concept (median of a 7x7 window) it reports
  same-spot dE: CIE76 Lab distance to the sprite colour at the mapped position (5x5 median, line colours left out),
  nearest dE:   distance to the closest colour anywhere in the sprite palette,
and how much of the concept's coloured area the sprite palette covers within dE 10 and 20.
Concept and sprite are aligned by crown, sole and centre line.
Run: python color_fidelity.py <points.json> <sprite.png>
points.json: {"concept", "crown", "sole", "centre_x", "points": {name: [x, y]}}
"""
import json, sys
import numpy as np
from PIL import Image


def lab(rgb):
    c = np.asarray(rgb, float) / 255
    c = np.where(c > 0.04045, ((c + 0.055) / 1.055) ** 2.4, c / 12.92)
    xyz = c @ np.array([[0.4124, 0.2126, 0.0193], [0.3576, 0.7152, 0.1192], [0.1805, 0.0722, 0.9505]])
    xyz /= np.array([0.9505, 1.0, 1.089])
    f = np.where(xyz > 0.008856, np.cbrt(xyz), 7.787 * xyz + 16 / 116)
    return np.stack([116 * f[..., 1] - 16, 500 * (f[..., 0] - f[..., 1]), 200 * (f[..., 1] - f[..., 2])], -1)


def run(points_path, sprite_path):
    cfg = json.load(open(points_path, encoding='utf-8'))
    con = np.array(Image.open(cfg['concept']).convert('RGB')).astype(float)
    spr = np.array(Image.open(sprite_path).convert('RGBA')).astype(float)
    solid = spr[..., 3] > 0
    ys, xs = np.where(solid)
    s = (ys.max() - ys.min()) / (cfg['sole'] - cfg['crown'])
    cx = (xs.min() + xs.max()) / 2
    palette = np.unique(spr[solid][:, :3], axis=0)
    plab = lab(palette)
    rows = []
    for name, (x, y) in cfg['points'].items():
        c = np.median(con[y - 3:y + 4, x - 3:x + 4].reshape(-1, 3), axis=0)
        r = int(round(ys.max() - (cfg['sole'] - y) * s))
        q = int(round(cx + (x - cfg['centre_x']) * s))
        # 5x5 window without line colours (luma under 90): the spot should be the material, not its outline
        win = spr[r - 2:r + 3, q - 2:q + 3]
        win = win[win[..., 3] > 0][:, :3]
        win = win[(win @ np.array([0.299, 0.587, 0.114])) >= 90]
        same = np.median(win, axis=0) if len(win) else None
        cl = lab(c)
        d_same = float(np.linalg.norm(cl - lab(same))) if same is not None else None
        d_near = float(np.linalg.norm(plab - cl, axis=1).min())
        rows.append((name, c.astype(int).tolist(), None if same is None else same.astype(int).tolist(), d_same, d_near))
    # coverage of the concept's figure colours
    bg = np.median(con[:, :8], axis=1)[:, None, :]
    fig = np.abs(con - bg).sum(-1) > 36
    sample = con[fig][::7]
    dmin = np.linalg.norm(lab(sample)[:, None, :] - plab[None], axis=-1).min(1)
    return rows, (dmin < 10).mean(), (dmin < 20).mean(), len(palette)


if __name__ == '__main__':
    rows, c10, c20, n = run(sys.argv[1], sys.argv[2])
    sys.stdout.reconfigure(encoding='utf-8')
    print('%-13s %-16s %-16s %8s %8s' % ('material', 'concept', 'sprite', 'same dE', 'near dE'))
    for name, c, s, ds, dn in rows:
        print('%-13s %-16s %-16s %8s %8.1f' % (name, c, s, '-' if ds is None else '%.1f' % ds, dn))
    print('palette %d colours | concept area within dE10: %.0f%%, within dE20: %.0f%%' % (n, c10 * 100, c20 * 100))
