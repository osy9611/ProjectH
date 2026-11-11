"""One-image review board for showing sprite candidates to a person.

Run:
  python review_board.py --name <Name> --variant A=Art/Characters/<Name> --variant B=drafts/<name>_fr_b.png
      [--concept <concept.png>] [--q "망토 밑단 A/B 중 선택"] [--out board.png]

--variant takes a character folder (<name>_<Anim>_<Dir>.png strips, frame --frame of --anim)
or a single PNG, used as the front view. Change one thing between variants.

Board: (1) concept next to each variant's front, (2) every drawn direction, (3) silhouettes,
(4) 1x and game scale (spec.SCREEN_DOT_PX screen pixels per dot), (5) four backdrops, (6) numbers with
out-of-rule values in red, (7) the questions. Every enlargement is an integer nearest-neighbour scale.
Default output: <first variant folder>/review/<name>_review_v<N>.png, N counting up.
A board that shows another game's sprites (--ref) is a working image: do not commit it.
"""
import argparse, datetime, glob, os, re, sys
import numpy as np
from PIL import Image, ImageDraw, ImageFont

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import spec
from silhouette_check import WIDTH_RATIO_MAX, DETACHED_MAX, measure

BG, PANEL, INK, SUB, BAD = (34, 34, 42), (48, 48, 58), (235, 235, 240), (160, 160, 175), (255, 96, 96)
BACKDROPS = [('어두운 배경', (30, 30, 38)), ('밝은 배경', (222, 218, 206)), ('풀밭', (96, 128, 72)), ('석조 바닥', (128, 118, 104))]
WIDTH = 1700


def font(size, bold=False):
    path = r'C:\Windows\Fonts\malgunbd.ttf' if bold else r'C:\Windows\Fonts\malgun.ttf'
    return ImageFont.truetype(path, size) if os.path.exists(path) else ImageFont.load_default()


F_TITLE, F_H, F_TXT = font(30, True), font(20, True), font(15)


def load_variant(path, anim, index):
    """Returns {direction: RGBA cell} for a folder, {'FR': image} for a PNG."""
    if os.path.isdir(path):
        views = {}
        for p in glob.glob(os.path.join(path, '*_%s_*.png' % anim)):
            m = re.search(r'_%s_([A-Z]+)\.png$' % anim, p)
            if m and m.group(1) in spec.DIRECTIONS:
                img = Image.open(p).convert('RGBA')
                views[m.group(1)] = img.crop((index * spec.CELL_W, 0, (index + 1) * spec.CELL_W, img.height))
        if not views:
            sys.exit('%s: %s 스트립이 없음' % (path, anim))
        return {d: views[d] for d in spec.DIRECTIONS if d in views}
    return {'FR': Image.open(path).convert('RGBA')}


def on(img, color, scale):
    cell = Image.new('RGBA', img.size, color + (255,))
    cell.alpha_composite(img)
    return cell.convert('RGB').resize((img.width * scale, img.height * scale), Image.NEAREST)


def silhouette(img, scale):
    a = np.array(img)
    out = np.full(a.shape[:2] + (3,), 222, np.uint8)
    out[a[..., 3] == 255] = (30, 30, 38)
    return Image.fromarray(out).resize((img.width * scale, img.height * scale), Image.NEAREST)


def numbers(views):
    """Rows of (label, value text, out of rule)."""
    rows = []
    palette = set()
    for img in views.values():
        a = np.array(img)
        palette |= {tuple(c) for c in a[a[..., 3] == 255][:, :3].tolist()}
    for d, img in views.items():
        r = measure(np.array(img))
        ratio = r['width'] / r['height']
        a = np.array(img).astype(float)
        lum = spec.luma(a[..., 0], a[..., 1], a[..., 2])[a[..., 3] == 255]
        bright = (lum > spec.TONE_LUMA_MAX).mean()
        rows.append(('%s 키·폭' % d, '%d · %d (폭/키 %.2f)' % (r['height'], r['width'], ratio), ratio > WIDTH_RATIO_MAX.get(d, 9)))
        rows.append(('%s 떨어진 픽셀' % d, str(r['detached']), r['detached'] > DETACHED_MAX))
        rows.append(('%s 루마 %d 초과' % (d, spec.TONE_LUMA_MAX), '%.1f%%' % (bright * 100), bright > spec.BRIGHT_RATIO_MAX))
    rows.append(('색 수(표시된 방향 합계)', str(len(palette)), len(palette) > spec.ROLES['hero']['colors']))
    return rows


