/*
 * UartContext.cpp
 *
 *  Created on: Sep 5, 2017
 *      Author: guoxs
 */
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <memory.h>
#include <termio.h>
#include <sys/ioctl.h>

#include "uart/UartContext.h"
#include "utils/Log.h"
#include "appconfig.h"
#include "uart/ba407ss/BA407SSParser.h"
#include "baselib/Utils.h"

#define UART_DATA_BUF_LEN			16384	// 16KB

extern int parseProtocol(const BYTE *pData, UINT len);

static const char* getBaudRate(UINT baudRate) {
	struct {
		UINT baud;
		const char *pBaudStr;
	} baudInfoTab[] = {
		{ B1200, "B1200" },
		{ B2400, "B2400" },
		{ B4800, "B4800" },
		{ B9600, "B9600" },
		{ B19200, "B19200" },
		{ B38400, "B38400" },
		{ B57600, "B57600" },
		{ B115200, "B115200" },
		{ B230400, "B230400" },
		{ B460800, "B460800" },
		{ B921600, "B921600" }
	};

	int len = sizeof(baudInfoTab) / sizeof(baudInfoTab[0]);
	for (int i = 0; i < len; ++i) {
		if (baudInfoTab[i].baud == baudRate) {
			return baudInfoTab[i].pBaudStr;
		}
	}

	return NULL;
}

UartContext::UartContext() :
	mIsOpen(false),
	mUartID(0),
	mDataBufPtr(NULL),
	mDataBufLen(0) {

}

UartContext::~UartContext() {
	delete[] mDataBufPtr;
	closeUart();
}

bool UartContext::openUart(const char *pFileName, UINT baudRate) {
	LOGD("openUart pFileName = %s, baudRate = %s\n", pFileName, getBaudRate(baudRate));
	mUartID = open(pFileName, O_RDWR|O_NOCTTY);

	if (mUartID <= 0) {
		mIsOpen = false;
	} else {
		struct termios oldtio = { 0 };
		struct termios newtio = { 0 };
		tcgetattr(mUartID, &oldtio);

		newtio.c_cflag = baudRate|CS8|CLOCAL|CREAD;
		newtio.c_iflag = 0;	// IGNPAR | ICRNL
		newtio.c_oflag = 0;
		newtio.c_lflag = 0;	// ICANON
		newtio.c_cc[VTIME] = 0; /* inter-character timer unused */
		newtio.c_cc[VMIN] = 1; /* blocking read until 1 character arrives */
		tcflush(mUartID, TCIOFLUSH);
		tcsetattr(mUartID, TCSANOW, &newtio);

		// 设置为非阻塞
		fcntl(mUartID, F_SETFL, O_NONBLOCK);

		mIsOpen = run("uart");
		if (!mIsOpen) {
			close(mUartID);
			mUartID = 0;
		}

		LOGD("openUart mIsOpen = %d\n", mIsOpen);
	}

	return mIsOpen;
}

void UartContext::closeUart() {
	LOGD("closeUart mIsOpen: %d...\n", mIsOpen);
	if (mIsOpen) {
		requestExit();

		close(mUartID);
		mUartID = 0;
		mIsOpen = false;
	}
}

bool UartContext::send(const BYTE *pData, UINT len) {
	if (!mIsOpen) {
		return false;
	}
	int t = len * (10 * 1000.0 / 9600) * 1000;
	UTILS->printfCMD(pData, len);
	LOGD("\nsend: mUartID=%d   len=%d  t=%d\n", mUartID, len, t);
	clock_t startTime = clock();
	system("echo 1 > /sys/class/gpio/gpio17/value");
	if (write(mUartID, pData, len) != (int) len) {	// fail
		LOGD("send Fail\n");
		system("echo 0 > /sys/class/gpio/gpio17/value");
		return false;
	}
	double tm = (double)(clock() - startTime) / CLOCKS_PER_SEC;
	int sleepTime = t - tm * 1000 * 1000;
	LOGD("\nsend: sleepTime=%d tm=%f\n", sleepTime, tm);
	usleep(sleepTime - 3000);
	system("echo 0 > /sys/class/gpio/gpio17/value");
	// success
	LOGD("send Success\n");

	return true;
}

UartContext* UartContext::getInstance() {
	static UartContext sUC;
	return &sUC;
}

bool UartContext::readyToRun() {
	if (mDataBufPtr == NULL) {
		mDataBufPtr = new BYTE[UART_DATA_BUF_LEN];
	}

	if (mDataBufPtr == NULL) {
		closeUart();
	}

	return (mDataBufPtr != NULL);
}

int UartContext::getUartDate(BYTE cmdNum, unsigned char* buffer, int defLen){
	if(!mIsOpen){
		return 0;
	}
	// 增加一个`legacy`变量，表示buffer中遗留的数据长度
	int legacy = 0;
	int len = 0;
	for (int checkIndex = 0;checkIndex < 40; ++checkIndex) {
		//根据legacy的大小，调整缓冲区的起始指针及大小，防止数据覆盖
		int ret = read(mUartID, buffer + legacy, defLen - legacy);
		if (ret > 0) {
			LOGD("\n read: ret = %d,buffer[-1] = %02X, buffer[1] = %02X, cmdNum=%02X, cmd=", ret, buffer[ret + legacy - 1], buffer[1], cmdNum);
			UTILS->printfCMD(buffer, ret + legacy);
			if ((ret + legacy) >= 8
					&& (buffer[0] == 0xF0)
					&& buffer[ret + legacy - 1] == 0xF1
					&& buffer[1] == cmdNum) {
				LOGD("正确读到一帧数据\n");
				//清空legacy
				len = ret + legacy;
				legacy = 0;
				break;
			} else if (ret < 10) {
				len = ret + legacy;
				legacy += ret;
				LOGD("协议头正确，但是帧长度不够，则暂存在buffer里\n");
			}
		} else {
			//没收到数据时，休眠50ms，防止过度消耗cpu
			usleep(1000 * 50);
//			Thread::sleep(10);
		}
	}
	return len;
}

bool UartContext::threadLoop() {

	if (mIsOpen) {
#ifdef BA407SS_SERIAL
	BA407SSPARSER->doTaskforThread();
	Thread::sleep(2000);
#else
		// 可能上一次解析后有残留数据，需要拼接起来
		int readNum = read(mUartID, mDataBufPtr + mDataBufLen, UART_DATA_BUF_LEN - mDataBufLen);
//		printf("threadLoop - %d\n", readNum);
		if (readNum > 0) {
			mDataBufLen += readNum;

			// 解析协议
			int len = parseProtocol(mDataBufPtr, mDataBufLen);
			if ((len > 0) && (len < mDataBufLen)) {
				// 将未解析的数据移到头部
				memcpy(mDataBufPtr, mDataBufPtr + len, mDataBufLen - len);
			}

			mDataBufLen -= len;
		} else {
//			BYTE* data = "test";
//			send(data, sizeof(data));
			Thread::sleep(5000);
		}

#endif
		return true;
	}
	return false;
}
