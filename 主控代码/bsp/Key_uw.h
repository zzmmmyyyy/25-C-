#ifndef _KEY_UW_H_
#define _KEY_UW_H_

#include "main.h"

extern uint8_t ucKey_Val, unKey_Down, ucKey_Up, ucKey_Old;		// 按键扫描变量

uint8_t Key_Scan(void);		// 按键取值
void Key_work(void);		// 按键处理

#endif
