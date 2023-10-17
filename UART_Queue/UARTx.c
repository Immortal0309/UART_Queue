#include "UARTx.h"

// 串口发送状态
typedef enum
{
  UART_STATE_OFF, // 串口未发送数据
  UART_STATE_ON,  // 串口正在发送数据
  UART_STATE_MAX
} EnumUARTState;

static StructCirQue UART_send_cir_queue; // 发送串口循环队列
static StructCirQue UART_rec_cir_queue;  // 接收串口循环队列

static uint8_t UART_send_buff[UART_BUF_SIZE]; // 串口发送循环队列的缓冲区
static uint8_t UART_rec_buff[UART_BUF_SIZE];  // 串口接收循环队列的缓冲区

static uint8_t UART_tx_state; // 串口发送数据状态
static uint8_t UART_rx_data; // 串口接收数据单字节缓冲

static void UART_init_buff(void);                // 初始化串口缓冲区，包括发送缓冲区和接收缓冲区
static uint8_t write_receive_buff(uint8_t d);    // 将接收到的数据写入接收缓冲区
static uint8_t read_send_buff(uint8_t *p);       // 读取发送缓冲区中的数据
static void UART_config(void);                 // 配置串口相关的参数，包括GPIO、RCC、USART和NVIC
static void UART_tx_enable(void);               // 使能串口发送，UART_writex中调用，每次发送数据之后需要调用
static void send_char_fputc(uint16_t ch); // 发送字符函数，专由fputc函数调用

/**
 * @brief  UART_init_buff
 * @note   初始化串口缓冲区，包括发送缓冲区和接收缓冲区
 * @param  无
 * @param  无
 * @param  无
 * @retval 无
 */
static void UART_init_buff(void)
{
  InitQueue(&UART_send_cir_queue, UART_send_buff, UART_BUF_SIZE);
  InitQueue(&UART_rec_cir_queue, UART_rec_buff, UART_BUF_SIZE);
}

/**
 * @brief  write_receive_buff
 * @note   写数据到串口接收缓冲区
 * @param  d: 待写入串口接收缓冲区的数据
 * @param  无
 * @param  无
 * @retval 写入数据成功标志，0-不成功，1-成功
 */
static uint8_t write_receive_buff(uint8_t d)
{
  uint16_t i;

  i = EnQueue(&UART_rec_cir_queue, &d, 1);

  return i; // 返回写入数据成功标志，0-不成功，1-成功
}

/**
 * @brief  read_send_buff
 * @note   读取串口发送缓冲区中的数据
 * @param  p: 读出来的数据存放的首地址
 * @param  无
 * @param  无
 * @retval 读取数据成功标志，0-不成功，1-成功
 */
static uint8_t read_send_buff(uint8_t *p)
{
  uint16_t i;

  i = DeQueue(&UART_send_cir_queue, p, 1);

  return i; // 返回读取数据成功标志，0-不成功，1-成功
}

/**
 * @brief  UART_config
 * @note   配置串口相关的参数，包括GPIO、RCC、USART和NVIC
 * @param  无
 * @param  无
 * @param  无
 * @retval 无
 */
static void UART_config(void)
{
  // 串口接收中断开启
  HAL_UART_Receive_IT(&UART_HANDLE, &UART_rx_data, 1);

  // 串口发送数据状态设置为未发送数据
  UART_tx_state = UART_STATE_OFF;
}

/**
 * @brief  UART_tx_enable
 * @note   使能串口发送，在UART_writex中调用，即每次发送数据之后需要调用这个函数来使能发送中断
 * @param  无
 * @param  无
 * @param  无
 * @retval 无
 */
static void UART_tx_enable(void)
{
  UART_tx_state = UART_STATE_ON; // 串口发送数据状态设置为正在发送数据
}

/**
 * @brief  send_char_fputc
 * @note   发送字符函数，专由fputc函数调用
 * @param  ch，待发送的字符
 * @param  无
 * @param  无
 * @retval 无
 */
