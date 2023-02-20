/*
 * appconfig.h
 *
 *  Created on: 2019年10月22日
 *      Author: koda.xu
 */

#ifndef JNI_APPCONFIG_H_
#define JNI_APPCONFIG_H_

#define USE_PANEL_1024_600		0
#define USE_AMIC				1
#define ENABLE_BF				0
#define ENABLE_ROTATE			0
#define ENABLE_ROTATE_180		0

#define DSPOTTER_DATA_PATH			"/customer/res/DSpotter/data"

#define CREADER_SAVE_FILE			0
#define CREADER_LIB_PATH			"/customer/lib"
#define CREADER_DATA_PATH			"/customer/res/CReader/data"

#define NETWORKTYPE_NONE 			0
#define NETWORKTYPE_2G 				2
#define NETWORKTYPE_4G 				4
#define NETWORKTYPE_WIFI 			5
#define NETWORKTYPE_ETHERNET 		6

#define BA407SS_SERIAL			//BA407ss
//#define SQ800_SERIAL				//刀头
//#define SING7_SERIAL 				//大可马电机


#ifdef SING7_SERIAL
#define IS_OPEN_READ_THREAD
#endif

#define UI_DEF_LANDSCAPE 				0		//横屏
#define UI_PORTRAIT_DOUBLE_SCREEN 		1		//竖屏双屏
//配置UI类型
#define UI_TYPE  UI_DEF_LANDSCAPE

//版本号
#define APP_VERSION				"V1.0.6"
//是否是测试环境
#define IS_TEST_MODE     		0
//是否打开工厂测试
#define IS_FACTORY_MOD 			0
#endif /* JNI_APPCONFIG_H_ */
