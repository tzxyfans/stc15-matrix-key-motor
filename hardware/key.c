/*
 * key.c —— 按键模块内部实现（项目二：新增"放开事件"识别，扩展至 S4~S7 四键）
 */
#include "key.h"

sbit S7 = P3^0;                /* 更换开发板只需修改这里 */
sbit S6 = P3^1;
sbit S5 = P3^2;
sbit S4 = P3^3;

static bit s4_last = 1;        /* S4 上一次按键状态（1 = 未按下），程序运行期间不丢失 */
static bit s5_last = 1;        /* S5 上一次按键状态 */
static bit s6_last = 1;        /* S6 上一次按键状态 */
static bit s7_last = 1;        /* S7 上一次按键状态 */

void Key_Init(void)
{
    P3M1 &= ~0x0F;             /* P3.0~P3.3 准双向口：M1=0，M0=0（S7/S6/S5/S4 四个按键输入） */
    P3M0 &= ~0x0F;
    S7 = 1;                    /* 写 1，准备读引脚 */
    S6 = 1;
    S5 = 1;
    S4 = 1;
    s7_last = 1;               /* 历史状态初始化为"未按下"（与上拉电平一致） */
    s6_last = 1;
    s5_last = 1;
    s4_last = 1;
}

unsigned char Key_S4_Down(void)
{
    if (S4 == 0)               /* 独立按键低电平有效 */
    {
        return (1);            /* 按下返回 1 */
    }
    else
    {
        return (0);            /* 放开返回 0 */
    }
}

unsigned char Key_S5_Down(void)
{
    if (S5 == 0)               /* 独立按键低电平有效 */
    {
        return (1);
    }
    else
    {
        return (0);
    }
}

unsigned char Key_S6_Down(void)
{
    if (S6 == 0)               /* 独立按键低电平有效 */
    {
        return (1);
    }
    else
    {
        return (0);
    }
}

unsigned char Key_S7_Down(void)
{
    if (S7 == 0)               /* 独立按键低电平有效 */
    {
        return (1);
    }
    else
    {
        return (0);
    }
}


/*
 * 放开事件识别（S4~S7 四键写法完全相同）：
 * 只有"上次按下 + 本次放开"的那一次返回 1，
 * 其余时刻（一直按住 / 一直放开 / 按下的那一帧）都返回 0。
 * 注意：else 分支里也必须保存本次状态，否则"按下"永远记不住！
 *
 * 本阶段主循环只用 Key_Sx_Down 轮询电平，Released 系列暂无调用者，
 * 用 #if 0 屏蔽以消除 Keil WARNING L16（UNCALLED SEGMENT）；
 * 后续阶段做"事件式检测"时取消 #if 0 即可（与 OLED 模块同一惯例）。
 */

unsigned char Key_S4_Released(void)
{
    bit now = S4;                      /* 1. 读按键当前状态（1 = 放开，0 = 按下） */
    if (s4_last == 0 && now == 1)      /* 2. 上次按下 + 本次放开 = 放开事件 */
    {
        s4_last = now;                 /* 3. 保存本次，供下次比较 */
        return (1);                    /* 4. 事件只在本帧返回一次 */
    }
    else
    {
        s4_last = now;                 /* 3. 没有事件也要保存本次状态！ */
        return (0);                    /* 4. 无事件返回 0 */
    }
}

unsigned char Key_S5_Released(void)
{
    bit now = S5;
    if (s5_last == 0 && now == 1)
    {
        s5_last = now;
        return (1);
    }
    else
    {
        s5_last = now;
        return (0);
    }
}

unsigned char Key_S6_Released(void)
{
    bit now = S6;
    if (s6_last == 0 && now == 1)
    {
        s6_last = now;
        return (1);
    }
    else
    {
        s6_last = now;
        return (0);
    }
}

unsigned char Key_S7_Released(void)
{
    bit now = S7;
    if (s7_last == 0 && now == 1)
    {
        s7_last = now;
        return (1);
    }
    else
    {
        s7_last = now;
        return (0);
    }
}

