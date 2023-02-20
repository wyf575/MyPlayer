#pragma once
#include "uart/ProtocolSender.h"
/*
*此文件由GUI工具生成
*文件功能：用于处理用户的逻辑相应代码
*功能说明：
*========================onButtonClick_XXXX
当页面中的按键按下后系统会调用对应的函数，XXX代表GUI工具里面的[ID值]名称，
如Button1,当返回值为false的时候系统将不再处理这个按键，返回true的时候系统将会继续处理此按键。比如SYS_BACK.
*========================onSlideWindowItemClick_XXXX(int index) 
当页面中存在滑动窗口并且用户点击了滑动窗口的图标后系统会调用此函数,XXX代表GUI工具里面的[ID值]名称，
如slideWindow1;index 代表按下图标的偏移值
*========================onSeekBarChange_XXXX(int progress) 
当页面中存在滑动条并且用户改变了进度后系统会调用此函数,XXX代表GUI工具里面的[ID值]名称，
如SeekBar1;progress 代表当前的进度值
*========================ogetListItemCount_XXXX() 
当页面中存在滑动列表的时候，更新的时候系统会调用此接口获取列表的总数目,XXX代表GUI工具里面的[ID值]名称，
如List1;返回值为当前列表的总条数
*========================oobtainListItemData_XXXX(ZKListView::ZKListItem *pListItem, int index)
 当页面中存在滑动列表的时候，更新的时候系统会调用此接口获取列表当前条目下的内容信息,XXX代表GUI工具里面的[ID值]名称，
如List1;pListItem 是贴图中的单条目对象，index是列表总目的偏移量。具体见函数说明
*========================常用接口===============
*LOGD(...)  打印调试信息的接口
*mTextXXXPtr->setText("****") 在控件TextXXX上显示文字****
*mButton1Ptr->setSelected(true); 将控件mButton1设置为选中模式，图片会切换成选中图片，按钮文字会切换为选中后的颜色
*mSeekBarPtr->setProgress(12) 在控件mSeekBar上将进度调整到12
*mListView1Ptr->refreshListView() 让mListView1 重新刷新，当列表数据变化后调用
*mDashbroadView1Ptr->setTargetAngle(120) 在控件mDashbroadView1上指针显示角度调整到120度
*
* 在Eclipse编辑器中  使用 “alt + /”  快捷键可以打开智能提示
*/

#include <pthread.h>

#include "baselib/FileUtils.h"
#include "net/Module4GManager.h"
#include "net/DeviceSigManager.h"
#include "net/NetManager.h"
#include "tts.h"
#include "appconfig.h"
#include "mouse/mouse.h"
#include "model/JsonParse.h"
#include "storage/StoragePreferences.h"

#define PASS_COLOR  0x00FF00
#define FAIL_COLOR  0xFF0000

static bool sd_status = false;
static bool usb_status = false;
static bool eth_status = false;
static bool wifi_status = false;
static bool net4g_status = false;
static bool vol_status = false;
static bool exitQueryStatusThread = true;

static bool g_bStopped = true;
static bool g_bPaused = false;
static HANDLE g_hTts = NULL;
static bool g_bInit = false;
static bool g_bLoaded = false;

static int *g_pLangID = NULL;
static int g_nLangCnt = 0;
static int g_nSpeakerCnt = 0;
static int g_nLangIdx = 0;
static int g_nSpeakerIdx = 0;

extern Language g_language[LANG_SUPPORT_NUM];
extern Speaker g_speaker[SPEAKER_SUPPORT_NUM];

static void TTS_StopPlayCallback()
{
	if (!g_bStopped)
	{
		g_bStopped = true;
		g_bPaused = false;
	}

	printf("tts stop callback\n");
}


static void ttsInit(){
    char *pText = "音频测试中";

    if (!g_bLoaded)
    	return ;

    if (g_hTts)
    {
    	TTS_Release(g_hTts);
    	g_hTts = NULL;
    }

    if (g_pLangID && g_nLangCnt && g_nSpeakerCnt && strlen(pText))
    {
    	g_hTts = TTS_Initialize(CREADER_LIB_PATH, CREADER_DATA_PATH, g_pLangID[g_nLangIdx], g_speaker[g_nSpeakerIdx].speaker, TTS_StopPlayCallback);
		if (!g_hTts)
		{
			printf("fail to initialize tts\n");
			return;
		}

		//Read TTS string from file
		if(TTS_AddTTSStringUTF8(g_hTts, pText) != __PLAYER_SUCCESS__)
		{
			printf("Fail to read TTS string!\r\n");
			return;
		}

		g_bInit = true;
    }
}

