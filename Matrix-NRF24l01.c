#include "bsp.h"
#include <linux/videodev2.h>
unsigned char Rx[33] = {0};
void Switch_To_Recv()
{
	L01_CE_LOW();               // CE = 0, 关闭发送
    L01_FlushRX();              // 复位接收FIFO指针
    L01_FlushTX();              // 复位发送FIFO指针    
    L01_ClearIRQ(IRQ_ALL);      // 清除中断 
     L01_SetTRMode(RX_MODE);     // 接收模式    
     L01_CE_HIGH();              // 启动接收
}
void RF_Handler(void)
{
	uint8_t length = 0;
	if (0 == L01_IRQ_READ())                    // 检测无线模块是否产生接收中断 
    {
        if (L01_ReadIRQSource() & (1<<RX_DR))   // 检测无线模块是否接收到数据
        {
            // 读取接收到的数据长度和数据内容
            length = L01_ReadRXPayload(Rx);
            // 判断接收数据是否正确
            if (length > 0)
            {
				printf("%c\n",*Rx);
				printf("%c\n",*(Rx+1));
				printf("%c\n",*(Rx+2));
            }                
        }    
        L01_FlushRX();                          // 复位接收FIFO指针    
        L01_ClearIRQ(IRQ_ALL);                  // 清除中断            
    }
}
void RF_SendPacket(void)
{
    uint8_t length=3, buffer[] = "dda";
    L01_CE_LOW();               // CE = 0, 关闭发送    
    L01_SetTRMode(TX_MODE);     // 设置为发送模式      	
    L01_WriteTXPayload_NoAck(buffer, length);  
    L01_CE_HIGH();              // CE = 1, 启动发射 
    while (0 != L01_IRQ_READ());
    while (0 == L01_ReadIRQSource());
}
int main(int argc, char *argv[])
{
	SystemInitial();
	Switch_To_Recv();
	while(1)
	{
		RF_Handler();
		//RF_SendPacket();
		usleep(200);
	}
	return 0;
}
