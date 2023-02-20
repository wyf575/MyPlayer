/*
 * ProtocolSender.cpp
 *
 *  Created on: Sep 8, 2017
 *      Author: guoxs
 */

#include "uart/ProtocolSender.h"
#include "uart/UartContext.h"
#include "utils/Log.h"
#include <stdio.h>

extern BYTE getCheckSum(const BYTE *pData, int len);

/**
 * ��Ҫ����Э���ʽ����ƴ�ӣ�����ֻ�Ǹ�ģ��
 */
bool sendProtocol(const UINT16 cmdID, const BYTE *pData, BYTE len) {
	if (len + DATA_PACKAGE_MIN_LEN > 256) {
		LOGE("sendProtocol data is too len !!!\n");
		return false;
	}

	BYTE dataBuf[256];

	dataBuf[0] = CMD_HEAD1;
	dataBuf[1] = CMD_HEAD2;			// ͬ��֡ͷ

	dataBuf[2] = HIBYTE(cmdID);
	dataBuf[3] = LOBYTE(cmdID);		// �����ֽ�

	dataBuf[4] = len;

	UINT frameLen = 5;

	// ����
	for (int i = 0; i < len; ++i) {
		dataBuf[frameLen] = pData[i];
		frameLen++;
	}

#ifdef PRO_SUPPORT_CHECK_SUM
	// У����
	dataBuf[frameLen] = getCheckSum(dataBuf, frameLen);
	frameLen++;
#endif

	return UARTCONTEXT->send(dataBuf, frameLen);
}
