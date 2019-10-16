/*
 *  UcamScanner.cpp
 *
 *  Created on: 2019-05-15
 *      Author: knox
 */

#include "ucam/UcamScanner.h"

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/types.h>
#include <linux/netlink.h>
#include <errno.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/types.h>
#include "common/MtConstant.h"
#include "common/MtLog.h"

#define LOG_TAG "UcamScanner"

#define UEVENT_BUFFER_SIZE 2048

static int init_hotplug_sock()  
{
    const int buffersize = 1024;
    int ret;

    struct sockaddr_nl snl;
    bzero(&snl, sizeof(struct sockaddr_nl));
    snl.nl_family = AF_NETLINK;
    snl.nl_pid = getpid();
    snl.nl_groups = 1;

    int s = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT);
    if (s == -1) {
        LOGE(LOG_TAG, "%s error (socket failed)", __func__);
        return -1;
    }
    setsockopt(s, SOL_SOCKET, SO_RCVBUF, &buffersize, sizeof(buffersize));

    ret = bind(s, (struct sockaddr *)&snl, sizeof(struct sockaddr_nl));
    if (ret < 0) {
        LOGE(LOG_TAG, "%s error (bind failed)", __func__);
        close(s);
        return -1;
    }

    return s;
}

void UcamScanner::setListener(ISourceListener<UcamDesc> *listener) {
    mListener = listener;
}

bool UcamScanner::start() {
    mFirstDetect = true;
    mHotplugSock = 0;
    return startThread();
}

void UcamScanner::stop() {
    stopThread();
    joinThread();
    if (mHotplugSock > 0) {
        LOGD(LOG_TAG, "close hotplug_sock %d", mHotplugSock);
        close(mHotplugSock);
        mHotplugSock = 0;
    }
    mFirstDetect = false;
}

void UcamScanner::threadLoop() {
    if (NULL == mListener) {
        sleep(1);
        LOGD(LOG_TAG, "no listener, no detect");
        return;
    }
    if (mFirstDetect) {
        ucamDetect();
        mFirstDetect = false;
        mHotplugSock = init_hotplug_sock();
        LOGD(LOG_TAG, "init hotplug sock %d", mHotplugSock);
    } else {
        ucamHotplug();
    }
}

const char *UcamScanner::getThreadName() {
    return TNAME_UCAM_HOT_PLUG;
}

void UcamScanner::ucamDetect() {
    system("rm ls_camera.txt \n");
    system("ls /dev/video* > ls_camera.txt \n");
    int fd;
    char buffer[80] = {0};
    fd = open("ls_camera.txt", O_RDONLY);
    int size = read(fd, buffer, sizeof(buffer));
    uint32_t buf_len = strlen(buffer);
    int i = 0;
    int line = 0;
    int pos = 0;
    for (i = 0; i < buf_len; i++) {
        if (buffer[i] == '\n') {
            if (pos == i) {
                pos = i + 1;
                continue;
            }
            line++;
            int url_len = i - pos;
            char url[url_len + 1];
            memset(url, 0, url_len + 1);
            memcpy(url, buffer + pos, url_len);
            UcamDesc desc;
            desc.url = url;
            mListener->addSource(desc);
            pos = i + 1;
        }
    }
    close(fd);
}

void UcamScanner::ucamHotplug() {
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(mHotplugSock, &fds);
    int maxfdp = mHotplugSock + 1;
    struct timeval timeout = {1, 0};
    switch(select(maxfdp, &fds, NULL, NULL, &timeout)) 
    {
        case -1:
        {
            break;
        }
        case 0:
        {
            // LOGD(LOG_TAG, "timeout");
            break;
        }
        default:
        {
            if(FD_ISSET(mHotplugSock, &fds)) {
                selectHotplug();
            }
        }       
    }
}

void UcamScanner::selectHotplug() {
    char buffer[UEVENT_BUFFER_SIZE * 2] = {0};
    if (recv(mHotplugSock, &buffer, sizeof(buffer), 0) < 0) {
        LOGE(LOG_TAG, "recv errno %s", strerror(errno));
        return;
    }
    long unsigned int buf_len = strlen(buffer);
    int i = 0;
    int pos = 0;
    for (i = buf_len - 1; i >=0; i--) {
        if (buffer[i] == '/') {
            pos = i;
            break;
        }
    }
    if (pos + 5 > buf_len) {
        return;
    }
    if (buffer[pos + 1] == 'v' &&
        buffer[pos + 2] == 'i' &&
        buffer[pos + 3] == 'd' &&
        buffer[pos + 4] == 'e' &&
        buffer[pos + 5] == 'o') {
        int url_len = 5 + buf_len - pos - 1;
        char url[url_len + 1];
        memset(url, 0, url_len + 1);
        memcpy(url, "/dev/", 5);
        memcpy(url + 5, buffer + pos + 1, url_len);

        UcamDesc desc;
        desc.url = url;

        if (buffer[0] == 'a') {
            mListener->addSource(desc);
        } else if (buffer[0] == 'r') {
            mListener->removeSource(desc);
        }
    }
}