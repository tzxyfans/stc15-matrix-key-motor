/*
 * 矩阵按键控制电机 —— 4×4 矩阵按键 + PCA PWM 调速 + OLED 中文显示
 * 目标芯片：STC15F2K60S2 / IAP15F2K61S2
 * 晶振：11.0592MHz（内部 IRC）
 *
 * ============================================================================
 * ⚠️ 开发板跳线设置
 * ============================================================================
 *   跳线 J5 必须短接到【矩阵按键侧】的两个跳线端子！
 *   接在独立按键侧则矩阵按键行线/列线不连通，扫描读不到按键。
 *
 * ============================================================================
 * 一、功能说明（复刻第二讲独立按键功能，改用矩阵按键）
 * ============================================================================
 *   两种工作模式：自动模式 / 点动模式
 *   模式切换（A键）：只能在电机停止状态下切换，运行中按A无效（安全保护）
 *
 *   自动模式（边沿检测，按一次触发一次）：
 *     B键 = 正转/停止 复用（停止→正转，正转→停止，反转时忽略）
 *     C键 = 反转/停止 复用（停止→反转，反转→停止，正转时忽略）
 *
 *   点动模式（电平检测，按住动、松开停）：
 *     按住B = 正转，松开 = 停止
 *     按住C = 反转，松开 = 停止
 *
 *   占空比调节（两种模式下均可）：
 *     *键 = 占空比 +1（上限100）    #键 = 占空比 -1（下限1）
 *     数字键0~9 = 输入占空比两位数（保留最近两位，第三行最右侧预览）
 *     D键 = 参数确定（输入非0则更新占空比，输入为0则不变，确认后消隐预览）
 *
 *   互锁保护：正转不能直接切反转，必须先停止
 *
 * ============================================================================
 * 二、矩阵按键功能分配
 * ============================================================================
 *   ┌─────┬─────┬─────┬─────┐
 *   │  7  │  8  │  9  │  A  │  A=模式切换
 *   ├─────┼─────┼─────┼─────┤
 *   │  4  │  5  │  6  │  B  │  B=正转/停止
 *   ├─────┼─────┼─────┼─────┤
 *   │  1  │  2  │  3  │  C  │  C=反转/停止
 *   ├─────┼─────┼─────┼─────┤
 *   │  *  │  0  │  #  │  D  │  *=占空比+1 #=占空比-1 D=确认占空比
 *   └─────┴─────┴─────┴─────┘
 *     0~9键 = 输入占空比两位数（按D确认后生效）
 *
 * ============================================================================
 * 三、OLED 显示布局（128×64，4行每行16像素高）
 * ============================================================================
 *   第一行 Y=0  ：电机控制        （标题，居中）
 *   第二行 Y=16 ：模式:自动       （模式，动态刷新）
 *   第三行 Y=32 ：占空比:050%        输入预览(最右侧2位)
 *   第四行 Y=48 ：状态:停止       （状态，动态刷新）
 *
 *   字宽：英文/数字 8px，中文 16px
 *   只在数据变化时局部刷新对应区域，避免全屏闪烁
 */
#include "STC15.H"
#include "motor.h"
#include "matrix_key.h"
#include "delay.h"
#include "OLED_STC15.h"

/* ---- 工作模式定义 ---- */
#define MODE_AUTO   0    /* 自动模式：按一次触发一次 */
#define MODE_JOG    1    /* 点动模式：按住动、松开停 */

/* ---- 电机运行状态定义 ---- */
#define STATE_STOP  0    /* 停止 */
#define STATE_FWD   1    /* 正转 */
#define STATE_REV   2    /* 反转 */

/* ---- 显示坐标常量 ---- */
#define MODE_TEXT_X  40    /* 模式文字起始X（"模式:"之后）*/
#define DUTY_NUM_X   56    /* 占空比数字起始X（"占空比:"之后）*/
#define STATE_TEXT_X 40    /* 状态文字起始X（"状态:"之后）*/
#define INPUT_X     112    /* 输入预览起始X（第三行最右侧，2位数字16px）*/

/*
 * ShowModeText —— 显示模式文字（第二行 X=40 处写2个中文）
 *   先清旧文字区域再写新的，避免残留
 */
static void ShowModeText(unsigned char mode)
{
    switch (mode)
    {
        case MODE_AUTO: OLED_ShowChinese(MODE_TEXT_X, 16, "自动"); break;
        case MODE_JOG:  OLED_ShowChinese(MODE_TEXT_X, 16, "点动"); break;
    }
}

/*
 * ShowStateText —— 显示状态文字（第四行 X=40 处写2个中文）
 */
