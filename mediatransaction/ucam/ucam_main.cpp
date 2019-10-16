/*
 *  ucam_main.cpp
 *
 *  Created on: 2019-05-15
 *      Author: knox
 */

#include <stdio.h>
#include <string.h>
#include "common/MtLog.h"
#include "common/MtConstant.h"
#include "ucam/UcamServer.h"

#define LOG_TAG "ucam_main"

int main(int argc, char **argv) {

    setupLogFile(true, UCAM_LOG_CONF);

    LOGD(LOG_TAG, "ucam_main Go");

    UcamServer server;
    server.prepare();
    server.loop();

    MYLog_Shutdown();

    return 0;
}
