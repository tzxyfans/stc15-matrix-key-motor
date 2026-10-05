/*
 * motor.c —— 电机驱动模块内部实现
 *
 * ============================================================================
 * PCA 三模块的寄存器与引脚对应（默认引脚映射 CCP_S=00）
 * ============================================================================
 *   ┌──────┬────────┬────────┬────────┬──────────┬──────┬──────────────┐
 *   │ 模块 │ 模式   │ 比较值L│ 比较值H│ PWM控制  │ 引脚 │ 推挽配置     │
 *   │  n   │ CCAPMn │ CCAPnL │ CCAPnH │ PCA_PWMn │ CCPn │              │
 *   ├──────┼────────┼────────┼────────┼──────────┼──────┼──────────────┤
 *   │  0   │ 0xDA   │ 0xEA   │ 0xFA   │ 0xF2     │ P1.1 │ P1M0|=0x02   │
 *   │  1   │ 0xDB   │ 0xEB   │ 0xFB   │ 0xF3     │ P1.0 │ P1M0|=0x01   │
 *   │  2   │ 0xDC   │ 0xEC   │ 0xFC   │ 0xF4     │ P3.7 │ P3M0|=0x80   │
 *   └──────┴────────┴────────┴────────┴──────────┴──────┴──────────────┘
 *
 *   三模块共用同一个 PCA 计数器 CL/CH 和 CMOD/CCON，只是比较值寄存器独立。
 *   切换模块只需改 motor.h 的 MOTOR_PCA_MOD 宏（0/1/2），本文件自动适配。
 *
 * ============================================================================
 * 为什么用 switch 封装而不直接写 CCAP1L？
 * ============================================================================
 *   C51 语言限制：sfr 寄存器不能取地址（&CCAP1L 非法），不能用指针数组
 *   统一访问。所以用 switch 按模块号分发到对应 sfr，实现"一处配置、处处适配"。
 *   这是 STC15 多模块外设的通用移植手法。
 * ============================================================================
 */
#include "motor.h"

/* DIR / BRAKE 引脚定义（PWM 引脚由 PCA 硬件自动控制，不需要 sbit）*/
sbit DIR   = P0^4;            /* 方向控制端：高电平正转，低电平反转 */
sbit BRAKE = P4^7;            /* 刹车控制端：低电平刹车有效，高电平松闸 */

/* 当前占空比缓存（Motor_SetSpeed 写，Motor_Forward/Reverse 读）*/
static unsigned char s_duty = 50;   /* 默认 50% */

/*==================================================
 * PCA 寄存器访问封装（按模块号分发，C51 sfr 不能取地址）
 *==================================================*/

/* 写模块模式寄存器 CCAPMn */
static void PCA_wr_CCAPM(unsigned char mod, unsigned char val)
{
    switch (mod) {
        case 0: CCAPM0 = val; break;
        case 1: CCAPM1 = val; break;
        case 2: CCAPM2 = val; break;
        default: break;
    }
}

/* 写比较值低字节 CCAPnL（占空比核心，立即生效）*/
static void PCA_wr_CCAPL(unsigned char mod, unsigned char val)
{
    switch (mod) {
        case 0: CCAP0L = val; break;
        case 1: CCAP1L = val; break;
        case 2: CCAP2L = val; break;
        default: break;
    }
}

/* 写比较值高字节 CCAPnH（溢出重装值，保持占空比稳定）*/
static void PCA_wr_CCAPH(unsigned char mod, unsigned char val)
{
    switch (mod) {
        case 0: CCAP0H = val; break;
        case 1: CCAP1H = val; break;
        case 2: CCAP2H = val; break;
        default: break;
    }
}

/* 写 PWM 控制寄存器 PCA_PWMn（选 PWM 位数）*/
static void PCA_wr_PWM(unsigned char mod, unsigned char val)
{
    switch (mod) {
        case 0: PCA_PWM0 = val; break;
        case 1: PCA_PWM1 = val; break;
        case 2: PCA_PWM2 = val; break;
        default: break;
    }
}

/* 读比较值低字节 CCAPnL（判断 PWM 是否在运行）*/
static unsigned char PCA_rd_CCAPL(unsigned char mod)
{
    switch (mod) {
        case 0: return CCAP0L;
        case 1: return CCAP1L;
        case 2: return CCAP2L;
        default: return 0;
    }
}

/*
 * PCA_ConfigPin —— 配置 PWM 引脚为推挽输出
 *   PWM 输出需要强驱动，准双向口边沿太慢会使电机异响
 *   推挽：M1=0, M0=1；按模块号配置对应引脚
 */
static void PCA_ConfigPin(unsigned char mod)
{
    switch (mod) {
        case 0: P1M1 &= ~0x02; P1M0 |= 0x02; break;  /* CCP0 = P1.1 推挽 */
        case 1: P1M1 &= ~0x01; P1M0 |= 0x01; break;  /* CCP1 = P1.0 推挽 */
        case 2: P3M1 &= ~0x80; P3M0 |= 0x80; break;  /* CCP2 = P3.7 推挽 */
        default: break;
    }
}

/*==================================================
 * PWM 启停控制（通过封装函数访问，自动适配 MOTOR_PCA_MOD）
 *==================================================*/

/*
 * PWM_HW_Init —— PCA 硬件初始化
 *   配置 PCA 时钟源、模块模式、PWM 位数，启动计数器
 *   占空比初值设为 0（不驱动），由 Motor_Forward/Reverse 启动
 */
