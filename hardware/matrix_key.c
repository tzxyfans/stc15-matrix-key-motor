/*
 * matrix_key.c —— 4×4 矩阵按键模块内部实现
 *
 * ============================================================================
 * 逐行扫描法详解（重点知识）
 * ============================================================================
 *   矩阵按键的核心思想：用"分行拉低 + 读列状态"定位按键。
 *
 *   第一步：所有行线输出高电平（默认状态，不扫描任何行）
 *   第二步：把第 1 行拉低（ROW1=0），其他行保持高 → 读 4 条列线
 *     - 如果 COL1=0 → 第1行第1列的键 '7' 被按下
 *     - 如果 COL2=0 → 第1行第2列的键 '8' 被按下
 *     - ...以此类推
 *     - 如果 4 条列线全为高 → 第1行无按键，继续扫描下一行
 *   第三步：把第 2 行拉低（ROW2=0），其他行保持高 → 读列线
 *   第四步：第 3 行、第 4 行同理
 *
 *   消抖：按键按下/放开瞬间，金属触点弹跳会产生 5~10ms 抖动。
 *   检测到按键后延时 10ms 再确认，如果仍为低电平才算有效按下。
 *
 *   等待放开：按键不放开会持续返回同一个值，通常要求"按一次只触发一次"，
 *   所以检测到按下后，循环等待列线恢复高电平（放开）才返回。
 * ============================================================================
 */
#include "matrix_key.h"
#include "delay.h"

/* ---- 行线定义（输出扫描信号）---- */
sbit ROW1 = P3^0;            /* 第1行 */
sbit ROW2 = P3^1;            /* 第2行 */
sbit ROW3 = P3^2;            /* 第3行 */
sbit ROW4 = P3^3;            /* 第4行 */

/* ---- 列线定义（输入读取状态）---- */
sbit COL1 = P4^4;            /* 第1列 */
sbit COL2 = P4^2;            /* 第2列 */
sbit COL3 = P3^5;            /* 第3列 */
sbit COL4 = P3^4;            /* 第4列 */

/*
 * 按键值映射表（存放在 Flash 程序存储器，节省 RAM）
 *   key_values[行][列] = 按键字符
 *   code 关键字：把数组放在 Flash 而非 RAM，适合不变的映射表
 */
code unsigned char key_values[4][4] = {
    {'7', '8', '9', 'A'},    /* ROW1: 7 8 9 A */
    {'4', '5', '6', 'B'},    /* ROW2: 4 5 6 B */
    {'1', '2', '3', 'C'},    /* ROW3: 1 2 3 C */
    {'*', '0', '#', 'D'}     /* ROW4: * 0 # D */
};

/*
 * MatrixKey_Init —— 初始化矩阵按键端口
 *   行线/列线都配为准双向口（M1=0, M0=0），这是 STC15 上电默认模式
 *   准双向口做输出时能直接写 0/1，做输入时先写 1 再读引脚电平
 */
void MatrixKey_Init(void)
{
    /* P3.0~P3.3 (ROW1~ROW4) 准双向口 */
    P3M1 &= ~0x0F;  P3M0 &= ~0x0F;
    /* P3.4~P3.5 (COL4~COL3) 准双向口 */
    P3M1 &= ~0x30;  P3M0 &= ~0x30;
    /* P4.2 (COL2)、P4.4 (COL1) 准双向口 */
    P4M1 &= ~0x14;  P4M0 &= ~0x14;

    /* 行线默认输出高电平（不扫描任何行）*/
    ROW1 = 1;  ROW2 = 1;  ROW3 = 1;  ROW4 = 1;
    /* 列线写1（准双向口做输入前必须先写1）*/
    COL1 = 1;  COL2 = 1;  COL3 = 1;  COL4 = 1;
}

/*
 * ReadCol —— 读取列线状态，返回按下的列号（0~3），无按键返回 4
 *   按键按下时，对应列线被行线的低电平拉低
 */
static unsigned char ReadCol(void)
{
    if (COL1 == 0) return 0;
    if (COL2 == 0) return 1;
    if (COL3 == 0) return 2;
    if (COL4 == 0) return 3;
    return 4;                   /* 4 = 无按键 */
}

/*
 * SetRow —— 设置当前扫描行（拉低），其他行拉高
 *   row: 0~3 表示第1~4行
 */
static void SetRow(unsigned char row)
{
    ROW1 = 1;  ROW2 = 1;  ROW3 = 1;  ROW4 = 1;   /* 先全部拉高 */
    switch (row)
    {
        case 0: ROW1 = 0; break;                  /* 第1行拉低 */
        case 1: ROW2 = 0; break;                  /* 第2行拉低 */
        case 2: ROW3 = 0; break;                  /* 第3行拉低 */
        case 3: ROW4 = 0; break;                  /* 第4行拉低 */
    }
}

/*
 * MatrixKey_Read —— 非阻塞读取当前按键状态（无消抖）
 *   逐行扫描一次，返回当前按下的键值；无键返回 KEY_NONE
 *   不消抖、不等待放开，每次调用立即返回
 *   用途：点动控制（按住动、松开停），配合主循环10ms节拍隐式消抖
 */
unsigned char MatrixKey_Read(void)
{
    unsigned char row, col;

    for (row = 0; row < 4; row++)
    {
        SetRow(row);             /* 当前行拉低，其他行拉高 */
        col = ReadCol();         /* 读列线状态 */
        if (col < 4)             /* 某列被按下 */
        {
            ROW1 = 1;  ROW2 = 1;  ROW3 = 1;  ROW4 = 1;  /* 恢复行线 */
            return key_values[row][col];
        }
    }

    ROW1 = 1;  ROW2 = 1;  ROW3 = 1;  ROW4 = 1;  /* 无键，恢复行线 */
    return KEY_NONE;
}

/*
 * MatrixKey_Scan —— 非阻塞按键扫描（边沿检测 + 消抖）
 *   返回值：检测到"刚按下"时返回键值，其他情况返回 KEY_NONE
 *
 *   原理（key_now / key_last 边沿检测）：
 *     key_now  = 本次扫描的按键状态
 *     key_last = 上次扫描的按键状态（static 保存，跨调用不丢失）
 *     当 key_last==KEY_NONE 且 key_now!=KEY_NONE → 下降沿（刚按下）→ 返回键值
 *     按住不放时 key_now==key_last，不会重复返回 → 按一次只触发一次
 *
 *   ✅ 非阻塞：不等待放开，主循环不会被卡死
 *   ✅ 按一次触发一次：边沿检测，持续按下不重复返回
 *   ✅ 消抖：下降沿时延时10ms再次确认
 */
unsigned char MatrixKey_Scan(void)
{
    static unsigned char key_last = KEY_NONE;   /* 上次扫描结果（跨调用保持）*/
    unsigned char key_now;

    key_now = MatrixKey_Read();                 /* 1. 读取当前按键状态 */

    /* 2. 下降沿检测：上次无键 + 本次有键 = 刚按下 */
    if (key_now != KEY_NONE && key_last == KEY_NONE)
    {
        DelayMs(10);                            /* 3. 消抖：延时10ms */
        key_now = MatrixKey_Read();             /* 4. 再次读取确认 */
        if (key_now != KEY_NONE)                /* 5. 确认有效按下 */
        {
            key_last = key_now;
            return key_now;                     /* 返回键值 */
        }
        /* 抖动，不算有效，继续往下更新 key_last */
    }

    key_last = key_now;                         /* 6. 更新上次状态 */
    return KEY_NONE;                            /* 无事件 */
}
