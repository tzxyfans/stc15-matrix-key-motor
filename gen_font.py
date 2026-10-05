# -*- coding: utf-8 -*-
# 生成16x16汉字字模，列行式取模，低位在上(bit0=最上行)
# 用"正"字与字库对比验证取模方式
from PIL import Image, ImageDraw, ImageFont

FONT_PATH = r"C:\Windows\Fonts\simsun.ttc"
font = ImageFont.truetype(FONT_PATH, 16)

def gen_zimo(ch):
    img = Image.new("1", (16, 16), 0)
    dr = ImageDraw.Draw(img)
    bbox = dr.textbbox((0, 0), ch, font=font)
    w, h = bbox[2] - bbox[0], bbox[3] - bbox[1]
    x = (16 - w) // 2 - bbox[0]
    y = (16 - h) // 2 - bbox[1]
    dr.text((x, y), ch, fill=1, font=font)
    # 列行式：先列后行，每列8像素，上半行0-7，下半行8-15
    # 低位在上：bit0=最上行
    upper, lower = [], []
    for col in range(16):
        b = 0
        for row in range(8):
            if img.getpixel((col, row)):
                b |= (1 << row)
        upper.append(b)
    for col in range(16):
        b = 0
        for row in range(8):
            if img.getpixel((col, row + 8)):
                b |= (1 << row)
        lower.append(b)
    return upper, lower

def fmt(upper, lower):
    s1 = ",".join("0x%02X" % b for b in upper)
    s2 = ",".join("0x%02X" % b for b in lower)
    return s1 + ",\n\t" + s2 + ","

# 验证"正"字
zheng_lib = (
    "0x00,0x02,0x02,0xC2,0x02,0x02,0x02,0xFE,0x82,0x82,0x82,0x82,0x82,0x02,0x00,0x00",
    "0x40,0x40,0x40,0x7F,0x40,0x40,0x40,0x7F,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x00",
)
u, d = gen_zimo("正")
mine_u = ",".join("0x%02X" % b for b in u)
mine_d = ",".join("0x%02X" % b for b in d)
print("=== 验证正字 ===")
print("字库上:", zheng_lib[0])
print("我的上:", mine_u)
print("字库下:", zheng_lib[1])
print("我的下:", mine_d)
print("匹配:", mine_u == zheng_lib[0] and mine_d == zheng_lib[1])

print("\n=== 生成新字模 ===")
for ch in "限常左侧右":
    u, d = gen_zimo(ch)
    print('\t"%s",' % ch)
    print("\t" + fmt(u, d))
    print()