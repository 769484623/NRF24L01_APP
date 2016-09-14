#include "bsp.h"
static const char *device = "/dev/spidev0.0";
static unsigned char mode = 0;
static uint8_t bits = 8;
static uint32_t speed = 10*1000*1000;
static uint16_t delay = 0;
int fd = 0;
static void EXIT()
{
	unexportGPIOPin(12);
	close(fd);
	abort();
}
static void pabort(const char *s)
{
	perror(s);
	EXIT();
}
static int SPI_Init()
{
	int fd = open(device, O_RDWR);
	int ret = 0;
	if (fd < 0)
		pabort("can't open device");
	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't set bits per word");
	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't set max speed hz");
	ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
	if (ret == -1)
		pabort("can't set spi mode");
	return fd;
}
void RF_Initial(void)
{
	L01_Init();             // 初始化L01寄存器
	L01_SetTRMode(RX_MODE); // 接收模式      
	L01_FlushRX();          // 复位接收FIFO指针    
  	L01_FlushTX();          // 复位发送FIFO指针
  	L01_ClearIRQ(IRQ_ALL);  // 清除所有中断
  	L01_CE_HIGH();          // CE = 1, 启动接收  
	//SetL01IRQ();	
}
void SystemInitial()
{
	fd = SPI_Init();
	RF_Initial();
	exportGPIOPin(12);
	setGPIODirection(12, GPIO_OUT);
}
void transfer(int fd,uint8_t tx[],uint8_t rx[],uint16_t lenth)
{
	int ret;
	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long)tx,
		.rx_buf = (unsigned long)rx,
		.len = lenth,
		.delay_usecs = delay,
		.speed_hz = speed,
		.bits_per_word = bits,
	};
	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
	if (ret < 1)
		pabort("can't send spi message");
}
