#!/usr/bin/env python3
"""从 Chromium 官方精灵图切割恐龙/仙人掌位图 → dino_sprites.h
用法: python extract_sprites.py 100-offline-sprite.png
来源: https://chromium.googlesource.com/chromium/src/+/main/components/neterror/resources/images/
坐标: 官方 offline_sprite_definitions.ts ldpi 段（BSD-3）"""
import zlib, struct, sys

# ldpi 切割坐标 (x, w, h)：tRex 起点 848（帧偏移 0/88/132/220）
SPRITES = [
    ('DinoWait',  848,      44, 47),
    ('DinoRun1',  848+88,   44, 47),
    ('DinoRun2',  848+132,  44, 47),
    ('DinoDead',  848+220,  44, 47),
    ('CactusS',   228,      17, 35),
    ('CactusL',   332,      25, 50),
]

def decode_png(path):
    png = open(path, 'rb').read()
    pos = 8; idat = b''; plte = b''; trns = None
    while pos < len(png):
        length, ctype = struct.unpack('>I4s', png[pos:pos+8])
        chunk = png[pos+8:pos+8+length]
        if ctype == b'IHDR': w, h, depth, color = struct.unpack('>IIBB', chunk[:10])
        elif ctype == b'PLTE': plte = chunk
        elif ctype == b'tRNS': trns = chunk
        elif ctype == b'IDAT': idat += chunk
        pos += 12 + length
    assert depth == 4 and color == 3, f'预期 4bpp 调色板图, 得到 depth={depth} color={color}'
    raw = zlib.decompress(idat)
    stride = (w * 4 + 7) // 8
    img = bytearray(h * stride); prev = bytearray(stride); p = 0
    for y in range(h):
        f = raw[p]; p += 1
        line = bytearray(raw[p:p+stride]); p += stride
        if f == 1:
            for i in range(1, stride): line[i] = (line[i] + line[i-1]) & 0xFF
        elif f == 2:
            for i in range(stride): line[i] = (line[i] + prev[i]) & 0xFF
        elif f == 3:
            for i in range(stride):
                a = line[i-1] if i >= 1 else 0
                line[i] = (line[i] + ((a + prev[i]) >> 1)) & 0xFF
        elif f == 4:
            for i in range(stride):
                a = line[i-1] if i >= 1 else 0
                b = prev[i]; c = prev[i-1] if i >= 1 else 0
                pa, pb, pc = abs(b-c), abs(a-c), abs(a+b-2*c)
                pr = a if pa <= pb and pa <= pc else b if pb <= pc else c
                line[i] = (line[i] + pr) & 0xFF
        img[y*stride:(y+1)*stride] = line
        prev = line
    return w, h, img, stride, plte, trns

def main():
    png_path = sys.argv[1] if len(sys.argv) > 1 else '100-offline-sprite.png'
    w, h, img, stride, plte, trns = decode_png(png_path)
    ncolors = len(plte)//3
    transparent = {trns[i] for i in range(min(len(trns), ncolors)) if trns[i] == 0} if trns else {0}
    def fg(x, y):
        byte = img[y*stride + x//2]
        idx = (byte >> 4) if x % 2 == 0 else (byte & 0x0F)
        if idx in transparent: return False
        return sum(plte[idx*3:idx*3+3]) / 3 < 160
    parts = []
    for name, sx, sw, sh in SPRITES:
        nbytes = (sw + 7) // 8
        out = [f'/* {name}: {sw}x{sh} */',
               f'static const uint8_t {name}[{sh}][{nbytes}] = {{']
        for y in range(sh):
            bits = 0
            for x in range(sw):
                bits = (bits << 1) | (1 if fg(sx + x, 2 + y) else 0)
            r = (f'0x{bits:02x}' if nbytes == 1 else
                 ', '.join(f'0x{(bits >> (8*(nbytes-1-k))) & 0xFF:02x}' for k in range(nbytes)))
            out.append(f'  {{{r}}},')
        out.append('};')
        parts.append('\r\n'.join(out))
    header = ('/* Chrome Dino 官方精灵位图（Chromium, BSD-3）MSB-first, 1=画点 */\r\n'
              '#pragma once\r\n\r\n#include "main.h"\r\n\r\n')
    open('dino_sprites.h', 'wb').write((header + '\r\n\r\n'.join(parts) + '\r\n').encode('utf-8'))
    print(f'OK: dino_sprites.h 生成（{len(SPRITES)} 个精灵, 源图 {w}x{h}）')

if __name__ == '__main__':
    main()
