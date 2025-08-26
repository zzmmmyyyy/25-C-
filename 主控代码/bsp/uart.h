#ifndef _UART_H_
#define _UART_H_

#include "usart.h"
#include "stdio.h"
#include "string.h"
#include "stdbool.h"
#include "stm32f1xx_it.h"

extern DMA_HandleTypeDef hdma_usart3_rx;
extern char c_flag;  // 指令 0:停止 1:开始

#define USE_LEN 255

// 串口2使用函数数据
extern size_t strlen(const char *);

extern uint8_t USART2_RX[USE_LEN];		// 接收缓冲，最大USE_LEN字节
extern uint8_t USART2_TX[USE_LEN];		// 发送缓冲，最大USE_LEN字节
extern uint16_t USART2_STA;				// 	接收数据状态
extern uint16_t point2;			

// 串口3使用函数数据
extern __IO bool rx_3_Flag;
extern __IO uint8_t rxUse3[USE_LEN];
extern __IO uint8_t rxDatalen3;

void USART2_CLR_Buf(void);	// 串口缓存清理
void uart3_init(void);		// 通信初始化
void uart3_receive(void);	// 处理数据接收信息

#endif
