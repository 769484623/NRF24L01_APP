/*===========================================================================
* 网址 ：http://yhmcu.taobao.com/   http://www.cdebyte.com/                 *
* 作者 ：李勇  原 亿和电子工作室  现 亿佰特电子科技有限公司                 * 
* 邮件 ：yihe_liyong@126.com                                                *
* 电话 ：18615799380                                                        *
============================================================================*/

#ifndef _nRF24L01_H_
#define _nRF24L01_H_
#include "nRF24L01_Reg.h"
#include "libfahw.h"
#include <stdint.h>
#define REPEAT_CNT      15  // 0-15, repeat transmit count
#define INIT_ADDR       1, 2, 3, 4, 5

// nRF24L01P相关控制引脚定义

#define L01_CE_LOW()    setGPIOValue(12, GPIO_LOW)
#define L01_CE_HIGH()   setGPIOValue(12, GPIO_HIGH)
#define L01_IRQ_READ() getGPIOValue(7)
typedef unsigned char uint8_t;
// nRF24L01P相关函数接口
// 初始化L01
void L01_Init(void);

void L01_ReadAddReg(uint8_t Addr);

// 复位TX FIFO指针      
void L01_FlushTX(void);

// 复位RX FIFO指针     
void L01_FlushRX(void);     

// 读取中断
uint8_t L01_ReadIRQSource(void);          

// 清除中断
#define IRQ_ALL  ((1<<RX_DR) | (1<<TX_DS) | (1<<MAX_RT))
void L01_ClearIRQ(uint8_t IRQ_Source); 
   
// 读取FIFO数据宽度
uint8_t L01_ReadTopFIFOWidth(void);

// 读取接收到的数据       
uint8_t L01_ReadRXPayload(uint8_t *pBuff);  

// 设置L01模式 
typedef enum{ TX_MODE, RX_MODE } L01MD; 
void L01_SetTRMode(L01MD mode);

// 设置L01空速                 
typedef enum{ SPD_250K, SPD_1M, SPD_2M } L01SPD;
void L01_SetSpeed(L01SPD speed);

// 设置L01功率                 
typedef enum{ P_F18DBM, P_F12DBM, P_F6DBM, P_0DBM } L01PWR;
void L01_SetPower(L01PWR power);

// 设置L01频率                
void L01_WriteHoppingPoint(uint8_t FreqPoint);    

uint8_t L01_ReadStatusReg(void);

// 写数据到一个寄存器
void L01_WriteSingleReg(uint8_t Addr, uint8_t Value);

// 读取一个寄存器的值   
uint8_t L01_ReadSingleReg(uint8_t Addr);

// 读取多个寄存器的值                 
void L01_ReadMultiReg(uint8_t StartAddr, uint8_t nBytes, uint8_t *pBuff);

// 写数据到多个寄存器
void L01_WriteMultiReg(uint8_t StartAddr, uint8_t *pBuff, uint8_t Length);

// 写数据到TXFIFO(带ACK返回)
void L01_WriteTXPayload_Ack(uint8_t *pBuff, uint8_t nBytes);

// 写数据到TXFIFO(不带ACK返回)
void L01_WriteTXPayload_NoAck(uint8_t *Data, uint8_t Data_Length);

// 设置发送物理地址
void L01_SetTXAddr(uint8_t *pAddr, uint8_t Addr_Length);

// 设置接收物理地址
void L01_SetRXAddr(uint8_t PipeNum, uint8_t *pAddr, uint8_t Addr_Length);

#endif//_nRF24L01_H_
