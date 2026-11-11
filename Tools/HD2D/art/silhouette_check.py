"""Silhouette checks that check_sprite.py does not cover: width against height, pixels cut off from the body,
and height drift between directions.

Thresholds are proposals from Docs/HD2D/art-pipeline/README.md (FFBE 22-character measurements, 2026-11-10).
Move them into spec.py once the spec session adopts them.

Run: python silhouette_check.py <character folder> [--anim Idle] [--frame 0]
The folder holds <name>_<Anim>_<Dir>.png strips of spec.CELL_W x spec.CELL_H cells (the check_sprite.py layout).
Exits 1 when any check fails.
"""
import argparse, glob, os, re, sys
import numpy as np
from PIL import Image
from scipy import ndimage

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import spec

# FFBE 22 characters, first walk frame, ground shadow removed. Front: width/height mean 0.61, max 0.78.
# Back: mean 0.62, 90th percentile 0.79. Fronts have no detached pixels; sides only for held weapons.
WIDTH_RATIO_MAX = {'FR': 0.78, 'BK': 0.80}
DETACHED_MAX = 0  # pixels outside the largest 8-connected part; a prop that is meant to float goes in character.json
HEIGHT_SPREAD_MAX = 3  # tallest minus shortest direction, same animation frame


def frame(path, index):
    img = np.array(Image.open(path).convert('RGBA'))
    return img[:, index * spec.CELL_W:(index + 1) * spec.CELL_W]


def measure(f):
    solid = f[..., 3] == 255
    ys, xs = np.nonzero(solid)
    labels, _ = ndimage.label(solid, structure=np.ones((3, 3)))
    sizes = np.bincount(labels.ravel())[1:]
    return dict(height=int(ys.max() - ys.min() + 1), width=int(xs.max() - xs.min() + 1), area=int(solid.sum()),
                detached=int(sizes.sum() - sizes.max()), parts=sorted(sizes.tolist(), reverse=True)[1:])


def main():
    sys.stdout.reconfigure(encoding='utf-8')
    ap = argparse.ArgumentParser()
    ap.add_argument('folder')
    ap.add_argument('--anim', default='Idle')
    ap.add_argument('--frame', type=int, default=0)
    args = ap.parse_args()
    found = {}
    for path in glob.glob(os.path.join(args.folder, '*_%s_*.png' % args.anim)):
        m = re.search(r'_%s_([A-Z]+)\.png$' % args.anim, path)
        if m and m.group(1) in spec.DIRECTIONS:
            found[m.group(1)] = measure(frame(path, args.frame))
    if not found:
        sys.exit('%s: %s 스트립이 없음' % (args.folder, args.anim))
    errors = 0
    print('| 방향 | 키 | 폭 | 폭/키 | 면적 | 떨어진 픽셀 |')
    print('|---|---|---|---|---|---|')
    for d in spec.DIRECTIONS:
        if d in found:
            r = found[d]
            print('| %s | %d | %d | %.2f | %d | %d %s |' % (d, r['height'], r['width'], r['width'] / r['height'], r['area'], r['detached'], r['parts'] or ''))
    for d, r in sorted(found.items()):
        ratio = r['width'] / r['height']
        if d in WIDTH_RATIO_MAX and ratio > WIDTH_RATIO_MAX[d]:
            errors += 1
            print('ERROR %s: 폭/키 %.2f > %.2f (FFBE 최대 기준)' % (d, ratio, WIDTH_RATIO_MAX[d]))
        if r['detached'] > DETACHED_MAX:
            errors += 1
            print('ERROR %s: 몸에서 떨어진 픽셀 %d개, 덩어리 %s' % (d, r['detached'], r['parts']))
    heights = [r['height'] for r in found.values()]
    if max(heights) - min(heights) > HEIGHT_SPREAD_MAX:
        errors += 1
        print('ERROR 방향 간 키 차이 %d > %d' % (max(heights) - min(heights), HEIGHT_SPREAD_MAX))
    print('검사 %d방향, 오류 %d건' % (len(found), errors))
    sys.exit(1 if errors else 0)


if __name__ == '__main__':
    main()
