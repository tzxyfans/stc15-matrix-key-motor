/*
 * stdint.h —— Keil C51 兼容头文件
 * Keil C51（C90）不带标准 stdint.h，为 OLED 模块的 uint8_t 等类型
 * 提供等效定义，避免 warning C318 / error C129。
 */
#ifndef __STDINT_COMPAT_H__
#define __STDINT_COMPAT_H__

typedef unsigned char  uint8_t;    /* 无符号 8 位 */
typedef unsigned int   uint16_t;   /* 无符号 16 位 */
typedef unsigned long  uint32_t;   /* 无符号 32 位 */
typedef signed   char  int8_t;     /* 有符号 8 位 */
typedef          int   int16_t;    /* 有符号 16 位 */
typedef          long  int32_t;    /* 有符号 32 位 */

#endif /* __STDINT_COMPAT_H__ */