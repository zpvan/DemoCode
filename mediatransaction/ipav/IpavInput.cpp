/*
 *  IpavInput.cpp
 *
 *  Created on: 2019-05-15
 *      Author: knox
 */

#include "ipav/IpavInput.h"

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/netlink.h>
#include <sys/socket.h>
#include <errno.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/types.h>
#include <errno.h>
#include <net/if.h>
#include <ifaddrs.h>
#include "common/MtConstant.h"
#include "common/MtLog.h"

#define LOG_TAG "IpavInput"

int getLocalIp(char *ip) {

    const char *ethInf = "eth0";

    int sd;
    struct sockaddr_in sin;
    struct ifreq ifr;

    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (-1 == sd)
    {
        LOGE(LOG_TAG, "socket error: %s", strerror(errno));
        return -1;
    }

    strncpy(ifr.ifr_name, ethInf, IFNAMSIZ);
    ifr.ifr_name[IFNAMSIZ - 1] = 0;

    // if error: No such device
    if (ioctl(sd, SIOCGIFADDR, &ifr) < 0)
    {
        // LOGE(LOG_TAG, "ioctl error: %s", strerror(errno));
        close(sd);
        return -1;
    }

    memcpy(&sin, &ifr.ifr_addr, sizeof(sin));
    snprintf(ip, IP_ADDR_MAX_LEN, "%s", inet_ntoa(sin.sin_addr));

    close(sd);
    return 0;
}

bool GetLocalIP(const char* i_pInterface, uint32_t& io_u32LocalIP)
{
    struct sockaddr_in stLocalIPAddr;
    stLocalIPAddr.sin_family = AF_INET;
    stLocalIPAddr.sin_port = 0;
    
    struct ifaddrs* ifaddr = NULL;
    struct ifaddrs* ifa = NULL;
    void* sin_addr = NULL;
    char addrs[INET_ADDRSTRLEN] = {0};
    if (getifaddrs(&ifaddr) == -1)
    {
        perror("getifaddrs");
        return false;
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
    {
        if (ifa->ifa_addr == NULL || ifa->ifa_name == NULL)
        {
            continue;
        }

        if (ifa->ifa_addr->sa_family == AF_INET && 0 == strncmp(ifa->ifa_name, i_pInterface, strlen(i_pInterface)))
        {
            sin_addr = &((struct sockaddr_in*) ifa->ifa_addr)->sin_addr;
            inet_ntop(AF_INET, sin_addr, addrs, INET_ADDRSTRLEN);
            break;
        }
    }

    if (NULL != ifa)
    {        
        inet_aton(addrs, &stLocalIPAddr.sin_addr);
        io_u32LocalIP = ntohl(stLocalIPAddr.sin_addr.s_addr);
    }
    else
    {
        io_u32LocalIP = 0;
    }

    printf("GetLocalIP <%s> <%d>\n", addrs, io_u32LocalIP);
    
    freeifaddrs(ifaddr);
    return true;
}

// public
bool IpavInput::setup(char *ip, int port) {
    // create UDP socket
	mSock = socket(AF_INET, SOCK_DGRAM, 0);
	
	if (mSock <= 0) {
        LOGE(LOG_TAG, "create socket failed! (%s:%d)", ip, port);
        return false;
	}

    int reuse = 1;
    if (setsockopt(mSock, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) < 0) {
        LOGE(LOG_TAG, "set SOL_SOCKET failed! (sock %d)", mSock);
        close(mSock);
        return false;
    }

    int bufSize = 5 * 1024 * 1024;
    if(setsockopt(mSock, SOL_SOCKET, SO_RCVBUF, (char*)&bufSize, sizeof(int)) < 0) {

    }
 
	// bind socket and the client of send link request
	struct sockaddr_in bindAddr;
	memset(&bindAddr, 0, sizeof(bindAddr));
	bindAddr.sin_family = AF_INET;
    bindAddr.sin_addr.s_addr = inet_addr(ip);
	bindAddr.sin_port = htons(port);
	
	if (bind(mSock, (struct sockaddr*)&bindAddr, sizeof(bindAddr)) < 0) {
        LOGE(LOG_TAG, "bind socket failed! (%s:%d)", ip, port);
		close(mSock);
		return false;
	}

    char localIp[IP_ADDR_MAX_LEN] = {0};
    getLocalIp(localIp);
    mGroup.imr_multiaddr.s_addr = inet_addr(ip);
    mGroup.imr_interface.s_addr = inet_addr(localIp);
    LOGD(LOG_TAG, "create socket (socket %d, local_ip %s, multicasting_ip %s:%d)", mSock, localIp, ip, port);

    int loop = 0;
    if (setsockopt(mSock, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop)) < 0) {
        LOGE(LOG_TAG, "set IP_MULTICAST_LOOP 0 failed! (sock %d, multicasting_ip %s:%d, errno 0x%x %s)", mSock, ip, port, errno, strerror(errno));
    }
    
    if (NULL != mIp) {
        delete mIp;
        mIp = NULL;
    }
    mIp = new char[strlen(ip) + 1];
    memset(mIp, 0, strlen(ip) + 1);
    strcpy(mIp, ip);
    mPort = port;
    return true;
}

