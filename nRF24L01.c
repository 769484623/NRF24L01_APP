/*===========================================================================
* 网址 ：http://yhmcu.taobao.com/   http://www.cdebyte.com/                 *
* 作者 ：李勇  原 亿和电子工作室  现 亿佰特电子科技有限公司                 * 
* 邮件 ：yihe_liyong@126.com                                                *
* 电话 ：18615799380                                                        *
============================================================================*/
#include "nRF24L01.h"
#include <unistd.h>
#include <string.h>
#include <stdio.h>
extern int fd;
unsigned char Rxbuff[33] ={0},Txbuff[33] = {0};

extern void transfer(int fd,uint8_t tx[],uint8_t rx[],uint16_t lenth);

/*===========================================================================
* 函数 ：L01_ReadSingleReg() => 读取一个寄存器的值                          * 
* 输入 ：Addr，读取的寄存器地址                                             * 
* 输出 ：读出的值                                                           * 
============================================================================*/
uint8_t L01_ReadSingleReg(uint8_t Addr)
{
	Txbuff[0] = R_REGISTER | Addr;
	Txbuff[1] = 0xFF;
	transfer(fd,Txbuff,Rxbuff,2);
	return Rxbuff[1];
}

/*===========================================================================
* 函数 ：L01_WriteSingleReg() => 写数据到一个寄存器                         * 
* 输入 ：Addr，写入寄存器的地址，Value，待写入的值                          * 
============================================================================*/
void L01_WriteSingleReg(uint8_t Addr, uint8_t Value)
{
	Txbuff[0] = W_REGISTER | Addr;
	Txbuff[1] = Value;
	transfer(fd,Txbuff,Rxbuff,2);
}

/*===========================================================================
* 函数 ：L01_WriteMultiReg() => 写数据到多个寄存器                          * 
* 输入 ：StartAddr,写入寄存器的首地址，pBuff,指向待写入的值，Length,长度    * 
============================================================================*/
void L01_WriteMultiReg(uint8_t StartAddr, uint8_t *pBuff, uint8_t Length)
{
	Txbuff[0] = W_REGISTER | StartAddr;
	memcpy(Txbuff+1,pBuff,Length);
	transfer(fd,Txbuff,Rxbuff,Length+1);
}

/*===========================================================================
* 函数 ：L01_FlushTX() => 复位TX FIFO指针                                   * 
============================================================================*/
void L01_FlushTX(void)
{
	Txbuff[0] = FLUSH_TX;
	transfer(fd,Txbuff,Rxbuff,1);
}

/*===========================================================================
* 函数 ：L01_FlushRX() => 复位RX FIFO指针                                   *
============================================================================*/
void L01_FlushRX(void)
{
	Txbuff[0] = FLUSH_RX;
	transfer(fd,Txbuff,Rxbuff,1);
}

uint8_t L01_ReadStatusReg(void)
{
	Txbuff[0] = R_REGISTER + L01REG_STATUS;
	Txbuff[1] = 0xFF;
	transfer(fd,Txbuff,Rxbuff,2);
	return Rxbuff[1];
}

/*===========================================================================
* 函数 ：L01_ClearIRQ() => 清除中断                                         * 
* 输入 ：IRQ_Source，需要清除的中断源                                       * 
============================================================================*/
void L01_ClearIRQ(uint8_t IRQ_Source)
{
    uint8_t btmp = 0;
    IRQ_Source &= (1<<RX_DR) | (1<<TX_DS) | (1<<MAX_RT);
    btmp = L01_ReadStatusReg();
    Txbuff[0] = W_REGISTER + L01REG_STATUS;
    Txbuff[1] = IRQ_Source | btmp;
    transfer(fd,Txbuff,Rxbuff,2);
    L01_ReadStatusReg();
}

/*===========================================================================
* 函数 ：L01_ReadIRQSource() => 读取中断                                    *         
* 输出 ：读出的中断源                                                       * 
============================================================================*/
uint8_t L01_ReadIRQSource(void)
{
    return (L01_ReadStatusReg() & ((1<<RX_DR)|(1<<TX_DS)|(1<<MAX_RT)));
}

