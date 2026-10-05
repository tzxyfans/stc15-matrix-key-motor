/*
 * delay.c ���� ��ʱģ���ڲ�ʵ�֣�@11.0592MHz��12T ģʽ��
 */
#include "delay.h"
#include <intrins.h>           /* _nop_() ��ָ�� */

void Delay1ms(void)            /* @11.0592MHz��Լ 1ms */
{
    unsigned char data i, j;
    _nop_();
    _nop_();
    _nop_();
    i = 11;
    j = 190;
    do
    {
        while (--j);
    } while (--i);
}

void DelayMs(unsigned int ms) /* �������������ʱ */
{
    unsigned int  n;
    for (n = 0; n < ms; n++)
    {
        Delay1ms();            /* ÿѭ��һ��Լ 1ms */
    }
}