static void ShowStateText(unsigned char state)
{
    switch (state)
    {
        case STATE_STOP: OLED_ShowChinese(STATE_TEXT_X, 48, "停止"); break;
        case STATE_FWD:  OLED_ShowChinese(STATE_TEXT_X, 48, "正转"); break;
        case STATE_REV:  OLED_ShowChinese(STATE_TEXT_X, 48, "反转"); break;
    }
}

/*
 * ShowDutyNum —— 显示占空比数字（第三行，3位高位0消隐）
 */
static void ShowDutyNum(unsigned char duty)
{
    OLED_ShowNumNoZero(DUTY_NUM_X, 32, duty, 3);
}

/*
 * ShowInputNum —— 显示输入预览数字（第三行最右侧，2位高位0消隐）
 *   数字键输入时实时显示最近按下的两位数
 */
static void ShowInputNum(unsigned char num)
{
    OLED_ShowNum(INPUT_X, 32, num, 2);
}

/*
 * ClearInputNum —— 消隐输入预览区域（确认或取消后调用）
 */
static void ClearInputNum(void)
{
    OLED_ClearArea(INPUT_X, 32, 16, 16);     /* 2位数字 = 16px宽 */
}

/*
 * 主函数
 *   初始化 → 画静态界面 → 画初始数据 → 主循环扫描按键控制电机
 */
