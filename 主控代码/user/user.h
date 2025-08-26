#ifndef _USER_H_
#define _USER_H_

#include "tim.h"
#include "adc.h"
#include "main.h"
#include "usart.h"
#include "string.h"

#include "uart.h"
#include "OLED.h"
#include "Key_uw.h"

struct PARAM
{
	uint32_t adc_value;		// ADC读取数值
	uint32_t D_data;		// 物面距离摄像头的距离
	uint32_t x_data;		// 检测物体边长或直径的数据
	uint32_t Is_data;		// 直流稳压电源对测量装置整机的供电电流 Is
};

struct DATA
{
	uint8_t Data1[10];
	uint8_t Data2[10];
	uint8_t Data3[10];
	uint8_t Data4[10];
};

enum STATUS
{
	data1=1,
	data2=2,
	data3=3,
	data4=4,
};

void Key_use(void);
void User_init(void);
void Clear_Data(void);
void putData(void);
void SendData(void);
void GetData(void);
void Get_ADC(void);
void SafeShowString(uint8_t line, uint8_t col, uint8_t *data);

extern struct PARAM param;		// 一些参数
extern struct DATA datas;		// 四个数据包
extern enum STATUS Status;		// 解析数据包的四个数据
extern bool Mode;				// 收发模式

#endif
