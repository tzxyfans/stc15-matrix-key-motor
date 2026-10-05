/*
 * key.h —— 按键模块对外接口（项目二：新增"放开事件"识别，扩展至 S4~S7 四键）
 * 接线：S7 = P3.0，S6 = P3.1，S5 = P3.2，S4 = P3.3（独立按键，10k上拉，按下为低电平）
 */
#ifndef __KEY_H__
#define __KEY_H__

#include <STC15.H>

void Key_Init(void);                 /* 初始化：准双向口，写 1，历史状态清零 */
unsigned char Key_S4_Down(void);     /* S4 当前按下？按下返回 1，放开返回 0 */
unsigned char Key_S5_Down(void);     /* S5 当前按下？按下返回 1，放开返回 0 */
unsigned char Key_S6_Down(void);     /* S6 当前按下？按下返回 1，放开返回 0 */
unsigned char Key_S7_Down(void);     /* S7 当前按下？按下返回 1，放开返回 0 */
unsigned char Key_S4_Released(void); /* S4 放开事件？按下后放开的那一次返回 1 */
unsigned char Key_S5_Released(void); /* S5 放开事件？按下后放开的那一次返回 1 */
unsigned char Key_S6_Released(void); /* S6 放开事件？按下后放开的那一次返回 1 */
unsigned char Key_S7_Released(void); /* S7 放开事件？按下后放开的那一次返回 1 */

#endif /* __KEY_H__ */
