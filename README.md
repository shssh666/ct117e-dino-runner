# 🦖 ct117e-dino-runner

Chrome 断网小恐龙跑酷，跑在蓝桥杯竞赛板 **CT117E-M4**（STM32G431RBT6）上。
白底黑像素风，板载按键 B1 跳跃，避开仙人掌，右上角计分（每跳过一个 +10）。

![board](https://img.shields.io/badge/board-CT117E--M4-blue) ![mcu](https://img.shields.io/badge/MCU-STM32G431RBT6-green) ![game](https://img.shields.io/badge/game-T--Rex%20Runner-black)

## 硬件

| 项 | 型号 |
|---|---|
| 竞赛板 | 蓝桥杯 CT117E-M4（4T Tech） |
| MCU | STM32G431RBT6 (Cortex-M4 @80MHz) |
| LCD | 240×320 (横向 320×纵向 240)，FSMC 总线，UC8230S/ILI932x 控制器 |
| 按键 | B1 (PB0) 跳跃 / 开始 / 重开 |
| 烧录 | 板载 DAP-Link（USB） |

## 快速开始

1. **装 Keil MDK**（5.x）+ **STM32G4xx DFP 1.2.0** 器件包
   （命令行静默装法：`D:\Keil5\UV4\PackUnzip.exe Keil.STM32G4xx_DFP.1.2.0.pack`）
2. 双击 `HAL_06_LCD/MDK-ARM/HAL_06_LCD.uvprojx` 打开工程
3. 编译（F7）→ 下载（F8）→ 按 B1 开始玩
   或命令行：
   ```
   D:\Keil5\UV4\UV4.exe -j0 -b HAL_06_LCD.uvprojx -o build_log.txt   # 编译
   D:\Keil5\UV4\UV4.exe -j0 -f HAL_06_LCD.uvprojx -o flash_log.txt   # 烧录
   ```

## 玩法

- 上电 → 恐龙站在地面 → **按 B1 开始**
- **按 B1 跳**（跳高 80px，腾空约 16 帧，时机对了能干净飞过仙人掌）
- 每跳过一个 +10 分（右上角）
- 撞上 → GAME OVER → 按 B1 重开
- `main.c` 顶部可调：`JUMP_V`（跳高）`GRAVITY`（下落）`OB_BASE_MS`（帧周期）

## 全流程踩坑实录（给后来者）

在 CT117E-M4 上从零跑通 LCD 动画的四个大坑，全都有官方源码背书的解法：

### 坑 1：坐标系是"竖横"不是"横竖"
官方 `LCD_DrawRect(70,210,100,100)`：`Height` 沿 **X（u8，0..239）**、`Width` 沿 **Y（u16，0..319）**。
即 **x = 物理竖边 240、y = 物理横边 320**。按 240×320 竖屏直觉写必翻车。

### 坑 2：写屏一律逐像素寻址，别用连写
板上 LCD 控制器可能是 UC8230S **或** ILI932x（`LCD_Init` 读 ID 自动分支），两家的 GRAM 连写自增方向不同。
用 `SetCursor + WriteRAM` 逐点写（官方 `LCD_DrawLine` 竖线分支就是这么干的）——慢一点，但两颗控制器下表现完全一致。任何依赖连写方向的批量擦除，换板必出残影。

### 坑 3：Bresenham 判定必须用 |dx|/|dy|
画"向上斜线"（dy<0）时若 err 判定用了带符号 dy，画笔冲过终点永不停止 → 程序死循环、画面定格。
而且 LCD 的 GRAM 断电才清、单片机复位不清——死循环后你看到的是上一次程序的残影，极具迷惑性！

### 坑 4：烧录报 "Target DLL has been cancelled" = USB 掉线
板载 DAP-Link 疑似被 Windows USB 节电挂起。插拔 USB 线重试即可。烧录前先确认 COM 口在线。

### 素材提取流程（可复现）
恐龙/仙人掌位图来自 Chromium 官方（`components/neterror/resources/images/100-offline-sprite.png`，4-bit 调色板 PNG）。
用 Python（zlib + PNG filter 还原 + 官方 `offline_sprite_definitions.ts` 的 ldpi 坐标切割）转成 C 位图数组 → `Inc/dino_sprites.h`。
索引 0 是透明色（tRNS 块），别把它当地图前景。

## 目录结构

```
HAL_06_LCD/
├── Inc/
│   ├── dino_sprites.h    # 官方恐龙/仙人掌位图（脚本生成）
│   ├── lcd.h             # 官方 LCD 驱动（未改动）
│   └── main.h
├── Src/
│   ├── main.c            # 游戏逻辑（碰撞/物理/计分/状态机）
│   └── lcd.c             # 官方 LCD 驱动（未改动）
├── MDK-ARM/
│   └── HAL_06_LCD.uvprojx  # Keil 工程
└── Drivers/              # STM32G4 HAL + CMSIS
```

## 许可

- 本项目代码：**MIT**
- 恐龙游戏素材：来自 [Chromium](https://chromium.googlesource.com/chromium/src/+/main/components/neterror/resources/)，BSD-3-Clause，© The Chromium Authors
- LCD 驱动（`lcd.c/lcd.h/fonts.h`）：来自蓝桥杯官方资源包（CT117E-M4 配套）
