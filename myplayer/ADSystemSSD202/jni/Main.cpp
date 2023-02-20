#include "entry/EasyUIContext.h"
#include "uart/UartContext.h"
#include "manager/ConfigManager.h"
#include "appconfig.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

void onEasyUIInit(EasyUIContext *pContext) {
	// 初始化时打开串口
	printf("get uart name %s\n", CONFIGMANAGER->getUartName().c_str());
#ifdef SING7_SERIAL
	UARTCONTEXT->openUart("/dev/ttyS2", 0000015);//9600
#else
	UARTCONTEXT->openUart(CONFIGMANAGER->getUartName().c_str(), CONFIGMANAGER->getUartBaudRate());
//	UARTCONTEXT->openUart("/dev/ttyS1", 0010002);//115200
#endif


}

void onEasyUIDeinit(EasyUIContext *pContext) {
	UARTCONTEXT->closeUart();
}

const char* onStartupApp(EasyUIContext *pContext) {
//	return "doubleScreenActivity";
	return "mainActivity";
//	return "windowActivity";
//	return "playerActivity";
//	return "player1280x1024Logic";
}


#ifdef __cplusplus
}
#endif  /* __cplusplus */

