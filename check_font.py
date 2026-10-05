data = open('hardware/OLED_Data.c', 'rb').read()
need = '电机控制调速占空比状态停止正转反转频率模式自动点动'
missing = []
for c in need:
    b = c.encode('utf-8')
    if b not in data:
        missing.append(c)
print('missing=' + ''.join(missing) if missing else 'missing=none')