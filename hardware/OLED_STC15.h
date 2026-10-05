#ifndef __OLED_STC15_H
#define __OLED_STC15_H

#include "stdint.h"
#include "OLED_Data.h"

  
/*参数宏定义*********************/

/*字体像素宽度（供函数内部做字宽计算与字库选择使用）*/
/*字体接口已简化：常规显示函数默认大字 OLED_8X16（宽8像素，高16像素），
 *需要小字 OLED_6X8（宽6像素，高8像素）时，调用带 Small 后缀的同名函数*/
#define OLED_8X16				8
#define OLED_6X8				6

/*IsFilled参数数值*/
#define OLED_UNFILLED			0
#define OLED_FILLED				1

/*********************参数宏定义*/


/*函数声明*********************/

/*初始化函数*/
void OLED_Init(void);

/*更新函数*/
void OLED_Update(void);
void OLED_UpdateArea(uint8_t X, uint8_t Y, uint8_t Width, uint8_t Height);

/*显存控制函数*/
void OLED_Clear(void);
void OLED_ClearArea(uint8_t X, uint8_t Y, uint8_t Width, uint8_t Height);
void OLED_Reverse(void) reentrant;
void OLED_ReverseArea(uint8_t X, uint8_t Y, uint8_t Width, uint8_t Height);
void OLED_Rotate180(void);
void OLED_SetNormal(void);

/*显示函数*/
/*字体简化说明：常规函数默认大字 OLED_8X16（宽8像素，高16像素），
 *需要小字 OLED_6X8（宽6像素，高8像素）时，调用带 Small 后缀的同名函数*/
void OLED_ShowChar(uint8_t X, uint8_t Y, char Char);
void OLED_ShowCharSmall(uint8_t X, uint8_t Y, char Char);
void OLED_ShowString(uint8_t X, uint8_t Y, char *String);
void OLED_ShowStringSmall(uint8_t X, uint8_t Y, char *String);
void OLED_ShowNum(uint8_t X, uint8_t Y, uint32_t Number, uint8_t Length);
void OLED_ShowNumNoZero(uint8_t X, uint8_t Y, uint32_t Number, uint8_t Length);/* 高位0消隐 */
void OLED_ShowNumSmall(uint8_t X, uint8_t Y, uint32_t Number, uint8_t Length);
void OLED_ShowSignedNum(uint8_t X, uint8_t Y, int32_t Number, uint8_t Length);
void OLED_ShowSignedNumSmall(uint8_t X, uint8_t Y, int32_t Number, uint8_t Length);
void OLED_ShowHexNum(uint8_t X, uint8_t Y, uint32_t Number, uint8_t Length);
void OLED_ShowHexNumSmall(uint8_t X, uint8_t Y, uint32_t Number, uint8_t Length);
void OLED_ShowBinNum(uint8_t X, uint8_t Y, uint32_t Number, uint8_t Length);
void OLED_ShowBinNumSmall(uint8_t X, uint8_t Y, uint32_t Number, uint8_t Length);
void OLED_ShowFloatNum(uint8_t X, uint8_t Y, double Number, uint8_t IntLength, uint8_t FraLength);
void OLED_ShowFloatNumSmall(uint8_t X, uint8_t Y, double Number, uint8_t IntLength, uint8_t FraLength);
void OLED_ShowChinese(uint8_t X, uint8_t Y, char *Chinese);
void OLED_ShowImage(uint8_t X, uint8_t Y, uint8_t Width, uint8_t Height, const uint8_t *Image);
void OLED_Printf(uint8_t X, uint8_t Y, char *format, ...);
void OLED_PrintfSmall(uint8_t X, uint8_t Y, char *format, ...);
void OLED_ShowMixedText(uint8_t X, uint8_t Y, char *Text);
void OLED_ShowMixedTextSmall(uint8_t X, uint8_t Y, char *Text);

/*绘图函数*/
void OLED_DrawPoint(uint8_t X, uint8_t Y);
uint8_t OLED_GetPoint(uint8_t X, uint8_t Y) reentrant;
void OLED_DrawLine(uint8_t X0, uint8_t Y0, uint8_t X1, uint8_t Y1);
void OLED_DrawRectangle(uint8_t X, uint8_t Y, uint8_t Width, uint8_t Height, uint8_t IsFilled);
void OLED_DrawTriangle(uint8_t X0, uint8_t Y0, uint8_t X1, uint8_t Y1, uint8_t X2, uint8_t Y2, uint8_t IsFilled) reentrant;
void OLED_DrawCircle(uint8_t X, uint8_t Y, uint8_t Radius, uint8_t IsFilled);
void OLED_DrawEllipse(uint8_t X, uint8_t Y, uint8_t A, uint8_t B, uint8_t IsFilled);
void OLED_DrawArc(uint8_t X, uint8_t Y, uint8_t Radius, int16_t StartAngle, int16_t EndAngle, uint8_t IsFilled) reentrant;

/*********************函数声明*/

#endif


/*****************江协科技|版权所有****************/
/*****************jiangxiekeji.com*****************/