/*===========================================================================
* 函数 ：L01_ReadTopFIFOWidth() => 读取FIFO数据宽度                         * 
============================================================================*/
uint8_t L01_ReadTopFIFOWidth(void)
{
	Txbuff[0] = R_RX_PL_WID;
	Txbuff[1] = 0xFF;
	transfer(fd,Txbuff,Rxbuff,2);
    	return Rxbuff[1];
}

/*===========================================================================
* 函数 ：L01_ReadRXPayload() => 读取接收到的数据                            * 
* 输入 ：pBuff，指向收到的数据                                              * 
* 输出 ：数据长度                                                           * 
============================================================================*/
uint8_t L01_ReadRXPayload(uint8_t *pBuff)
{
	uint8_t width, PipeNum;
	PipeNum = (L01_ReadSingleReg(L01REG_STATUS)>>1) & 0x07;
	width = L01_ReadTopFIFOWidth();
	Txbuff[0] = R_RX_PAYLOAD;
	memset(Txbuff+1,0xFF,width);
	transfer(fd,Txbuff,Rxbuff,width+1);
	memcpy(pBuff,Rxbuff,width);
    	L01_FlushRX();
    	return (width);
}

/*===========================================================================
* 函数 ：L01_WriteTXPayload_Ack() => 写数据到TXFIFO(带ACK返回)              * 
* 输入 ：pBuff，指向待写入的数据，nBytes，写入数据的长度                    * 
============================================================================*/
void L01_WriteTXPayload_Ack(uint8_t *pBuff, uint8_t nBytes)
{
    uint8_t length = (nBytes>32) ? 32 : nBytes;
    L01_FlushTX();
    Txbuff[0] = W_TX_PAYLOAD;
    memcpy(Txbuff+1,pBuff,length);
    transfer(fd,Txbuff,Rxbuff,length+1);
}

/*===========================================================================
* 函数 ：L01_WriteTXPayload_Ack() => 写数据到TXFIFO(不带ACK返回)            * 
* 输入 ：Data，指向待写入的数据，Data_Length，写入数据的长度                * 
============================================================================*/
void L01_WriteTXPayload_NoAck(uint8_t *Data, uint8_t Data_Length)
{
    if ((Data_Length>32) || (Data_Length==0))   { return; }
        
    Txbuff[0] = W_TX_PAYLOAD_NOACK;
    memcpy(Txbuff+1,Data,Data_Length);
    transfer(fd,Txbuff,Rxbuff,Data_Length+1);
}

/*===========================================================================
* 函数 ：L01_SetTXAddr() => 设置发送物理地址                                * 
* 输入 ：pAddr指向需要设置的地址数据，Addr_Length，地址长度                 * 
============================================================================*/
void L01_SetTXAddr(uint8_t *pAddr, uint8_t Addr_Length)
{
    uint8_t Length = (Addr_Length>5) ? 5 : Addr_Length;
    L01_WriteMultiReg(L01REG_TX_ADDR, pAddr, Length);
}

/*===========================================================================
* 函数 ：L01_SetRXAddr() => 设置接收物理地址                                * 
* 输入 ：PipeNum，管道号，pAddr指向需要设置地址数据，Addr_Length，地址长度  * 
============================================================================*/
void L01_SetRXAddr(uint8_t PipeNum, uint8_t *pAddr, uint8_t Addr_Length)
{
    uint8_t Length = (Addr_Length>5) ? 5 : Addr_Length;
    uint8_t pipe = (PipeNum>5) ? 5 : PipeNum;

    L01_WriteMultiReg(L01REG_RX_ADDR_P0 + pipe, pAddr, Length);
}

/*===========================================================================
* 函数 ：L01_SetSpeed() => 设置L01空速                                      * 
* 输入 ：speed，=SPD_250K(250K), =SPD_1M(1M), =SPD_2M(2M)                   * 
============================================================================*/
void L01_SetSpeed(L01SPD speed)
{
	uint8_t btmp = L01_ReadSingleReg(L01REG_RF_SETUP);

	btmp &= ~((1<<5) | (1<<3));
	
	switch (speed)
	{
	    case SPD_250K:  btmp |= (1<<5);             break;  // 250K
        case SPD_1M:    btmp &= ~((1<<5) | (1<<3)); break;  // 1M
        case SPD_2M:    btmp |= (1<<3);             break;  // 2M
        default:        break;                                     
	        
	}
	L01_WriteSingleReg(L01REG_RF_SETUP, btmp);
}

