#ifndef _BSP
#define _BSP
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include "nRF24L01.h"
void SystemInitial(void);
void transfer(int fd,uint8_t tx[],uint8_t rx[],uint16_t lenth);
#endif
