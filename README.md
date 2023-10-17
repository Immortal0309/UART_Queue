# UART_Queue
串口环形队列
使用方法：

#include "UARTx.h"

// 串口循环队列接收缓冲区
static uint8_t UART_buff[10];

int main(void)
{
   UART_init();
  while(1)
  {
    if (UART_read_queue_len() != 0)
    {
      UART_read(UART_buff, 3);
      
      if(strcmp((char *)UART_buff, "LD1") == 0)
      {
				printf("hello\r\n");
      }
    }
  }
  return 0;
}

-------------------------------------------
uint8_t arr[] = "hello";
int main(void)
{
   UART_init();
  while(1)
  {
    UART_write(arr, sizeof(arr));
   UART_tx_scan();
  }
   
  return 0;
}