void main(void)
{
    unsigned char mode  = MODE_AUTO;     /* 当前工作模式，上电默认自动 */
    unsigned char state = STATE_STOP;    /* 电机运行状态，上电默认停止 */
    unsigned char duty  = 50;            /* 占空比，上电默认50% */
    unsigned char input_buf = 0;         /* 数字键输入缓冲（最近两位）*/
    bit need_update = 0;                 /* 本轮是否需要刷屏 */

    /* ---- 初始化 ---- */
    Motor_Init();                        /* PCA PWM + DIR/BRAKE，默认惯性停转 */
    MatrixKey_Init();                    /* 矩阵按键端口初始化 */
    OLED_Init();
    OLED_Clear();

    /* ---- 第一行：电机控制（居中）---- */
    /* "电机控制" = 4中文 = 64px，居中 X=(128-64)/2=32 */
    OLED_ShowChinese(32, 0, "电机控制");

    /* ---- 第二行：模式:自动 ---- */
    OLED_ShowChinese(0, 16, "模式");     /* 标签 X=0~32 */
    OLED_ShowString(32, 16, ":");        /* 冒号 X=32~40 */
    ShowModeText(mode);                  /* 文字 X=40~72 */

    /* ---- 第三行：占空比:050% ---- */
    OLED_ShowChinese(0, 32, "占空比");   /* 标签 X=0~48 */
    OLED_ShowString(48, 32, ":");        /* 冒号 X=48~56 */
    OLED_ShowString(80, 32, "%");        /* 百分号 X=80~88 */
    ShowDutyNum(duty);                   /* 数字 X=56~80 */

    /* ---- 第四行：状态:停止 ---- */
    OLED_ShowChinese(0, 48, "状态");     /* 标签 X=0~32 */
    OLED_ShowString(32, 48, ":");        /* 冒号 X=32~40 */
    ShowStateText(state);                /* 文字 X=40~72 */

    OLED_Update();                       /* 首次全屏刷新 */

    /* ---- 主循环 ---- */
    while (1)
    {
        unsigned char key_evt = MatrixKey_Scan();   /* 边沿事件，两种模式共用 */

        /* ============================================================
         * 公共处理：A、星、井、D、0~9 键（自动/点动两种模式逻辑相同）
         *   用边沿检测，按一次触发一次，长按不重复
         * ============================================================ */
        if (key_evt != KEY_NONE)
        {
            switch (key_evt)
            {
                /*
                 * A键：模式切换
                 *   只能在停止时切换，运行中按A无效（安全保护）
                 *   自动 ↔ 点动 双向切换，同时取消未确认的数字输入
                 */
                case 'A':
                    if (state == STATE_STOP)
                    {
                        mode = (mode == MODE_AUTO) ? MODE_JOG : MODE_AUTO;
                        ShowModeText(mode);
                        need_update = 1;
                    }
                    break;

                /*
                 * 星键：占空比 +1（上限100，运行中立即生效）
                 *   调节后取消未确认的数字输入
                 */
                case '*':
                    if (duty < 100)
                    {
                        duty++;
                        Motor_SetSpeed(duty);
                        ShowDutyNum(duty);
                        need_update = 1;
                    }
                    break;

                /*
                 * 井键：占空比 -1（下限1）
                 *   调节后取消未确认的数字输入
                 */
                case '#':
                    if (duty > 1)
                    {
                        duty--;
                        Motor_SetSpeed(duty);
                        ShowDutyNum(duty);
                        need_update = 1;
                    }
                    break;

                /*
                 * 数字键 0~9：输入占空比两位数
                 *   保留最近输入的两位：新值 = (旧值%10)*10 + 本次数字
                 *   例：按5→05，再按3→53，再按7→37（挤出最旧的位）
                 *   输入过程在第三行最右侧实时预览，按D键确认后生效
                 */
                case '0': case '1': case '2': case '3': case '4':
                case '5': case '6': case '7': case '8': case '9':
                    input_buf = (input_buf % 10) * 10 + (key_evt - '0');
                    ShowInputNum(input_buf);           /* 最右侧预览输入数字 */
                    need_update = 1;
                    break;

                /*
                 * D键：参数确定
                 *   有输入时：输入值非0 → 更新duty并生效；输入值为0 → duty不变
                 *   确认后消隐输入预览区
                 */
                case 'D':
										if (input_buf > 0)             /* 输入非0 → 更新占空比 */
										{
												duty = input_buf;
												Motor_SetSpeed(duty);
										  	ShowDutyNum(duty);         
											  input_buf = 0;           /* 清零 → 为下一次参数准备*/         
										}
										/* input_buf == 0 时占空比不变 */
										ClearInputNum();               /* 消隐输入预览 */
										need_update = 1;
                    break;
            }
        }

        /* ============================================================
         * 模式相关处理：B、C 键（两种模式检测方式不同）
         *   自动模式 → 边沿检测，复用上面的 key_evt
         *   点动模式 → 电平检测，用 MatrixKey_Read()
         * ============================================================ */
        if (mode == MODE_AUTO)
        {
            /*
             * 自动模式：B/C 边沿检测，按一次触发一次
             *   B键 = 正转/停止 复用（互锁：反转时按B忽略）
             *   C键 = 反转/停止 复用（互锁：正转时按C忽略）
             */
            if (key_evt == 'B')
            {
                if (state == STATE_FWD)
                {
                    Motor_Coast();               /* 正转 → 停止 */
                    state = STATE_STOP;
                    ShowStateText(state);
                    need_update = 1;
                }
                else if (state == STATE_STOP)
                {
                    Motor_SetSpeed(duty);        /* 停止 → 正转 */
                    Motor_Forward();
                    state = STATE_FWD;
                    ShowStateText(state);
                    need_update = 1;
                }
                /* state == STATE_REV 时忽略：不能从反转直接切正转 */
            }
            else if (key_evt == 'C')
            {
                if (state == STATE_REV)
                {
                    Motor_Coast();               /* 反转 → 停止 */
                    state = STATE_STOP;
                    ShowStateText(state);
                    need_update = 1;
                }
                else if (state == STATE_STOP)
                {
                    Motor_SetSpeed(duty);        /* 停止 → 反转 */
                    Motor_Reverse();
                    state = STATE_REV;
                    ShowStateText(state);
                    need_update = 1;
                }
                /* state == STATE_FWD 时忽略：不能从正转直接切反转 */
            }
        }
        else /* MODE_JOG 点动模式 */
        {
            /*
             * 点动模式：B/C 电平检测，按住动、松开停
             * 按住B → 正转    按住C → 反转    其他 → 停止（松开即停）
             */
            unsigned char key_state = MatrixKey_Read();

            if (key_state == 'B')
            {
                if (state != STATE_FWD)           /* 避免重复调用 */
                {
                    Motor_SetSpeed(duty);
                    Motor_Forward();
                    state = STATE_FWD;
                    ShowStateText(state);
                    need_update = 1;
                }
            }
            else if (key_state == 'C')
            {
                if (state != STATE_REV)
                {
                    Motor_SetSpeed(duty);
                    Motor_Reverse();
                    state = STATE_REV;
                    ShowStateText(state);
                    need_update = 1;
                }
            }
            else
            {
                /* 没按B或C → 停止（松开即停）*/
                if (state != STATE_STOP)
                {
                    Motor_Coast();
                    state = STATE_STOP;
                    ShowStateText(state);
                    need_update = 1;
                }
            }
        }
        /* 有变化才刷屏，避免无谓刷新导致闪烁 */
        if (need_update)
        {
            OLED_Update();
            need_update = 0;
        }
        DelayMs(10);                         /* 主循环10ms节拍，兼做按键消抖时间基准 */
    }
}
