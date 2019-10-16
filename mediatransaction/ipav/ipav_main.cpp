/*
 *  ipav_main.cpp
 *
 *  Created on: 2019-05-15
 *      Author: knox
 */

#include <stdio.h>
#include <string.h>
#include "common/MtLog.h"
#include "common/MtConstant.h"
#include "ipav/IpavServer.h"

#define LOG_TAG "ipav_main"

int main(int argc, char **argv) {

    setupLogFile(true, IPAV_LOG_CONF);

    LOGD(LOG_TAG, "ipav_main Go");

    IpavServer server;
    server.prepare();
    server.loop();

    MYLog_Shutdown();

    return 0;
}