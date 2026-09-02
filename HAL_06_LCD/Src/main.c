/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body — Claude 星芒动画演示
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License.  You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "dino_sprites.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* Chrome Dino 跑酷（官方坐标系：x=竖0..239, y=横0..319，白底黑精灵）
   恐龙脚踩地面 GX=210，跳跃抛物线，仙人掌右进左移，B1(PB0) 跳，碰撞 GameOver */
#define GROUND_X      210       /* 地面（竖向 x） */
#define DINO_X        30        /* 恐龙左上 x（固定） */
#define GAME_W        320       /* 横向宽度 */
#define GRAVITY       25        /* 每帧速度增量（0.1px/frame） */
#define JUMP_V       -200       /* 峰高 80px 腾空 16 帧：横向 96px 移动裕量足够 */
#define OB_BASE_MS    40        /* 逻辑帧周期 ms */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */
static void Claude_Pixel(int x, int y, u16 color);
static void ClearRect(int x0, int y0, int x1, int y1, u16 color);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* 画一个点：x=竖 0..239，y=横 0..319（官方 SetCursor 参数序），全逐点寻址 */
static void Claude_Pixel(int x, int y, u16 color)
{
  if (x < 0 || x > 239 || y < 0 || y > 319) return;
  LCD_SetCursor((u8)x, (u16)y);
  LCD_WriteRAM_Prepare();
  LCD_WriteRAM(color);
}

/* 矩形擦除：全逐点（唯一实测无残影方案） */
static void ClearRect(int x0, int y0, int x1, int y1, u16 color)
{
  int x, y;
  if (x0 < 0) x0 = 0;  if (x1 > 239) x1 = 239;
  if (y0 < 0) y0 = 0;  if (y1 > 319) y1 = 319;
  for (x = x0; x <= x1; x++)
    for (y = y0; y <= y1; y++)
      Claude_Pixel(x, y, color);
}

/* 位图精灵绘制：bit=1 画 color（全逐点寻址）。精灵行=竖向x、列=横向y */
static void DrawSprite(const uint8_t img[][6], int h, int w, int x, int y, u16 color)
{
  int r, c;
  for (r = 0; r < h; r++)
    for (c = 0; c < w; c++)
      if (img[r][c >> 3] & (0x80 >> (c & 7)))
        Claude_Pixel(x + r, y + c, color);
}

static void DrawSprite3(const uint8_t img[][3], int h, int w, int x, int y, u16 color)
{
  int r, c;
  for (r = 0; r < h; r++)
    for (c = 0; c < w; c++)
      if (img[r][c >> 3] & (0x80 >> (c & 7)))
        Claude_Pixel(x + r, y + c, color);
}

static void DrawSprite4(const uint8_t img[][4], int h, int w, int x, int y, u16 color)
{
  int r, c;
  for (r = 0; r < h; r++)
    for (c = 0; c < w; c++)
      if (img[r][c >> 3] & (0x80 >> (c & 7)))
        Claude_Pixel(x + r, y + c, color);
}

/* 擦精灵 = 白色重画 */
static void EraseSprite(const uint8_t img[][6], int h, int w, int x, int y)
{
  DrawSprite(img, h, w, x, y, White);
}

/* 碰撞：恐龙收缩盒 vs 仙人掌收缩盒，矩形相交
   dinoTop = 恐龙顶 x（= DINO_TOP + dino_y_off）；恐龙高 47 宽 44 */
