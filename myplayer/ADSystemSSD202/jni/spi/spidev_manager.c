/*
 * spidev_manager.c
 *
 *  Created on: 2021年2月20日
 *      Author: Administrator
 */

/*
* 说明：SPI通讯实现
* 方式一： 同时发送与接收实现函数： SPI_Transfer()
* 方式二：发送与接收分开来实现
* SPI_Write() 只发送
* SPI_Read() 只接收
* 两种方式不同之处：方式一，在发的过程中也在接收，第二种方式，收与发单独进行
*/
#include "spidev_manager.h"

#define SPI_DEBUG 1

static const char *device = "/dev/spidev0.0";
static u8 mode = 0; /* SPI通信使用全双工，设置CPOL＝0，CPHA＝0。 */
static u8 bits = 8; /* ８ｂiｔｓ读写，MSB first。*/
static uint32_t speed = 3 * 1000 * 1000;/* 设置800K传输速度 */
static uint16_t delay = 0;
static int g_SPI_Fd = 0;


static void pabort(const char *s)
{
perror(s);
abort();
}


/**
* 功 能：同步数据传输
* 入口参数 ：
* TxBuf -> 发送数据首地址
* len -> 交换数据的长度
* 出口参数：
* RxBuf -> 接收数据缓冲区
* 返回值：0 成功
*/
int SPI_Transfer(const u8 *TxBuf, u8 *RxBuf, int len)
{
int ret;
int fd = g_SPI_Fd;


struct spi_ioc_transfer tr ={
.tx_buf = (unsigned long) TxBuf,
.rx_buf = (unsigned long) RxBuf,
.len =len,
.delay_usecs = delay,
};

ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
if (ret < 1)
	printf("can't send spi message");
else
{
#if SPI_DEBUG
int i;
printf("nsend spi message Succeed\n");
printf("nSPI Send [Len:%d]: \n", len);
for (i = 0; i < len; i++)
{
if (i % 8 == 0)
printf("nt");
printf("0x%02X ", TxBuf[i]);
}
printf("\n");


printf("SPI Receive [len:%d]:\n", len);
for (i = 0; i < len; i++)
{
if (i % 8 == 0)
printf("nt");
printf("0x%02X ", RxBuf[i]);
}
printf("\n");
#endif
}
return ret;
}


/**
* 功 能：发送数据
* 入口参数 ：
* TxBuf -> 发送数据首地址
* len -> 发送与长度
* 返回值：0 成功
*/
int SPI_Write(u8 *TxBuf, int len)
{
int ret;
int fd = g_SPI_Fd;


ret = write(fd, TxBuf, len);
if (ret < 0){
	printf("SPI Write error - %02X\n", TxBuf[0]);
}else
{
	#if SPI_DEBUG
	int i;
	printf("nSPI Write [Len:%d]: \n", len);
	for (i = 0; i < len; i++)
	{
		if (i % 8 == 0)
			printf("nt");
		printf("0x%02X", TxBuf[i]);
	}
	printf("\n");

	#endif
}

return ret;
}


/**
* 功 能：接收数据
* 出口参数：
* RxBuf -> 接收数据缓冲区
* rtn -> 接收到的长度
* 返回值：>=0 成功
*/
int SPI_Read(u8 *RxBuf, int len)
{
int ret;
int fd = g_SPI_Fd;
ret = read(fd, RxBuf, len);
if (ret < 0)
	printf("SPI Read error\n");
else
{
#if SPI_DEBUG
int i;
printf("SPI Read [len:%d]:\n", len);
for (i = 0; i < len; i++)
{
if (i % 8 == 0)
printf("nt");
printf("0x%02X ", RxBuf[i]);
}
printf("\n");
#endif
}


return ret;
}


/**
* 功 能：打开设备 并初始化设备
* 入口参数 ：
* 出口参数：
* 返回值：0 表示已打开 0XF1 表示SPI已打开 其它出错
*/
int SPI_Open(void)
{
int fd;
int ret = 0;


if (g_SPI_Fd != 0) /* 设备已打开 */
return 0xF1;


fd = open(device, O_RDWR);
if (fd < 0)
pabort("can't open SPI device\n");
else
printf("SPI - Open Succeed. Start Init SPI...%d\n", fd);


g_SPI_Fd = fd;
/*
* spi mode
*/

ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
if (ret == -1)
pabort("can't get spi mode\n");

ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
if (ret == -1)
pabort("can't set spi mode\n");


/*
* bits per word
*/
ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
if (ret == -1)
pabort("can't set bits per word\n");


ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
if (ret == -1)
pabort("can't get bits per word\n");


/*
* max speed hz
*/
ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
if (ret == -1)
pabort("can't set max speed hz\n");


ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
if (ret == -1)
pabort("can't get max speed hz\n");


printf("spi mode: %d\n", mode);
printf("bits per word: %d\n", bits);
printf("max speed: %d KHz (%d MHz)\n", speed / 1000, speed / 1000 / 1000);


return ret;
}


/**
* 功 能：关闭SPI模块
*/
int SPI_Close(void)
{
int fd = g_SPI_Fd;


if (fd == 0) /* SPI是否已经打开*/
return 0;
close(fd);
g_SPI_Fd = 0;


return 0;
}


