#ifndef _QUEUE_H_
#define _QUEUE_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "main.h"

  // 定义数据类型，即队列中元素的数据类型
  typedef uint8_t DATA_TYPE;

  // 定义循环队列结构体
  typedef struct
  {
    uint16_t front;     // 头指针，队非空时指向队头元素
    uint16_t rear;      // 尾指针，队非空时指向队尾元素的下一个位置
    uint16_t bufLen;    // 队列的总容量
    uint16_t elemNum;   // 当前队列中的元素的数量
    DATA_TYPE *pBuffer; // 循环队列的缓冲区
  } StructCirQue;

  void InitQueue(StructCirQue *pQue, DATA_TYPE *pBuf, uint16_t len);      // 初始化队列
  void ClearQueue(StructCirQue *pQue);                                    // 清队列
  uint8_t QueueEmpty(StructCirQue *pQue);                                 // 判断队列是否为空，1为空，0为非空
  uint16_t QueueLength(StructCirQue *pQue);                               // 返回队列中元素个数，即为队列的长度
  uint16_t EnQueue(StructCirQue *pQue, DATA_TYPE *pInput, uint16_t len);  // 入队len个元素
  uint16_t DeQueue(StructCirQue *pQue, DATA_TYPE *pOutput, uint16_t len); // 出队len个元素

#ifdef __cplusplus
}
#endif

#endif
