# -*- coding: utf-8 -*-
from PIL import Image, ImageDraw, ImageFont
font = ImageFont.truetype(r"C:\Windows\Fonts\simsun.ttc", 16)
def gen(ch):
    img = Image.new("1", (16, 16), 0)
    dr = ImageDraw.Draw(img)
    b = dr.textbbox((0, 0), ch, font=font)
    w, h = b[2] - b[0], b[3] - b[1]
    dr.text(((16 - w) // 2 - b[0], (16 - h) // 2 - b[1]), ch, fill=1, font=font)
    up, lo = [], []
    for c in range(16):
        v = 0
        for r in range(8):
            if img.getpixel((c, r)): v |= (1 << r)
        up.append(v)
    for c in range(16):
        v = 0
        for r in range(8):
            if img.getpixel((c, r + 8)): v |= (1 << r)
        lo.append(v)
    return up, lo
for ch in "调速频率占空比":
    u, d = gen(ch)
    print('\t"%s",' % ch)
    print("\t" + ",".join("0x%02X" % x for x in u) + ",")
    print("\t" + ",".join("0x%02X" % x for x in d) + ',/*"%s"*/' % ch)
    print()