static void send_char_fputc(uint16_t ch)
{
  UART_HANDLE.Instance->TDR = (uint8_t)(ch & 0xFFU);

  // 等待发送完毕
  while ((__HAL_UART_GET_FLAG(&UART_HANDLE, UART_FLAG_TXE) ? SET : RESET) == RESET)
  {
  }
}

/**
 * @brief  UART_tx_scan
 * @note   USARTx 循环服务函数
 * @param  无
 * @param  无
 * @param  无
 * @retval 无
 */
void UART_tx_scan(void)
{
  uint8_t uData = 0;

  // 当发送状态 为 开启发送时
  if (UART_tx_state == UART_STATE_ON)
  {
    read_send_buff(&uData); // 读取发送缓冲区的数据到uData

    send_char_fputc(uData); // 将uData写入USART_DR

    if (QueueEmpty(&UART_send_cir_queue)) // 当发送缓冲区为空时
    {
      UART_tx_state = UART_STATE_OFF; // 串口发送数据状态设置为未发送数据
    }
  }
}

/**
 * @brief  HAL_UART_RxCpltCallback
 * @note   USARTx 中断服务回调函数
 * @param  无
 * @param  无
 * @param  无
 * @retval 无
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == UART_ID)
  {
    // 将 USART1 接收到的数据从 s_iUARTRxDat 取出
    write_receive_buff(UART_rx_data); // 将接收到的数据写入接收缓冲区
    // 继续开启接收中断
    HAL_UART_Receive_IT(&UART_HANDLE, &UART_rx_data, 1);
  }
}

/**
 * @brief  UART_init
 * @note   初始化UART模块
 * @param  无
 * @param  无
 * @param  无
 * @retval 无
 */
void UART_init(void)
{
  UART_init_buff(); // 初始化串口缓冲区，包括发送缓冲区和接收缓冲区
  UART_config();  // 配置串口相关的参数，包括GPIO、RCC、USART和NVIC
}

/**
 * @brief  UART_write
 * @note   写串口，即写数据到的串口发送缓冲区
 * @param  pBuf，要写入数据的首地址，len，期望写入数据的个数
 * @param  无
 * @param
 * @retval 成功写入数据的个数，不一定与形参len相等
 */
uint8_t UART_write(uint8_t *pBuf, uint8_t len)
{
  uint8_t wLen = 0; // 实际写入数据的个数

  wLen = EnQueue(&UART_send_cir_queue, pBuf, len);

  if (wLen > 0)
  {
    if (UART_tx_state == UART_STATE_OFF)
    {
      UART_tx_enable();
    }
  }

  return wLen; // 返回实际写入数据的个数
}

/**
 * @brief  UART_read1
 * @note   读串口，即读取串口接收缓冲区中的数据
 * @param  pBuf: 读取的数据存放的首地址，
 * @param  len: 期望读取数据的个数
 * @param  无
 * @retval 成功读取数据的个数，不一定与形参len相等
 */
uint8_t UART_read(uint8_t *pBuf, uint8_t len)
{
  uint16_t rLen;

  rLen = DeQueue(&UART_rec_cir_queue, pBuf, len);

  return rLen; // 返回实际读取数据的长度
}

/**
 * @brief  UART_read_queue_len
 * @note   获取串口接收队列的元素数量
 * @param  无
 * @param  无
 * @param  无
 * @retval 返回缓冲区数据数量
 */
uint16_t UART_read_queue_len(void)
{
  uint16_t rLen;

  rLen = QueueLength(&UART_rec_cir_queue);

  return rLen;
}

/**
 * @brief  fputc
 * @note   重定向函数
 * @param  无
 * @param  无
 * @param  无
 * @retval 无
 */
int fputc(int ch, FILE *f)
{
  tx_mutex_get(&Print_mutex_TCB, TX_WAIT_FOREVER);
  send_char_fputc((uint8_t)ch); // 发送到串口
  tx_mutex_put(&Print_mutex_TCB);

  return ch; // 返回ch
}
