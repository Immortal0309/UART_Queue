#ifndef _UART_X_H_
#define _UART_X_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "Queue.h"
#include "app_threadx.h"
#include "main.h"
#include "usart.h"
#include <stdio.h>

#define UART_ID  USART3
#define UART_HANDLE huart3

#define UART_BUF_SIZE 64                        // 设置缓冲区的大小

void UART_init(void);                           // 初始化UART模块
void UART_tx_scan(void);                        // UART发送轮询函数
uint8_t UART_write(uint8_t *pBuf, uint8_t len); // 写串口，返回已写入数据的个数
uint8_t UART_read(uint8_t *pBuf, uint8_t len);  // 读串口，返回读到数据的个数
uint16_t UART_read_queue_len(void);             // 获取串口接收队列的元素数量

#ifdef __cplusplus
}
#endif

#endif
