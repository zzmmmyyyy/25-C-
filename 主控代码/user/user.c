#include "user.h"

__IO uint32_t uwTick_Key = 0;	// 控制Key的执行速度
__IO uint32_t uwTick_Send = 0;	// 控制Send的执行速度
__IO uint32_t uwTick_Adc = 0;	// 控制adc的读取速度
__IO uint32_t uwTick_Uart = 0;	// 控制UASRT的读取速度
__IO uint32_t uwTick_OLED = 0;	// 控制OLED的读取速度

struct DATA datas;					// 四个数据包

struct PARAM param;			// 变量数据
enum STATUS Status;

uint16_t adc_buff[200];			// 存放ADC采集的数据
__IO uint8_t adcConvEnd = 0;		// ADC采集标志
float voltage = 0.0f;			// 具体电压值

char c_flag;  // 指令 0:停止 1:开始
bool Mode_adc = false;				// 收发模式 0:停止发送 1：发送使能

void User_init(void)
{
	HAL_TIM_Base_Start(&htim3);			//	 开启定时器3
	HAL_ADCEx_Calibration_Start(&hadc1);		// ADC校准
	HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_buff,200);	// 
	
	HAL_UART_Receive_IT(&huart2,(uint8_t *)&c_flag,1);
	uart3_init();
	
	OLED_Init();			// OLED初始化
	
	Clear_Data();
	// 初始化数据缓冲区
	memset(&datas, 0, sizeof(datas));
}


// 数据清零
void Clear_Data(void)
{
	param.adc_value = 0;
	param.D_data = 0;
	param.x_data = 0;
	param.Is_data = 0;
	printf("x0.val=%d\xff\xff\xff",param.D_data);
	printf("x1.val=%d\xff\xff\xff",param.x_data);
	printf("x2.val=%d\xff\xff\xff",param.Is_data);
}

// 按键模拟
void Key_use(void)
{
    if((uwTick -  uwTick_Key)<40)	return;//减速函数
	uwTick_Key = uwTick;
	Key_work();
    if(unKey_Down == 1)
    {
		Clear_Data();
    }
    if(unKey_Down == 2)
    {
		SendData();		// 选择是否接收
    }
    if(unKey_Down == 3)
    {
		Mode_adc =! Mode_adc;
    }
    if(unKey_Down == 4)
    {
		
    }
}

// 5点滤波函数 - 处理数据不足的情况
void FilterData(uint8_t *data_array, uint8_t data_index)
{
    static uint16_t filter_buffer[2][5] = {0};  // 2个数据包，每个5个数据
    static uint8_t buffer_index[2] = {0};       // 缓冲区索引
    static uint8_t data_count[2] = {0};         // 实际数据数量
    
    // 将字符串转换为数字
    uint16_t current_value = 0;
    uint8_t len = strlen((char*)data_array);
    
    // 简单字符串转数字
    for(uint8_t i = 0; i < len; i++)
    {
        if(data_array[i] >= '0' && data_array[i] <= '9')
        {
            current_value = current_value * 10 + (data_array[i] - '0');
        }
    }
    
    // 将新数据加入滤波缓冲区
    filter_buffer[data_index][buffer_index[data_index]] = current_value;
    buffer_index[data_index] = (buffer_index[data_index] + 1) % 5;
    
    // 更新数据计数
    if(data_count[data_index] < 5)
    {
        data_count[data_index]++;
    }
    
    // 计算平均值（根据实际数据数量）
    uint32_t sum = 0;
    uint8_t valid_count = 0;
    
    // 只计算实际存在的数据
    for(uint8_t i = 0; i < data_count[data_index]; i++)
    {
        if(filter_buffer[data_index][i] != 0)
        {
            sum += filter_buffer[data_index][i];
            valid_count++;
        }
    }
    
    // 计算平均值并直接存储数字
    if(valid_count > 0)
    {
        uint16_t average = sum / valid_count;
        
        // 直接存储数字到全局变量
        if(data_index == 1)
        {
            param.D_data = average;  // 数据包3的滤波结果
        }
        else
        {
            param.x_data = average;  // 数据包4的滤波结果
        }
    }
}


// 获取摄像头的数据
void GetData(void)
{
    if((uwTick -  uwTick_Uart)<2)	return;  // 2ms快速
    uwTick_Uart = uwTick;
    
    if(rx_3_Flag != false)
    {			
        uint8_t i = 0, len = 0;
        char delimiters[] = {'A', 'X', 'D', 'B'};
        uint8_t *data_arrays[] = {datas.Data1, datas.Data2, datas.Data3, datas.Data4};
        
        // 清空数据缓冲区
        memset(&datas, 0, sizeof(datas));
        
        // 快速提取
        for(uint8_t packet = 0; packet < 4; packet++)
        {
            len = 0;
            while(i < rxDatalen3 && rxUse3[i] != delimiters[packet] && len < 9)
            {
                // 跳过小数点，只保留数字
                if(rxUse3[i] != '.')
                {
                    data_arrays[packet][len++] = rxUse3[i];
                }
                i++;
            }
            data_arrays[packet][len] = '\0';
            if(i < rxDatalen3 && rxUse3[i] == delimiters[packet]){ i++;}
			else { break; }
        }
        
		FilterData(datas.Data3, 0);  // 数据包3，结果存储到 param.x_data
        FilterData(datas.Data4, 1);  // 数据包4，结果存储到 param.D_data
		
        // 立即重启DMA
//        HAL_UART_Receive_DMA(&huart3, (uint8_t *)rxUse3, USE_LEN);
        rx_3_Flag = false;
    }
}

// 添加修改显示
void putData(void)
{
	if((uwTick -  uwTick_OLED)<200)	return;//减速函数
	uwTick_OLED = uwTick;
	OLED_ShowString(1, 1, "Target:");
	OLED_ShowString(2, 1, " True: ");
	OLED_ShowString(1, 8, (char *)datas.Data1);
	OLED_ShowString(2, 8, (char *)datas.Data2);
	OLED_ShowString(3, 1, (char *)datas.Data3);
	OLED_ShowString(4, 1, (char *)datas.Data4);
}

// 为串口屏发送数据
void SendData(void)
{
//	if((uwTick -  uwTick_Send)<40)	return;//减速函数
//	uwTick_Send = uwTick;
//	if(Mode != false)
	{
		printf("x0.val=%s\xff\xff\xff",datas.Data3);
		printf("x1.val=%s\xff\xff\xff",datas.Data4);
	}
}

void Get_ADC(void)
{
	if((uwTick -  uwTick_Adc)<300)	return;//减速函数
	uwTick_Adc = uwTick;
	
	if(Mode_adc)
	{
		printf("x2.val=%d\xff\xff\xff",param.Is_data);
	}
	
}
