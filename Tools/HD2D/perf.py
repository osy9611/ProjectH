"""Prints median/p95 frame and GPU times from Unreal CSV profiles, skipping the load phase."""
import csv, statistics as st, sys

csv.field_size_limit(2**31 - 1)
SKIP_SECONDS = float(sys.argv[1])


def is_number(text):
    try:
        float(text)
        return True
    except ValueError:
        return False


for path in sys.argv[2:]:
    rows = list(csv.reader(open(path, encoding='utf-8', errors='ignore')))
    head = rows[0]
    frame = head.index('FrameTime')
    cols = [c for c in ('FrameTime', 'GPUTime', 'GameThreadTime', 'RenderThreadTime') if c in head]
    last = max(head.index(c) for c in cols)
    # The file ends with a metadata row and repeats the header, which fail these checks.
    data = [r for r in rows[1:] if len(r) > last and is_number(r[frame])]
    elapsed, kept = 0.0, []
    for r in data:
        elapsed += float(r[frame]) / 1000
        if elapsed > SKIP_SECONDS:
            kept.append(r)
    out = [path.rsplit('\\', 1)[-1], 'frames=%d' % len(data), 'n=%d' % len(kept), 'span=%.0fs' % (elapsed - SKIP_SECONDS)]
    for col in cols:
        v = sorted(float(r[head.index(col)]) for r in kept)
        out.append('%s med %.2f p95 %.2f max %.1f' % (col, st.median(v), v[int(len(v) * .95)], v[-1]))
    print(' | '.join(out))
