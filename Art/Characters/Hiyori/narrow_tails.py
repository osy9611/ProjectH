"""Pull the twin tails in against the body (brief.md "꼬리 폭"), the hiyori_master_v1 narrow_silhouette method.

The body core (columns CORE_X0..CORE_X1) stays as drawn. Outside it, every row from ROW0 to ROW1 is resampled
toward the core at RATIO (nearest dot, no new colours), so the tails get thinner and sit closer to the body.
The outermost dot of each side keeps the silhouette outline colour. Rows above ROW0 (head wings) are untouched.
Run: python narrow_tails.py <in.png> <out.png> [ratio]
"""
import sys
import numpy as np
from PIL import Image

CORE_X0, CORE_X1 = 23, 40
ROW0, ROW1 = 34, 65
EDGE = (0x3b, 0x23, 0x47)


def narrow(a, ratio):
    out = a.copy()
    w = a.shape[1]
    for y in range(ROW0, ROW1 + 1):
        row = a[y]
        new = row.copy()
        for x in range(0, CORE_X0):                      # left: distance from the core edge shrinks by ratio
            src = CORE_X0 - round((CORE_X0 - x) / ratio)
            new[x] = row[src] if src >= 0 else 0
        for x in range(CORE_X1 + 1, w):
            src = CORE_X1 + round((x - CORE_X1) / ratio)
            new[x] = row[src] if src < w else 0
        for side in (range(0, CORE_X0), range(w - 1, CORE_X1, -1)):
            for x in side:                                # first opaque dot from the outside = outline
                if new[x, 3]:
                    new[x] = (*EDGE, 255)
                    break
        out[y] = new
    return out


if __name__ == '__main__':
    ratio = float(sys.argv[3]) if len(sys.argv) > 3 else 0.5
    a = np.array(Image.open(sys.argv[1]).convert('RGBA'))
    Image.fromarray(narrow(a, ratio), 'RGBA').save(sys.argv[2])