static int Hit(int dinoTop, int cy, int cw, int ch)
{
  int db = dinoTop + 42;                                 /* 恐龙竖向范围（脚） */
  int df = 60 + 6, dr = 60 + 38;                          /* 恐龙横向（y=60 起） */
  int ot = GROUND_X - (ch * 3 / 4) + 3;                  /* 仙人掌竖向顶（裁1/4后） */
  int ol = cy + 2, orr = cy + cw - 2;                    /* 仙人掌横向 */
  /* 双轴：脚(db)低于仙人掌顶(ot)=高度不够撞上；横向区间重叠=位置撞上 */
  return (db >= ot) && (dr >= ol) && (df <= orr);
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  /* USER CODE BEGIN 2 */
	
	LCD_Init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	
	LCD_Clear(White);
	LCD_SetBackColor(White);
	LCD_SetTextColor(Black);

	/* 地平线：竖向 x=GROUND_X，横向全宽 */
	for (int yy = 0; yy < GAME_W; yy++) Claude_Pixel(GROUND_X, yy, Black);

	LCD_DisplayStringLine(Line9 ,(unsigned char *)"   DINO  RUN         ");

	#define DINO_TOP (GROUND_X - 47)   /* 恐龙顶 x（脚正好踩地面） */
	int dino_off = 0;                  /* 腾空偏移(负=向上) */
	int vel = 0;
	int in_air = 0;
	int cy = 500;                      /* 仙人掌 y（屏外等待） */
	int cw = 17, ch = 35;
	int big = 0;
	int runframe = 0;
	int score = 0;
	int dead = 0;
	int started = 0;
	const uint8_t (*body)[6] = DinoRun1;

	DrawSprite(DinoRun1, 47, 44, DINO_TOP, 60, Black);
	LCD_DisplayStringLine(Line4 ,(unsigned char *)"   PRESS B1 TO START  ");

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    /* 待机：等 B1 开始 */
    if (!started)
    {
      if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) == GPIO_PIN_RESET)
      {
        /* 清掉待机字幕（用背景色空格覆盖） */
        LCD_SetBackColor(White);
        LCD_DisplayStringLine(Line4 ,(unsigned char *)"                      ");
        started = 1;
      }
      else
      {
        HAL_Delay(20);
        continue;
      }
    }

    HAL_Delay(OB_BASE_MS);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    HAL_Delay(OB_BASE_MS);

    if (dead)
    {
      if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) == GPIO_PIN_RESET)
      {
        /* 重开：擦现场 */
        EraseSprite(body, 47, 44, DINO_TOP + dino_off, 60);
        ClearRect(GROUND_X - ch, cy, GROUND_X, cy + cw, White);
        for (int yy = 0; yy < GAME_W; yy++) Claude_Pixel(GROUND_X, yy, Black);
        /* 清掉 GAME OVER / RESTART 两行字幕 */
        LCD_SetBackColor(White);
        LCD_DisplayStringLine(Line4 ,(unsigned char *)"                      ");
        LCD_DisplayStringLine(Line6 ,(unsigned char *)"                      ");
        dino_off = 0; vel = 0; in_air = 0; cy = 500; score = 0; dead = 0;
        LCD_DisplayChar(Line0, 296, '0');
        LCD_DisplayChar(Line0, 272, '0');
        LCD_DisplayChar(Line0, 248, '0');
        LCD_DisplayChar(Line0, 224, '0');
        body = DinoRun1;
        DrawSprite(body, 47, 44, DINO_TOP, 60, Black);
      }
      continue;
    }

    /* B1(PB0) 且在地面 → 起跳 */
    if (!in_air && HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) == GPIO_PIN_RESET)
    {
      vel = JUMP_V; in_air = 1;
    }

    /* --- 先擦旧 --- */
    EraseSprite(body, 47, 44, DINO_TOP + dino_off, 60);
    if (cy < GAME_W && cy + cw > 0)
      ClearRect(GROUND_X - ch, cy, GROUND_X, cy + cw, White);

    /* --- 恐龙物理 --- */
    if (in_air)
    {
      vel += GRAVITY;
      dino_off += vel / 10;
      if (dino_off >= 0) { dino_off = 0; vel = 0; in_air = 0; }
      body = DinoWait;
    }
    else
    {
      body = (runframe++ & 1) ? DinoRun2 : DinoRun1;
    }

    /* --- 仙人掌状态机：屏内左移 → 出屏+10分 → 重生 --- */
    if (cy + cw > 0)
    {
      cy -= 6;
    }
    else
    {
      score += 10;                         /* 成功跳过一个 */
      /* 计分板：右上角（Line0 右端，用 DisplayChar 显示 4 位） */
      LCD_SetTextColor(Black);
      LCD_DisplayChar(Line0, 296, '0' + (score / 1000) % 10);
      LCD_DisplayChar(Line0, 272, '0' + (score / 100) % 10);
      LCD_DisplayChar(Line0, 248, '0' + (score / 10) % 10);
      LCD_DisplayChar(Line0, 224, '0' + score % 10);
      if ((score / 10 % 4) == 0)
      {
        big = ((score / 10 & 3) == 2);
        cw = big ? 25 : 17; ch = big ? 50 : 35;
        cy = GAME_W - 1;
      }
    }

    /* --- 画新（仙人掌顶部裁 1/4 高度） --- */
    DrawSprite(body, 47, 44, DINO_TOP + dino_off, 60, Black);
    if (cy < GAME_W && cy + cw > 0)
    {
      int ox = GROUND_X - ch;
      if (big) DrawSprite4(CactusL + 12, 37, 25, ox, cy, Black);   /* 50→37 行 */
      else     DrawSprite3(CactusS + 8, 26, 17, ox, cy, Black);    /* 35→26 行 */
    }

    /* --- 碰撞 --- */
    if (cy < GAME_W && cy + cw > 0 && Hit(DINO_TOP + dino_off, cy, cw, ch))
    {
      body = DinoDead;
      DrawSprite(DinoDead, 47, 44, DINO_TOP + dino_off, 60, Black);
      LCD_DisplayStringLine(Line4 ,(unsigned char *)"     GAME OVER       ");
      LCD_DisplayStringLine(Line6 ,(unsigned char *)"   B1 to RESTART     ");
      dead = 1;
    }
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage 
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV2;
  RCC_OscInitStruct.PLL.PLLN = 20;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15|GPIO_PIN_0 
                          |GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4 
                          |GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8 
                          |GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5|GPIO_PIN_8|GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pins : PC13 PC14 PC15 PC0 
                           PC1 PC2 PC3 PC4 
                           PC5 PC6 PC7 PC8 
                           PC9 PC10 PC11 PC12 */
  GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15|GPIO_PIN_0 
                          |GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4 
                          |GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8 
                          |GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA8 */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB5 PB8 PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB1 （按键 B1 B2 输入） */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
