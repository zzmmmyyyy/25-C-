#include "uart.h"

uint16_t point2 = 0;

uint8_t USART2_RX[USE_LEN];		// 接收缓冲，最大USE_LEN字节
uint8_t USART2_TX[USE_LEN];		// 发送缓冲，最大USE_LEN字节

__IO bool rx_3_Flag = false;
__IO uint8_t rxUse3[USE_LEN] = {0};
__IO uint8_t rxDatalen3 = 0;

void uart3_init(void)
{
	__HAL_UART_CLEAR_IDLEFLAG(&huart3); 								// 清除IDLE标志
    __HAL_UART_ENABLE_IT(&huart3, UART_IT_IDLE); 						// 使能串UART5 IDLE中断
    HAL_UART_Receive_DMA(&huart3, (uint8_t *)rxUse3, USE_LEN);           // 开启DMA接收模式
}

// void USART3_IRQHandler(void)  找到UART3的全局中断对应的ISR并将以下函数放入
void uart3_receive(void)
{
	if(__HAL_UART_GET_FLAG(&huart3, UART_FLAG_IDLE) != RESET)
    {
        __HAL_UART_CLEAR_IDLEFLAG(&huart3);     //清除IDLE标志

        HAL_UART_DMAStop(&huart3);              // 停止DMA，为了重新设置DMA发送多少数据

        rxDatalen3 = USE_LEN - __HAL_DMA_GET_COUNTER(&hdma_usart3_rx);    // 计算数据包长度

        rx_3_Flag = true;       // 置位一帧命令接收完毕标志位
		
        HAL_UART_Receive_DMA(&huart3, (uint8_t *)rxUse3, USE_LEN);
    }
}

// 串口2重定向
int fputc(int ch, FILE *f)
{
	uint8_t temp[1] = {ch};
	HAL_UART_Transmit(&huart2,temp, 1, 2);
	return ch;
}

void USART2_CLR_Buf(void)
{
	memset(USART2_RX, 0, USE_LEN);		// 清空
	point2 = 0;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart == &huart2)	// 串口接收
	{
		if(point2<USE_LEN)		// 还可以接收数据
		{
			USART2_RX[point2++]=c_flag;	// 记录接收到的值
		}
	}
	
	// 重新设置中断
	HAL_UART_Receive_IT(huart,(uint8_t *)&c_flag, 1);
}