void ttsStartPlayer(){
	if (!g_bInit)
		return ;

	if (g_bStopped)
	{
		g_bStopped = false;
		g_bPaused = false;
		TTS_Start(g_hTts, CREADER_SAVE_FILE);

		printf("click play btn: play\n");
	}
	else
	{
		g_bPaused = !g_bPaused;

		if (g_bPaused)
			TTS_Pause(g_hTts);
		else
			TTS_Resume(g_hTts);

		printf("click play btn: %s\n", g_bPaused?"pause":"resume");
	}
}

static void* loopQueryStatus(){
	while(!exitQueryStatusThread){
		//sd
		if(!sd_status){
			std::string sdPath = "/dev/mmcblk0p1";
			sd_status = FILEUTILS->isFileExists_stat(sdPath);
			printf("----------------------sd_status-------------%d\n", sd_status);
		}

		//usb
		if(!usb_status){
			std::string usb1 = "/sys/devices/soc0/soc/soc:Sstar-ehci-1/usb2/2-1/2-1.1";
			usb_status = FILEUTILS->isFileExists_stat(usb1);
			if(!usb_status){
				usb1 = "/sys/devices/soc0/soc/soc:Sstar-ehci-1/usb2/2-1/2-1.2";
				usb_status = FILEUTILS->isFileExists_stat(usb1);
			}
			if(!usb_status){
				usb1 = "/sys/devices/soc0/soc/soc:Sstar-ehci-1/usb2/2-1/2-1.3";
				usb_status = FILEUTILS->isFileExists_stat(usb1);
			}
			printf("----------------------usb_status-------------%d\n", usb_status);
		}

		if(!eth_status){
			eth_status = getNetConnectStatus("eth0");
			printf("----------------------eth_status-------------%d\n", eth_status);
		}
		if(!wifi_status){
//			wifi_status = getNetConnectStatus("wlan0");
			wifi_status = DEVICESIGMANAGER->wifiIsNormal();
			printf("----------------------wifi_status-------------%d\n", wifi_status);
		}

		if(!net4g_status){
			bool result = getNetConnectStatus("eth2");
			net4g_status = result && MODULE4GMANAGER->isConnectedNet();
			printf("----------------------net4g_status-------------%d\n", net4g_status);
		}

		if(sd_status && usb_status && eth_status && net4g_status){
			exitQueryStatusThread = true;
			if(vol_status){
				LOGD("------------putBool-----------true\n");
				system("touch ../data/testPass");
				system("sync");
			}
		}
		sleep(4);
	}
	return NULL;
}

void initTestStatus(){
	sd_status = false;
	usb_status = false;
	eth_status = false;
	wifi_status = false;
	net4g_status = false;
	vol_status = false;

	mtv_sdPtr->setTextColor(FAIL_COLOR);
	mtv_usbPtr->setTextColor(FAIL_COLOR);
	mtv_wifiPtr->setTextColor(FAIL_COLOR);
	mtv_ethPtr->setTextColor(FAIL_COLOR);
	mtv_4gPtr->setTextColor(FAIL_COLOR);
	mtv_volPtr->setTextColor(FAIL_COLOR);
	mButton2Ptr->setVisible(true);
	mButton1Ptr->setVisible(true);

	ttsStartPlayer();
	if(exitQueryStatusThread){
		exitQueryStatusThread = false;
		pthread_t tid_status_thrad;
		pthread_create(&tid_status_thrad, NULL, (void*)loopQueryStatus, NULL);
	}

}

/**
 * 注册定时器
 * 填充数组用于注册定时器
 * 注意：id不能重复
 */
static S_ACTIVITY_TIMEER REGISTER_ACTIVITY_TIMER_TAB[] = {
	//{0,  6000}, //定时器id=0, 时间间隔6秒
	{1,  2000},
};

/**
 * 当界面构造时触发
 */
static void onUI_init(){
    //Tips :添加 UI初始化的显示代码到这里,如:mText1Ptr->setText("123");
	initMouseDev();
}

