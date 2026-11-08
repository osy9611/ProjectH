"""Checks character sprite folders against the production spec (spec.py, Docs/HD2D/character-sprite-spec.md).

A character folder holds character.json ({"name", "role": hero|npc, optional "anims", "mirror_exception"})
and one horizontal strip per animation and direction: <name>_<Anim>_<Dir>.png, cells of spec.CELL_W x spec.CELL_H.

Run: python check_sprite.py <character folder or a folder of them> [--sheet out.png]
Prints one line per finding and exits 1 when any error is found. --sheet writes a 4x preview with the guide lines.
"""
import argparse, glob, json, os, sys
import numpy as np
from PIL import Image, ImageDraw

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import spec


class Report:
    def __init__(self):
        self.errors = 0
        self.lines = []

    def err(self, where, msg):
        self.errors += 1
        self.lines.append('ERROR %s: %s' % (where, msg))

    def warn(self, where, msg):
        self.lines.append('WARN  %s: %s' % (where, msg))

    def ok(self, where, msg):
        self.lines.append('OK    %s: %s' % (where, msg))


def frames_of(img):
    return [img[:, i * spec.CELL_W:(i + 1) * spec.CELL_W] for i in range(img.shape[1] // spec.CELL_W)]


def check_frame(rep, where, f, anim, view):
    a = f[..., 3] > 0
    if not a.any():
        rep.err(where, '빈 프레임')
        return None
    rows = np.where(a.any(1))[0]
    cols = np.where(a.any(0))[0]
    if rows.max() > spec.SOLE_ROW:
        rep.err(where, '발바닥 아래 여백(%d행 이후)에 픽셀이 있음: 최하단 %d행' % (spec.SOLE_ROW, rows.max()))
    if rows.min() == 0 or cols.min() == 0 or cols.max() == spec.CELL_W - 1:
        rep.err(where, '셀 가장자리에 닿음(필터링 번짐 위험)')
    height = spec.SOLE_ROW - rows.min() + 1
    # Every pixel needs a 4-neighbour: a lone dot shimmers once the sprite moves.
    p = np.pad(a, 1)
    lone = a & ~(p[:-2, 1:-1] | p[2:, 1:-1] | p[1:-1, :-2] | p[1:-1, 2:])
    if lone.any():
        ys, xs = np.where(lone)
        rep.err(where, '외톨이 픽셀 %d개, 첫 위치 (%d,%d)' % (len(ys), xs[0], ys[0]))
    edge = a & ~(p[:-2, 1:-1] & p[2:, 1:-1] & p[1:-1, :-2] & p[1:-1, 2:])
    lum = spec.luma(*[f[..., i].astype(float) for i in range(3)])
    ring = lum[edge]
    ratio = ((ring <= spec.OUTLINE_LUMA_MAX) & (ring >= spec.OUTLINE_LUMA_MIN)).mean()
    if ratio < spec.OUTLINE_RATIO_MIN:
        rep.err(where, '외곽선 조건(밝기 %d~%d) 충족 %.0f%% < %.0f%%' % (spec.OUTLINE_LUMA_MIN, spec.OUTLINE_LUMA_MAX, ratio * 100, spec.OUTLINE_RATIO_MIN * 100))
    # Only standing frames: a walking foot lifts off and shifts the centre on purpose.
    bright = (lum[a] > spec.TONE_LUMA_MAX).mean()
    if bright > spec.BRIGHT_RATIO_MAX:
        rep.err(where, '밝기 %d 초과 픽셀 %.1f%% > %.0f%% (낮 장면에서 하얗게 날아감)' % (spec.TONE_LUMA_MAX, bright * 100, spec.BRIGHT_RATIO_MAX * 100))
    if view in ('FR', 'BK') and anim == 'Idle':
        foot = np.where(a[spec.SOLE_ROW - 2:spec.SOLE_ROW + 1].any(0))[0]
        centre = (foot.min() + foot.max() + 1) / 2
        if abs(centre - spec.PIVOT[0]) > 1.5:
            rep.err(where, '정면·후면 발 중심 x=%.1f, 피벗 %d에서 벗어남' % (centre, spec.PIVOT[0]))
    return rows.max(), height


def check_character(folder, rep):
    meta_path = os.path.join(folder, 'character.json')
    if not os.path.exists(meta_path):
        rep.err(folder, 'character.json 없음')
        return []
    meta = json.load(open(meta_path, encoding='utf-8'))
    name, role = meta['name'], meta['role']
    if role not in spec.ROLES:
        rep.err(name, '알 수 없는 role: %s' % role)
        return []
    want = spec.ROLES[role]['anims']
    anims = meta.get('anims', list(want))
    for missing in set(want) - set(anims):
        rep.warn(name, '%s 애니메이션은 이 캐릭터의 범위에서 제외됨(character.json anims)' % missing)
    views = list(spec.DIRECTIONS) + (['RS'] if meta.get('mirror_exception') else [])
    palette = set()
    strips = []
    for anim in anims:
        for view in views:
            path = os.path.join(folder, '%s_%s_%s.png' % (name, anim, view))
            where = os.path.basename(path)
            if not os.path.exists(path):
                rep.err(where, '파일 없음')
                continue
            im = Image.open(path)
            img = np.array(im.convert('RGBA'))
            h, w = img.shape[:2]
            if h != spec.CELL_H or w % spec.CELL_W:
                rep.err(where, '크기 %dx%d, 높이 %d·폭 %d의 배수여야 함' % (w, h, spec.CELL_H, spec.CELL_W))
                continue
            n = w // spec.CELL_W
            if anim in want and n != want[anim][0]:
                rep.err(where, '프레임 %d개, %s 역할 기준 %d개' % (n, role, want[anim][0]))
            alpha = np.unique(img[..., 3])
            if not set(alpha.tolist()) <= {0, 255}:
                rep.err(where, '반투명 알파 사용(0과 255만 허용)')
            palette |= {tuple(c) for c in img[img[..., 3] > 0][:, :3].tolist()}
            soles = []
            for i, f in enumerate(frames_of(img)):
                r = check_frame(rep, '%s#%d' % (where, i), f, anim, view)
                if r:
                    soles.append(r[0])
            if soles and spec.SOLE_ROW not in soles:
                rep.err(where, '발이 %d행에 닿는 프레임이 하나도 없음' % spec.SOLE_ROW)
            strips.append((where, img))
    limit = spec.ROLES[role]['colors']
    (rep.err if len(palette) > limit else rep.ok)(name, '색 수 %d / %s 기준 %d' % (len(palette), role, limit))
    return strips


def write_sheet(strips, out):
    scale = 4
    rows = [img for _, img in strips]
    W = max(i.shape[1] for i in rows) * scale
    H = sum(i.shape[0] for i in rows) * scale
    sheet = Image.new('RGBA', (W, H), (118, 128, 108, 255))
    d = ImageDraw.Draw(sheet)
    y = 0
    for img in rows:
        sheet.alpha_composite(Image.fromarray(img, 'RGBA').resize((img.shape[1] * scale, img.shape[0] * scale), Image.NEAREST), (0, y))
        for i in range(img.shape[1] // spec.CELL_W):
            x0 = i * spec.CELL_W * scale
            d.rectangle([x0, y, x0 + spec.CELL_W * scale - 1, y + spec.CELL_H * scale - 1], outline=(70, 80, 64))
            d.line([x0, y + spec.PIVOT[1] * scale, x0 + spec.CELL_W * scale, y + spec.PIVOT[1] * scale], fill=(220, 60, 60))
            d.line([x0 + spec.PIVOT[0] * scale, y, x0 + spec.PIVOT[0] * scale, y + spec.CELL_H * scale], fill=(60, 120, 220))
        y += img.shape[0] * scale
    sheet.save(out)


def main():
    sys.stdout.reconfigure(encoding='utf-8')
    ap = argparse.ArgumentParser()
    ap.add_argument('path')
    ap.add_argument('--sheet')
    args = ap.parse_args()
    folders = [args.path] if os.path.exists(os.path.join(args.path, 'character.json')) else \
        sorted(os.path.dirname(p) for p in glob.glob(os.path.join(args.path, '*', 'character.json')))
    rep = Report()
    strips = []
    for folder in folders:
        strips += check_character(folder, rep)
    print('\n'.join(rep.lines))
    print('검사 %d명, 오류 %d건' % (len(folders), rep.errors))
    if args.sheet and strips:
        write_sheet(strips, args.sheet)
    sys.exit(1 if rep.errors else 0)


if __name__ == '__main__':
    main()
