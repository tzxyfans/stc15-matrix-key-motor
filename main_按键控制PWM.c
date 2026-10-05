/*
 * PWM 调速控制 —— STC15 PCA 模块 8 位 PWM + 按键控制
 * 目标芯片：STC15F2K60S2 / IAP15F2K61S2
 * 晶振：11.0592MHz（内部 IRC）
 *
 * ============================================================================
 * 一、功能说明
 * ============================================================================
 *   上电默认停止状态，OLED 显示频率、占空比、运行状态。
 *   按键控制：
 *     S7 → 占空比 +1（范围 1~100）    S6 → 占空比 -1（范围 1~100）
 *     S4 → 正转/停止 复用              S5 → 反转/停止 复用
 *   占空比在停止时也可预调，启动后立即按该占空比运行。
 *
 * ============================================================================
 * 二、按键接线（key 模块统一定义）
 * ============================================================================
 *   S4 = P3.3（正转/停止复用）  S5 = P3.2（反转/停止复用）
 *   S6 = P3.1（占空比 -1）      S7 = P3.0（占空比 +1）
 *   独立按键，10k 上拉，按下为低电平，用 Key_Sx_Released() 放开事件检测。
 *
 * ============================================================================
 * 三、状态机设计
 * ============================================================================
 *   运行状态三态：停止(STOP) / 正转(FWD) / 反转(REV)
 *
 *   S4 按下（正转/停止复用）：
 *     当前正转 → 停止      当前停止 → 正转      当前反转 → 忽略（须先停止）
 *   S5 按下（反转/停止复用）：
 *     当前反转 → 停止      当前停止 → 反转      当前正转 → 忽略（须先停止）
 *   互锁保护：正转不能直接切反转，反转不能直接切正转，必须先回到停止。
 *
 *   占空比 S6/S7 在任何状态下都可调节：
 *     停止时只改缓存，启动时生效；运行中改则立即生效（motor 模块已处理）。
 *
 * ============================================================================
 * 四、PWM 硬件方案：PCA 模块1 的 8 位 PWM（配置在 motor.c 里）
 * ============================================================================
 *   CMOD=0x02(CPS=001→SYSclk/2) → 8位PWM频率 = 11059200/2/256 = 21.6KHz
 *   占空比 = CCAP1L/256，duty=50% → CCAP1L=128
 *   引脚：PWM=P1.0(CCP1)  DIR=P0.4  BRAKE=P4.7
 *
 * ============================================================================
 * 五、OLED 显示布局（128×64，4 行每行 16 像素高）
 * ============================================================================
 *   第一行 Y=0  ：PWM调速        （标题，居中，静态）
 *   第二行 Y=16 ：频率:21.6KHz   （静态）
 *   第三行 Y=32 ：占空比:050%    （数字动态刷新）
 *   第四行 Y=48 ：状态:停止      （文字动态刷新：停止/正转/反转）
 *
 *   字宽：英文/数字 8px，中文 16px。只在占空比或状态变化时局部刷新，不闪烁。
 */
#include "STC15.H"
#include "motor.h"
#include "key.h"
#include "delay.h"
#include "OLED_STC15.h"

/* 运行状态定义 */
#define STATE_STOP   0          /* 停止 */
#define STATE_FWD    1          /* 正转 */
#define STATE_REV    2          /* 反转 */

/*
 * 显示状态文字（第四行 X=40 处写 2 个中文）
 *   先清旧文字区域再写新的，避免残留
 */
static void ShowStateText(unsigned char state)
{
    OLED_ClearArea(40, 48, 32, 16);          /* 清 X=40~72, Y=48~64 的旧文字 */
    switch (state)
    {
        case STATE_STOP: OLED_ShowChinese(40, 48, "停止"); break;
        case STATE_FWD:  OLED_ShowChinese(40, 48, "正转"); break;
        case STATE_REV:  OLED_ShowChinese(40, 48, "反转"); break;
    }
}

/*
 * 主函数
 *   初始化 → 画静态界面 → 画初始占空比/状态 → 主循环扫描按键
 */