/*===========================================================================
* 函数 ：L01_SetPower() => 设置L01功率                                      * 
* 输入 ：power, =P_F18DBM(18DB),=P_F12DBM(12DB),=P_F6DBM(6DB),=P_0DBM(0DB)  *
============================================================================*/
void L01_SetPower(L01PWR power)
{
    uint8_t btmp = L01_ReadSingleReg(L01REG_RF_SETUP) & ~0x07;
    
    switch(power)
    {
        case P_F18DBM:  btmp |= PWR_18DB; break;    // 18DBM
        case P_F12DBM:  btmp |= PWR_12DB; break;    // 12DBM
        case P_F6DBM:   btmp |= PWR_6DB;  break;    // 6DBM
        case P_0DBM:    btmp |= PWR_0DB;  break;    // 0DBM
        default:        break;
    }
    L01_WriteSingleReg(L01REG_RF_SETUP, btmp);
}

/*===========================================================================
* 函数 ：L01_WriteHoppingPoint() => 设置L01频率                             * 
* 输入 ：FreqPoint，待设置的频率                                            * 
============================================================================*/
void L01_WriteHoppingPoint(uint8_t FreqPoint)
{
    L01_WriteSingleReg(L01REG_RF_CH, FreqPoint & 0x7F);
}

/*===========================================================================
* 函数 ：L01_SetTRMode() => 设置L01模式                                     * 
* 输入 ：mode，=TX_MODE, TX mode; =RX_MODE, RX mode                         * 
============================================================================*/
void L01_SetTRMode(L01MD mode)
{
    uint8_t controlreg = L01_ReadSingleReg(L01REG_CONFIG);
    if      (mode == TX_MODE)       { controlreg &= ~(1<<PRIM_RX); }
    else if (mode == RX_MODE)       { controlreg |= (1<<PRIM_RX); }

    L01_WriteSingleReg(L01REG_CONFIG, controlreg);
}

/*===========================================================================
* 函数 ：L01_Init() => 初始化L01                                             * 
============================================================================*/
void L01_Init(void)
{
    uint8_t addr[5] = { INIT_ADDR };

    L01_CE_LOW();
    usleep(200000);
    L01_ClearIRQ(IRQ_ALL);
    // 使能管道0动态包长度
    L01_WriteSingleReg(L01REG_DYNPD, (1<<0));
    L01_WriteSingleReg(L01REG_FEATRUE, 0x06);
    L01_ReadSingleReg(L01REG_DYNPD);
    L01_ReadSingleReg(L01REG_FEATRUE);
    L01_WriteSingleReg(L01REG_CONFIG, (1<<EN_CRC)|(1<<PWR_UP));
    L01_WriteSingleReg(L01REG_EN_AA, (1<<ENAA_P0));     // 自动应答（管道0）
    L01_WriteSingleReg(L01REG_EN_RXADDR, (1<<ERX_P0));  // 使能接收（管道0）
    L01_WriteSingleReg(L01REG_SETUP_AW, AW_5BYTES);     // 地址宽度 5byte
    L01_WriteSingleReg(L01REG_RETR, ARD_4000US|(REPEAT_CNT&0x0F));
    L01_WriteSingleReg(L01REG_RF_CH, 60);               // 初始化频率
    L01_WriteSingleReg(L01REG_RF_SETUP, 0x26); 
    L01_SetTXAddr(&addr[0], 5);                         // 设置地址（发送）
    L01_SetRXAddr(0, &addr[0], 5);                      // 设置地址（接收）
    
    L01_WriteHoppingPoint(77);                          // 设置频率
    L01_SetSpeed(SPD_250K);                             // 设置空速为250K
}

/*===========================================================================
-----------------------------------文件结束----------------------------------
===========================================================================*/
