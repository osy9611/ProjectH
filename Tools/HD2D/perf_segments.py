"""Prints per-segment GPU and frame medians for a light_probe.py run.

Usage: python perf_segments.py <driver.log> <profile.csv>
The driver logs "[Driver] seg <name> <first tick> <last tick>"; ticks are CSV rows because both start in the same frame.
Pass columns need r.GPUCsvStatsEnabled 1; only the passes a candidate can change are printed.
"""
import csv, re, statistics as st, sys

csv.field_size_limit(2**31 - 1)
PASSES = ('GPU/Basepass', 'GPU/Lights', 'GPU/ShadowProjection', 'GPU/ShadowDepths', 'GPU/VolumetricFog', 'GPU/Postprocessing')

segments = [(m.group(1), int(m.group(2)), int(m.group(3)))
            for m in re.finditer(r'\[Driver\] seg (\S+) (\d+) (\d+)', open(sys.argv[1], encoding='utf-8', errors='ignore').read())]
rows = list(csv.reader(open(sys.argv[2], encoding='utf-8', errors='ignore')))
head = rows[0]
cols = ['FrameTime', 'GPUTime', 'RenderThreadTime'] + [c for c in PASSES if c in head]


def value(r, c):
    try:
        return float(r[head.index(c)])
    except (ValueError, IndexError):
        return None


print('segment | n | ' + ' | '.join(cols))
for name, a, b in segments:
    out = [name, str(b - a)]
    for c in cols:
        v = [x for x in (value(r, c) for r in rows[a + 1:b + 1]) if x is not None]
        out.append('%.3f' % st.median(v) if v else '-')
    print(' | '.join(out))
