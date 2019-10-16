/*
 *  MtLog.h
 *
 *  Created on: 2019-05-15
 *      Author: knox
 */

#ifndef MT_LOG_H_
#define MT_LOG_H_

#include <stdio.h>
#include <sys/timeb.h>
#include <time.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>
#ifdef LOG4CPP
#include <log4cpp/log4cpp.hh>
#endif

#define LOG_LEVEL_ERROR 0
#define LOG_LEVEL_INFO  1
#define LOG_LEVEL_DEBUG 2

#define gettid() syscall(SYS_gettid)

static int debug_level = LOG_LEVEL_DEBUG;

static const char *mt_log_time(void)
{
    struct  tm      *ptm;
    struct  timeb   stTimeb;
    static  char    szTime[19];
 
    ftime(&stTimeb);
    ptm = localtime(&stTimeb.time);
    sprintf(szTime, "%02d-%02d %02d:%02d:%02d.%03d",
        ptm->tm_mon+1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec, stTimeb.millitm);
    szTime[18] = 0;
    return szTime;
}

static const long mt_gettid() {
    return gettid();
}

static const int mt_getpid() {
    return getpid();
}

static void setupLogFile(bool p2n, const char *conf) {
    if (p2n) {
        int fd;
        if ((fd = open("/dev/null", O_RDWR, 0)) != -1) {
            (void)dup2(fd, STDIN_FILENO);
            (void)dup2(fd, STDOUT_FILENO);
            (void)dup2(fd, STDERR_FILENO);
            if (fd > 2)
                (void)close (fd);
        }
    }
    

    MYLog_Init(conf);
}

#define LOG(TAG, format, ...) do { \
    fprintf(stderr, "%s %ld %d " TAG " : " format "\n", \
    mt_log_time(), mt_gettid(), mt_getpid(), ##__VA_ARGS__); \
    fflush(stderr); \
} while(0);

#ifdef LOG4CPP
#define LOGE(TAG, format, ...) do { \
    MYLog_Log(LL_ERROR, "%ld %d : " format " ", mt_gettid(), mt_getpid(), ##__VA_ARGS__); \
} while(0);
#else
#define LOGE(TAG, format, ...) do { \
    fprintf(stderr, "%s %ld %d E " TAG ": " format "\n", \
    mt_log_time(), mt_gettid(), mt_getpid(), ##__VA_ARGS__); \
    fflush(stderr); \
} while(0);
#endif


#ifdef LOG4CPP
#define LOGI(TAG, format, ...) do { \
    MYLog_Log(LL_INFO, "%ld %d : " format " ", mt_gettid(), mt_getpid(), ##__VA_ARGS__); \
} while(0);
#else
#define LOGI(TAG, format, ...) do { \
    if (debug_level >= LOG_LEVEL_INFO) { \
        fprintf(stderr, "%s %ld %d I " TAG ": " format "\n", \
        mt_log_time(), mt_gettid(), mt_getpid(), ##__VA_ARGS__); \
        fflush(stderr); \
    } \
} while(0);
#endif

#ifdef LOG4CPP
#define LOGD(TAG, format, ...) do { \
    MYLog_Log(LL_INFO, "%ld %d : " format " ", mt_gettid(), mt_getpid(), ##__VA_ARGS__); \
} while(0);
#else
#define LOGD(TAG, format, ...) do { \
    if (debug_level >= LOG_LEVEL_DEBUG) { \
        fprintf(stderr, "%s %ld %d D " TAG ": " format "\n", \
        mt_log_time(), mt_gettid(), mt_getpid(), ##__VA_ARGS__); \
        fflush(stderr); \
    } \
} while(0);
#endif

#define FUNC_IN(TAG) LOGD(TAG, "%s In",  __func__)
#define FUNC_OUT(TAG) LOGD(TAG, "%s Out",  __func__)

#ifdef LOG4CPP
#define LOG_DUMP(format, ...) do { \
    MYLog_Log(LL_INFO, "%ld %d : " format " ", mt_gettid(), mt_getpid(), ##__VA_ARGS__); \
} while(0);
#else
#define LOG_DUMP(format, ...) do { \
    fprintf(stderr, "%s %ld %d P " format "\n", \
    mt_log_time(), mt_gettid(), mt_getpid(), ##__VA_ARGS__); \
    fflush(stderr); \
} while(0);
#endif

#endif // MT_LOG_H_