void main(void)
{
    unsigned char state = STATE_STOP;        /* 当前运行状态，上电默认停止 */
    unsigned char duty  = 50;                /* 当前占空比，上电默认 50% */
    bit need_update = 0;                     /* 本轮是否需要刷屏 */

    /* ---- 初始化 ---- */
    Motor_Init();                            /* PCA PWM + DIR/BRAKE，默认惯性停转 */
    Key_Init();                              /* S4~S7 准双向口 + 上拉 */
    OLED_Init();
    OLED_Clear();

    /* ---- 第一行：PWM调速（居中）---- */
    OLED_ShowString(36, 0, "PWM");           /* PWM = 24px，X=36~60 */
    OLED_ShowChinese(60, 0, "调速");         /* 调速 = 32px，X=60~92 */

    /* ---- 第二行：频率:21.6KHz ---- */
    OLED_ShowChinese(0, 16, "频率");         /* X=0~32 */
    OLED_ShowString(32, 16, ":21.6KHz");     /* X=32~96 */

    /* ---- 第三行：占空比:050% ---- */
    OLED_ShowChinese(0, 32, "占空比");       /* 标签 X=0~48 */
    OLED_ShowString(48, 32, ":");            /* 冒号 X=48~56 */
    OLED_ShowString(80, 32, "%");            /* 百分号 X=80~88 */
    OLED_ShowNumNoZero(56, 32, duty, 3);           /* 3位数字 X=56~80，初始 050 */

    /* ---- 第四行：状态:停止 ---- */
    OLED_ShowChinese(0, 48, "状态");         /* 标签 X=0~32 */
    OLED_ShowString(32, 48, ":");            /* 冒号 X=32~40 */
    ShowStateText(state);                    /* 初始"停止" X=40~72 */

    OLED_Update();                           /* 首次全屏刷新 */

    /* ---- 主循环：扫描按键，局部刷新 ---- */
    while (1)
    {
        /*
         * S7：占空比 +1（上限 100）
         * 用 Released 放开事件，按一次只触发一次，避免长按连续跳
         */
        if (Key_S7_Released())
        {
            if (duty < 100)
            {
                duty++;
                Motor_SetSpeed(duty);        /* 运行中立即生效，停止时只改缓存 */
                OLED_ShowNumNoZero(56, 32, duty, 3); /* 重画数字 */
                need_update = 1;
            }
        }

        /*
         * S6：占空比 -1（下限 1）
         */
        if (Key_S6_Released())
        {
            if (duty > 1)
            {
                duty--;
                Motor_SetSpeed(duty);
                OLED_ShowNumNoZero(56, 32, duty, 3);
                need_update = 1;
            }
        }

        /*
         * S4：正转/停止 复用（互锁：反转时按 S4 忽略，须先停止）
         */
        if (Key_S4_Released())
        {
            if (state == STATE_FWD)
            {
                Motor_Coast();                /* 正转 → 停止 */
                state = STATE_STOP;
                ShowStateText(state);
                need_update = 1;
            }
            else if (state == STATE_STOP)
            {
                Motor_Forward();              /* 停止 → 正转 */
                state = STATE_FWD;
                ShowStateText(state);
                need_update = 1;
            }
            /* state == STATE_REV 时忽略：不能从反转直接切正转 */
        }

        /*
         * S5：反转/停止 复用（互锁：正转时按 S5 忽略，须先停止）
         */
        if (Key_S5_Released())
        {
            if (state == STATE_REV)
            {
                Motor_Coast();                /* 反转 → 停止 */
                state = STATE_STOP;
                ShowStateText(state);
                need_update = 1;
            }
            else if (state == STATE_STOP)
            {
                Motor_Reverse();              /* 停止 → 反转 */
                state = STATE_REV;
                ShowStateText(state);
                need_update = 1;
            }
            /* state == STATE_FWD 时忽略：不能从正转直接切反转 */
        }

        /* 有变化才刷屏，避免无谓刷新导致闪烁 */
        if (need_update)
        {
            OLED_Update();
            need_update = 0;
        }

        DelayMs(10);                         /* 主循环 10ms 节拍，兼做按键消抖 */
    }
}