static void PWM_HW_Init(void)
{
    PCA_ConfigPin(MOTOR_PCA_MOD);      /* PWM 引脚推挽输出 */

    CCON = 0x00;                        /* 清 PCA 控制寄存器，从停机状态干净启动 */

    /*
     * CMOD 位分布（STC15F2K60S2 官方手册）：
     *   bit7=CIDL  bit6~4=保留  bit3~1=CPS[2:0]  bit0=ECF
     * 要选 CPS=001 (SYSclk/2)，需把 bit1 置1 → CMOD=0x02
     * ⚠️ 易错：CMOD=0x01 是 bit0=1(ECF=1)+CPS=000(SYSclk/12)→频率只有3.6KHz
     */
    CMOD = 0x02;                        /* CPS[2:0]=001 → SYSclk/2，8位PWM频率≈21.6KHz */

    PCA_wr_PWM(MOTOR_PCA_MOD, 0x00);   /* EBS[1:0]=00 → 8位PWM模式 */
    PCA_wr_CCAPM(MOTOR_PCA_MOD, 0x42); /* ECOM(bit6)+PWM(bit1) → 纯硬件输出 */

    PCA_wr_CCAPH(MOTOR_PCA_MOD, 0);    /* 重装值=0，停止驱动（持续高电平）*/
    PCA_wr_CCAPL(MOTOR_PCA_MOD, 0);    /* 当前值=0 */

    CR = 1;                             /* 启动 PCA 计数器，此时占空比0%=不驱动 */
}

/*
 * PWM_Start —— 启动 PWM 驱动（按当前占空比输出方波）
 *   原理：写 CCAPnL = s_duty×256/100，硬件自动在 CCPn 引脚输出方波
 */
static void PWM_Start(void)
{
    unsigned char val = (unsigned char)(s_duty * 256 / 100);
    PCA_wr_CCAPH(MOTOR_PCA_MOD, val);  /* 重装值，保证占空比稳定 */
    PCA_wr_CCAPL(MOTOR_PCA_MOD, val);  /* 当前值，立即生效 */
}

/*
 * PWM_Stop —— 停止 PWM 驱动（CCPn 引脚持续高电平 = 不驱动）
 *   原理：CCAPnL=0 → CL>=0 恒成立 → 输出持续高电平
 */
static void PWM_Stop(void)
{
    PCA_wr_CCAPH(MOTOR_PCA_MOD, 0);
    PCA_wr_CCAPL(MOTOR_PCA_MOD, 0);
}

/*==================================================
 * 对外接口实现
 *==================================================*/

void Motor_Init(void)
{
    /*
     * 端口模式：
     *   CCPn (PWM)  → 推挽输出（在 PWM_HW_Init 里按 MOTOR_PCA_MOD 配）
     *   P0.4 (DIR)  → 准双向口（M1=0, M0=0）
     *   P4.7 (BRAKE)→ 准双向口（M1=0, M0=0）
     */
    P0M1 &= ~0x10;                      /* P0.4 准双向口 */
    P0M0 &= ~0x10;
    P4M1 &= ~0x80;                      /* P4.7 准双向口 */
    P4M0 &= ~0x80;

    PWM_HW_Init();                      /* PCA PWM 硬件初始化（占空比0%=不驱动）*/

    /*
     * 默认安全状态：惯性停转
     *   BRAKE = 1 松闸，DIR = 1 方向默认正转，PWM 已停止驱动
     *   开机即让电机处于断电自由状态，避免上电瞬间意外转动。
     */
    BRAKE = 1;
    DIR   = 1;
}

void Motor_Forward(void)
{
    BRAKE = 1;                          /* 1. 先松闸，否则刹车会强制锁死电机 */
    DIR   = 1;                          /* 2. 设置正转方向 */
    PWM_Start();                        /* 3. 启动 PWM 方波，按当前占空比驱动 */
}

void Motor_Reverse(void)
{
    BRAKE = 1;                          /* 1. 先松闸 */
    DIR   = 0;                          /* 2. 设置反转方向 */
    PWM_Start();                        /* 3. 启动 PWM 方波，按当前占空比驱动 */
}

void Motor_Coast(void)
{
    /*
     * 惯性停转：断开驱动、不刹车，电机靠自身惯性与绕组阻尼自由减速直至停止。
     * 方向位 DIR 保持不变（不驱动时方向无意义，保留上次方向便于再次启动）。
     */
    PWM_Stop();                         /* 停 PWM 驱动，CCPn 持续高电平 */
    BRAKE = 1;                          /* 松闸，不短接电机两端 */
}

void Motor_Brake(void)
{
    /*
     * 刹车停转：先停止驱动，再拉低 BRAKE 短接电机两端，
     * 电机绕组切割磁力线产生反向电流，形成制动力矩，快速停转。
     * 顺序不能反：若先刹车再停 PWM，驱动与刹车会瞬间冲突。
     */
    PWM_Stop();                         /* 1. 先停驱动，避免与刹车冲突 */
    BRAKE = 0;                          /* 2. 拉低 BRAKE，短接电机两端快速制动 */
}

void Motor_SetSpeed(unsigned char duty)
{
    if (duty > 100) duty = 100;         /* 限幅 0~100 */
    s_duty = duty;                      /* 缓存新占空比 */
    /*
     * 若 PWM 正在运行（CCAPnL != 0），立即更新比较值，占空比实时生效；
     * 若已停止（CCAPnL == 0），只更新缓存，下次 Motor_Forward/Reverse 时生效。
     */
    if (PCA_rd_CCAPL(MOTOR_PCA_MOD) != 0)
    {
        unsigned char val = (unsigned char)(s_duty * 256 / 100);
        PCA_wr_CCAPH(MOTOR_PCA_MOD, val);
        PCA_wr_CCAPL(MOTOR_PCA_MOD, val);
    }
}

unsigned char Motor_GetSpeed(void)
{
    return s_duty;
}