/*
 * delay.h —— 延时模块对外接口（@11.0592MHz）
 */
#ifndef __DELAY_H__
#define __DELAY_H__

void Delay1ms(void);                 /* 精确延时约 1ms */
void DelayMs(unsigned int ms);      /* 带毫秒参数的延时：ms=20 → 约 20ms */

#endif /* __DELAY_H__ */
