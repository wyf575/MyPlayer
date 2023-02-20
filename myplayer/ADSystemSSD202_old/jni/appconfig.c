/*
 * panelconfig.c
 *
 *  Created on: 2019年10月22日
 *      Author: koda.xu
 */

#include "appconfig.h"

#if USE_PANEL_1024_600
#include "SAT070CP50_1024x600.h"
#else
//#include "SAT070AT50_800x480.h"
#include "MV190E0M_1280x1024_MIPI.h"
#endif