void IpavInput::setCallbackFunc(ICallbackFunc cbFunc) {
    mCbFunc = cbFunc;
}

void IpavInput::setCallbackObj(IInputCallback *cbObj) {
    mCbObj = cbObj;
}

bool IpavInput::start() {
    if (!mStarting) {
        add_membership();
        startThread();
        mStarting = true;
    }
}

void IpavInput::stop() {
    if (mStarting) {
        stopThread();
        joinThread();
        mStarting = false;

        drop_membership();
    }
}

IpavInput::~IpavInput() {
    if (mSock > 0) {
        LOGD(LOG_TAG, "close sock %d", mSock);
        close(mSock);
        mSock = 0;
    }
}

// protected
void IpavInput::threadLoop() {
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(mSock, &fds);
    int maxfdp = mSock + 1;
    struct timeval timeout = {1, 0};
    switch(select(maxfdp, &fds, NULL, NULL, &timeout)) {
        case -1:
        {
            break;
        }
        case 0:
        {
            // LOGD(LOG_TAG, "timeout");
            add_membership();
            timeoutSock();
            break;
        }
        default:
        {
            if(FD_ISSET(mSock, &fds)) {
                selectSock(mSock);
            }
        }
    }
}

const char *IpavInput::getThreadName() {
    if (IPAV_VIDEO_PORT == mPort) {
        return "V-IN";
    } else if (IPAV_AUDIO_PORT == mPort) {
        return "A-IN";
    }
    return "IN";
}

// private
void IpavInput::selectSock(int ssock) {
    memset(mRecvBuf, 0, BUF_SIZE);
	int recvLen = -1;
    recvLen = recvfrom(ssock, mRecvBuf, BUF_SIZE, 0, NULL, NULL);
    // LOGD(LOG_TAG, "%s len %d", __func__, recvLen);
	if (recvLen > 0) {
        if (NULL != mCbFunc) {
            mCbFunc(mRecvBuf, recvLen);
        }
        if (NULL != mCbObj) {
            mCbObj->dataCallback(mRecvBuf, recvLen, mIp, mPort);
            if (!mConnected) {
                mCbObj->connectCallback(true);
                mConnected = true;
                mTimeoutCnt = 0;
            }
        }
	}
}

void IpavInput::timeoutSock() {
    mTimeoutCnt++;
    // LOGD(LOG_TAG, "%s timeout %d conntect %d", __func__, mTimeoutCnt, mConnected);
    if (mConnected && (mTimeoutCnt > 8) && (NULL != mCbObj)) {
        mCbObj->connectCallback(false);
        mConnected = false;
    }
}

void IpavInput::add_membership() {
    if (mSock > 0 && NULL != mIp) {
        char localIp[IP_ADDR_MAX_LEN] = {0};
        getLocalIp(localIp);
        mGroup.imr_multiaddr.s_addr = inet_addr(mIp);
        mGroup.imr_interface.s_addr = inet_addr(localIp);
        // LOGD(LOG_TAG, "%s (socket %d, local_ip %s, multicasting_ip %s:%d)", __func__, mSock, localIp, mIp, mPort);
        if (setsockopt(mSock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mGroup, sizeof(mGroup)) < 0) {
            // LOGE(LOG_TAG, "%s failed! (sock %d, multicasting_ip %s:%d, errno 0x%x)", __func__, mSock, mIp, mPort, errno);
        } else {
            // LOGD(LOG_TAG, "%s success", __func__);
        }
    }
}

void IpavInput::drop_membership() {
    if (mSock > 0) {
        setsockopt(mSock, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char *)&mGroup, sizeof(mGroup));
    }
}