/*
 * WatchdogManager.cpp
 *
 *  Created on: 2021年4月10日
 *      Author: Administrator
 */

#include "WatchdogManager.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/watchdog.h>

#define WATCHDOG_IOCTL_BASE 'W'

#define WDIOC_SETTIMEOUT _IOWR(WATCHDOG_IOCTL_BASE, 6, int)
#define WDIOC_KEEPALIVE _IOR(WATCHDOG_IOCTL_BASE, 5, int)

int timeout = 20;

WatchdogManager::WatchdogManager(){
	wdt_fd = -1;
}

WatchdogManager* WatchdogManager::getInstance(){
	static WatchdogManager mInstance;
	return &mInstance;
}

bool WatchdogManager::openWatchdog(){
	wdt_fd = open("/dev/watchdog", O_WRONLY);
	if (wdt_fd == -1){
	    // fail to open watchdog device
		return false;
	}
	ioctl(wdt_fd, WDIOC_SETTIMEOUT, &timeout);
	return true;
}

void WatchdogManager::closeWatchdog(){
	int option = WDIOS_DISABLECARD;
	ioctl(wdt_fd, WDIOC_SETOPTIONS, &option);

	if (wdt_fd != -1){
	        close(wdt_fd);
	        wdt_fd = -1;
	}
}

void WatchdogManager::keep_alive(){
    int dummy;
    printf(".");
    ioctl(wdt_fd, WDIOC_KEEPALIVE, &dummy);
}
