"""Makes the FR arm on the viewer's right a mirror of the arm on the viewer's left, so both arms and hands
have the same size. The lantern removal left the right arm redrawn by inpaint at a different length.

The copied block is the left sleeve from shoulder to cuff plus the hand; the rows below 58 stay as drawn
because the birdcage frame sits on that hip. Mirror axis: between columns 31 and 32 (x -> 63 - x).
Run: python mirror_arm.py <in.png> <out.png>
"""
import sys
from PIL import Image

BLOCKS = [((17, 43), (27, 56)), ((17, 57), (22, 58))]  # inclusive (x0, y0), (x1, y1) on the viewer's left

if __name__ == '__main__':
    im = Image.open(sys.argv[1]).convert('RGBA')
    src = im.copy()
    for (x0, y0), (x1, y1) in BLOCKS:
        for y in range(y0, y1 + 1):
            for x in range(x0, x1 + 1):
                im.putpixel((63 - x, y), src.getpixel((x, y)))
    im.save(sys.argv[2])
