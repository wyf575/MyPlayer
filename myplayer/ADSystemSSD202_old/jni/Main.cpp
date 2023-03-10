#include "entry/EasyUIContext.h"
#include "uart/UartContext.h"
#include "manager/ConfigManager.h"
#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

void onEasyUIInit(EasyUIContext *pContext) {
	// 初始化时打开串口
	printf("get uart name %s\n", CONFIGMANAGER->getUartName().c_str());
	UARTCONTEXT->openUart(CONFIGMANAGER->getUartName().c_str(), CONFIGMANAGER->getUartBaudRate());
//	UARTCONTEXT->openUart("/dev/ttyS1", 0010002);//115200
//	UARTCONTEXT->openUart("/dev/ttyS1", 0000015);//9600
}

void onEasyUIDeinit(EasyUIContext *pContext) {
	UARTCONTEXT->closeUart();
}

const char* onStartupApp(EasyUIContext *pContext) {
//	return "doubleScreenActivity";
	return "mainActivity";
//	return "playerActivity";
//	return "player1280x1024Logic";
}


#ifdef __cplusplus
}
#endif  /* __cplusplus */

