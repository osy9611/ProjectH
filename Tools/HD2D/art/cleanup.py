"""Runs the standard clean-up on a batch of generated sprites in one go, keeping every step.

Steps, in order (each one is an existing tool):
  00_alpha    binarise                alpha cut at 128 (generators leave soft edges)
  01_fit      fit_to_cell.fit         soles on spec.SOLE_ROW, feet centred on the pivot, no resampling
  02_clean    recolor_outline.clean   alpha to 0/255, lone dots removed, near-black dots to a dark tint
  03_selout   selout.selout           selective coloured outline (FFBE map sprite rule)
  04_palette  merge_colors.merge      one palette across the whole batch, at most the role's colour count

Every step writes its own folder under <out>, the final images go to <out> itself, and <out>/lineage.json records
the inputs (with SHA-1), the steps, the options and the counts, so the run can be repeated exactly.
Run: python cleanup.py <src dir or png> <out dir> [--role hero|npc] [--colors N] [--protect r,g,b ...]
"""
import argparse, datetime, glob, hashlib, json, os, shutil, sys
import numpy as np
from PIL import Image

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import spec
from fit_to_cell import fit
from recolor_outline import clean
from selout import selout
from merge_colors import merge

ALPHA_CUT = 128


def sha1(path):
    return hashlib.sha1(open(path, 'rb').read()).hexdigest()


def save(images, folder):
    os.makedirs(folder, exist_ok=True)
    for name, a in images.items():
        Image.fromarray(a, 'RGBA').save(os.path.join(folder, name))


def binarise(path, folder):
    """PixelLab output may carry soft alpha; the spec allows 0 or 255 only."""
    a = np.array(Image.open(path).convert('RGBA'))
    a[..., 3] = np.where(a[..., 3] >= ALPHA_CUT, 255, 0)
    a[a[..., 3] == 0] = 0
    tmp = os.path.join(folder, os.path.basename(path))
    Image.fromarray(a, 'RGBA').save(tmp)
    return tmp


def main():
    p = argparse.ArgumentParser()
    p.add_argument('src')
    p.add_argument('out')
    p.add_argument('--role', default='hero', choices=sorted(spec.ROLES))
    p.add_argument('--colors', type=int)
    p.add_argument('--protect', nargs='*', default=[])
    o = p.parse_args()
    srcs = [o.src] if os.path.isfile(o.src) else sorted(glob.glob(os.path.join(o.src, '*.png')))
    if not srcs:
        raise SystemExit('no PNG in %s' % o.src)
    if os.path.exists(o.out) and os.listdir(o.out):
        raise SystemExit('%s is not empty; pick a new folder so earlier runs stay as they were' % o.out)
    protect = {tuple(int(v) for v in s.split(',')) for s in o.protect}
    limit = o.colors or spec.ROLES[o.role]['colors']
    log = {'tool': 'Tools/HD2D/art/cleanup.py', 'date': datetime.datetime.now().isoformat(timespec='seconds'),
           'inputs': {os.path.basename(s): {'path': os.path.abspath(s), 'sha1': sha1(s)} for s in srcs},
           'options': {'role': o.role, 'colors': limit, 'protect': sorted(protect)}, 'steps': []}

    os.makedirs(o.out, exist_ok=True)
    stage = os.path.join(o.out, '00_input')
    os.makedirs(stage, exist_ok=True)
    for s in srcs:
        shutil.copy2(s, stage)
    os.makedirs(os.path.join(o.out, '00_alpha'), exist_ok=True)
    work = {os.path.basename(s): fit(binarise(s, os.path.join(o.out, '00_alpha'))) for s in srcs}
    save(work, os.path.join(o.out, '01_fit'))
    log['steps'].append({'step': '01_fit', 'cell': [spec.CELL_W, spec.CELL_H], 'sole_row': spec.SOLE_ROW})

    counts = {}
    for n, a in work.items():
        work[n], lone, black = clean(a)
        counts[n] = {'lone_removed': lone, 'near_black_recoloured': black}
    save(work, os.path.join(o.out, '02_clean'))
    log['steps'].append({'step': '02_clean', 'per_image': counts})

    counts = {}
    for n, a in work.items():
        work[n], changed = selout(a, protect)
        counts[n] = {'outline_dots': changed}
    save(work, os.path.join(o.out, '03_selout'))
    log['steps'].append({'step': '03_selout', 'per_image': counts})

    names = list(work)
    merged, before, after = merge([work[n] for n in names], limit, protect)
    work = dict(zip(names, merged))
    save(work, os.path.join(o.out, '04_palette'))
    save(work, o.out)
    log['steps'].append({'step': '04_palette', 'colours_before': before, 'colours_after': after})

    json.dump(log, open(os.path.join(o.out, 'lineage.json'), 'w', encoding='utf-8'), ensure_ascii=False, indent=1)
    print('%d sprites -> %s, colours %d -> %d' % (len(work), o.out, before, after))


if __name__ == '__main__':
    main()