/**
 * 当切换到该界面时触发
 */
static void onUI_intent(const Intent *intentPtr) {
    if (intentPtr != NULL) {
        //TODO
    }
}

/*
 * 当界面显示时触发
 */
static void onUI_show() {
	if (TTS_Preload())
	{
		printf("tts module load failed\n");
		g_bLoaded = 0;
		return ;
	}

	g_bLoaded = 1;

	// get text from m_pchText
	g_pLangID = (int*)malloc(sizeof(int) * TTS_GetLanguageMaxNum());
	if (!g_pLangID)
	{
		printf("alloc memory fail\n");
		return;
	}

	g_nLangCnt = TTS_GetAvailableLangID(CREADER_DATA_PATH, g_pLangID);
	if (g_nLangCnt <= 0)
	{
		printf("Get lang ID fail\n");
		return;
	}

	g_nSpeakerCnt = TTS_GetAvailableSpeaker(CREADER_LIB_PATH, CREADER_DATA_PATH, g_pLangID[g_nLangIdx]);
	if (g_nSpeakerCnt <= 0)
	{
		printf("Get speaker fail\n");
		return;
	}

	ttsInit();

	initTestStatus();
//	ttsStartPlayer();
}

/*
 * 当界面隐藏时触发
 */
static void onUI_hide() {
	exitQueryStatusThread = true;
}

/*
 * 当界面完全退出时触发
 */
static void onUI_quit() {

}

/**
 * 串口数据回调接口
 */
static void onProtocolDataUpdate(const SProtocolData &data) {

}

/**
 * 定时器触发函数
 * 不建议在此函数中写耗时操作，否则将影响UI刷新
 * 参数： id
 *         当前所触发定时器的id，与注册时的id相同
 * 返回值: true
 *             继续运行当前定时器
 *         false
 *             停止运行当前定时器
 */
static bool onUI_Timer(int id){
	switch (id) {
	case 1:
		if(sd_status){
			mtv_sdPtr->setTextColor(PASS_COLOR);
		}
		if(usb_status){
			mtv_usbPtr->setTextColor(PASS_COLOR);
		}
		if(eth_status){
			mtv_ethPtr->setTextColor(PASS_COLOR);
		}
		if(wifi_status){
			mtv_wifiPtr->setTextColor(PASS_COLOR);
		}
		if(net4g_status){
			mtv_4gPtr->setTextColor(PASS_COLOR);
		}
		break;
		default:
			break;
	}
    return true;
}

/**
 * 有新的触摸事件时触发
 * 参数：ev
 *         新的触摸事件
 * 返回值：true
 *            表示该触摸事件在此被拦截，系统不再将此触摸事件传递到控件上
 *         false
 *            触摸事件将继续传递到控件上
 */
static bool onfactoryTestActivityTouchEvent(const MotionEvent &ev) {
    switch (ev.mActionStatus) {
		case MotionEvent::E_ACTION_DOWN://触摸按下
			LOGD("时刻 = %ld 坐标  x = %d, y = %d", ev.mEventTime, ev.mX, ev.mY);
			break;
		case MotionEvent::E_ACTION_MOVE://触摸滑动
			setMousePos(ev.mX, ev.mY);
			break;
		case MotionEvent::E_ACTION_UP:  //触摸抬起
			if(ev.mY > 185 && ev.mY < 205){
				if(ev.mX < 515){
				    mtv_volPtr->setTextColor(PASS_COLOR);
				    mButton1Ptr->setVisible(false);
				    mButton2Ptr->setVisible(false);
				    vol_status = true;
				    if(exitQueryStatusThread){
				    	system("touch ../data/testPass");
				    }
				}else{
				    mButton1Ptr->setVisible(false);
				    mButton2Ptr->setVisible(false);
				    vol_status = false;
				}
			}
			break;
		default:
			break;
	}
	return false;
}

static bool onButtonClick_Button1(ZKButton *pButton) {
    LOGD(" ButtonClick Button1 !!!\n");
    return false;
}

static bool onButtonClick_Button2(ZKButton *pButton) {
    LOGD(" ButtonClick Button2 !!!\n");
    return false;
}

static bool onButtonClick_Button3(ZKButton *pButton) {
    LOGD(" ButtonClick Button3 !!!\n");
    initTestStatus();
    return false;
}
