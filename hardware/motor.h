/*
 * motor.h —— 电机驱动模块对外接口
 *
 * ============================================================================
 * PCA 模块选择（改这一个宏即可切换 PWM 输出引脚，无需改其他代码）
 * ============================================================================
 *   #define MOTOR_PCA_MOD 0  →  CCP0 = P1.1
 *   #define MOTOR_PCA_MOD 1  →  CCP1 = P1.0  （默认）
 *   #define MOTOR_PCA_MOD 2  →  CCP2 = P3.7
 *
 *   三组模块共用同一个 PCA 计数器（CL/CH），8位PWM频率均为 21.6KHz。
 *   引脚映射用默认组 CCP_S=00（P_SW1 bit5:4=00），无需配置 P_SW1。
 *
 * ============================================================================
 * 控制端接线
 * ============================================================================
 *   PWM   = CCPn（由 MOTOR_PCA_MOD 决定，n=0/1/2）
 *                   低电平 = 驱动动作，高电平 = 惯性阻尼停止
 *                   占空比 = 电机驱动时间占比（0~100%）
 *   DIR   = P0.4  方向控制：高电平 = 正转，低电平 = 反转
 *   BRAKE = P4.7  刹车控制：低电平 = 刹车有效（短接电机两端快速停转），
 *                   高电平 = 松闸（恢复 PWM 控制）
 *
 * ============================================================================
 * 对外接口
 * ============================================================================
 *   Motor_Init       初始化 PCA PWM + 端口，默认惯性停转（安全状态）
 *   Motor_Forward    正转：松闸 → 方向正 → 启动 PWM 方波（当前占空比）
 *   Motor_Reverse    反转：松闸 → 方向反 → 启动 PWM 方波
 *   Motor_Coast      惯性停转：停 PWM 驱动、不刹车，靠惯性与阻尼自由减速
 *   Motor_Brake      刹车停转：先停 PWM 驱动 → 再拉低 BRAKE 短接电机两端
 *   Motor_SetSpeed   设置占空比 0~100（运行时改，立即生效）
 *   Motor_GetSpeed   读取当前占空比
 */
#ifndef __MOTOR_H__
#define __MOTOR_H__

#include <STC15.H>

/* PCA 模块选择：0=CCP0(P1.1) 1=CCP1(P1.0) 2=CCP2(P3.7) */
#ifndef MOTOR_PCA_MOD
#define MOTOR_PCA_MOD  1
#endif

void Motor_Init(void);                   /* 初始化PCA PWM+端口，默认惯性停转 */
void Motor_Forward(void);                /* 正转：松闸→方向正→启动PWM方波 */
void Motor_Reverse(void);                /* 反转：松闸→方向反→启动PWM方波 */
void Motor_Coast(void);                  /* 惯性停转：停PWM驱动→松闸 */
void Motor_Brake(void);                  /* 刹车停转：停PWM驱动→短接电机两端 */
void Motor_SetSpeed(unsigned char duty); /* 设占空比0~100 */
unsigned char Motor_GetSpeed(void);      /* 读当前占空比 */

#endif /* __MOTOR_H__ */