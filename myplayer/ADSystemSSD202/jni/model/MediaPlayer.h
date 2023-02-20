/*
 * MediaPlayer.h
 *
 *  Created on: 2021年2月1日
 *      Author: Administrator
 */

#ifndef JNI_MODEL_MEDIAPLAYER_H_
#define JNI_MODEL_MEDIAPLAYER_H_

#include "control/ZKVideoView.h"
#include "control/ZKTextView.h"

class MediaPlayer{
public:
	static MediaPlayer* getInstance();

	void initPlayer(ZKVideoView* mVideoview_videoPtr, ZKTextView* mTextViewPicPtr);
	static void setPlayerVol(int newVol);
	static void showPlayer(bool isSplitDisplay);
	static void quitPlayer();
	static void setMute();

private:
	MediaPlayer();
};


#define MEDIAPLAYER 	MediaPlayer::getInstance()
#endif /* JNI_MODEL_MEDIAPLAYER_H_ */