class Board:
    def __init__(self):
        self.parts = []  # (image, x, y)
        self.y = 20

    def text(self, s, f, color=INK, x=20, dy=None):
        self.parts.append(('text', (x, self.y), s, f, color))
        self.y += dy if dy is not None else f.size + 10

    def row(self, images, gap=16, x=20):
        h = 0
        for img in images:
            self.parts.append(('img', (x, self.y), img))
            x += img.width + gap
            h = max(h, img.height)
        self.y += h + 14

    def render(self):
        out = Image.new('RGB', (WIDTH, self.y + 20), BG)
        d = ImageDraw.Draw(out)
        for p in self.parts:
            if p[0] == 'img':
                out.paste(p[2], p[1])
            else:
                d.text(p[1], p[2], font=p[3], fill=p[4])
        return out


def labelled(img, label):
    out = Image.new('RGB', (max(img.width, 120), img.height + 24), BG)
    ImageDraw.Draw(out).text((0, 0), label, font=F_TXT, fill=SUB)
    out.paste(img, (0, 24))
    return out


def main():
    sys.stdout.reconfigure(encoding='utf-8')
    ap = argparse.ArgumentParser()
    ap.add_argument('--name', required=True)
    ap.add_argument('--variant', action='append', required=True, help='LABEL=folder or png')
    ap.add_argument('--concept')
    ap.add_argument('--ref', action='append', default=[], help='reference PNG shown at the same dot scale')
    ap.add_argument('--q', action='append', default=[])
    ap.add_argument('--anim', default='Idle')
    ap.add_argument('--frame', type=int, default=0)
    ap.add_argument('--out')
    args = ap.parse_args()
    variants = []
    for v in args.variant:
        label, path = v.split('=', 1)
        variants.append((label, path, load_variant(path, args.anim, args.frame)))

    b = Board()
    b.text('%s 리뷰 보드 · %s · %s %d프레임' % (args.name, datetime.date.today(), args.anim, args.frame), F_TITLE)
    b.text('안: ' + ' / '.join('%s = %s' % (l, p) for l, p, _ in variants), F_TXT, SUB)

    b.text('① 시안과 정면 (×5)', F_H)
    row = []
    if args.concept:
        c = Image.open(args.concept).convert('RGB')
        h = spec.CELL_H * 5
        row.append(labelled(c.resize((int(c.width * h / c.height), h), Image.LANCZOS), '시안'))
    for label, _, views in variants:
        row.append(labelled(on(views['FR'], BACKDROPS[0][1], 5), label + ' FR'))
    b.row(row)

    b.text('② 방향 (×3)', F_H)
    for label, _, views in variants:
        b.row([labelled(on(img, BACKDROPS[0][1], 3), '%s %s' % (label, d)) for d, img in views.items()], gap=8)

    b.text('③ 실루엣 (×3)', F_H)
    b.row([labelled(silhouette(img, 3), '%s %s' % (label, d)) for label, _, views in variants for d, img in views.items()
           if d in ('FR', 'LS', 'BK')], gap=8)

    b.text('④ 1배와 게임 배율(도트 1개 = 화면 %dpx)' % spec.SCREEN_DOT_PX, F_H)
    row = []
    for label, _, views in variants:
        row.append(labelled(on(views['FR'], BACKDROPS[3][1], 1), label + ' 1배'))
        row.append(labelled(on(views['FR'], BACKDROPS[3][1], spec.SCREEN_DOT_PX), label + ' 게임'))
    for path in args.ref:
        row.append(labelled(on(Image.open(path).convert('RGBA'), BACKDROPS[3][1], spec.SCREEN_DOT_PX), os.path.basename(path)))
    b.row(row)

    b.text('⑤ 배경 (×3)', F_H)
    b.row([labelled(on(views['FR'], color, 3), '%s %s' % (label, name)) for label, _, views in variants for name, color in BACKDROPS], gap=8)

    b.text('⑥ 수치 (빨강 = 기준 밖. 폭/키·떨어진 픽셀은 silhouette_check.py 제안값)', F_H)
    for label, _, views in variants:
        b.text(label, F_TXT, INK)
        for name, value, bad in numbers(views):
            b.text('    %s: %s' % (name, value), F_TXT, BAD if bad else SUB, dy=20)
        b.y += 6

    if args.q:
        b.text('⑦ 리뷰 질문', F_H)
        for i, q in enumerate(args.q, 1):
            b.text('%d. %s' % (i, q), F_TXT, dy=22)

    out = args.out
    if not out:
        first = variants[0][1]
        folder = os.path.join(first if os.path.isdir(first) else os.path.dirname(first), 'review')
        os.makedirs(folder, exist_ok=True)
        n = 1 + max([int(m.group(1)) for p in glob.glob(os.path.join(folder, '%s_review_v*.png' % args.name))
                     for m in [re.search(r'_v(\d+)\.png$', p)] if m] or [0])
        out = os.path.join(folder, '%s_review_v%d.png' % (args.name, n))
    b.render().save(out)
    print(out)


if __name__ == '__main__':
    main()
