#include "Key_uw.h"

uint8_t ucKey_Val = 0;
uint8_t unKey_Down = 0;
uint8_t ucKey_Up = 0;
uint8_t ucKey_Old = 0;

uint8_t Key_Scan(void)
{
	uint8_t unKey_Val = 0;
	
	if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15) == GPIO_PIN_RESET)
		unKey_Val = 1;

	if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14) == GPIO_PIN_RESET)
		unKey_Val = 2;

	if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13) == GPIO_PIN_RESET)
		unKey_Val = 3;
	
	if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12) == GPIO_PIN_RESET)
		unKey_Val = 4;	
	
	return unKey_Val;
}

void Key_work(void)
{
	ucKey_Val = Key_Scan();
	unKey_Down = ucKey_Val & (ucKey_Old ^ ucKey_Val); 
	ucKey_Up = ~ucKey_Val & (ucKey_Old ^ ucKey_Val);	
	ucKey_Old = ucKey_Val;
}

// 使用前带上这个减速函数
//void Key_Proc(void)
//{
//	if((uwTick -  uwTick_Key)<40)	return;//减速函数
//		uwTick_Key = uwTick;



//	// 示例
////	if(unKey_Down == 4)
////	{
////		ucLed = 0x88;
////	}
////	if(unKey_Down == 3)
////	{
////		ucLed = 0x00;
////	}	

//}

