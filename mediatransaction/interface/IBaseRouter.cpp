/*
 *  IBaseRouter.cpp
 *
 *  Created on: 2019-05-15
 *      Author: knox
 */

#include "interface/IBaseRouter.h"

#include <assert.h>
#include "common/MtLog.h"

#define LOG_TAG "IBaseRouter"

IBaseRouter::IBaseRouter() : mListener(NULL) {
    memset(mSourceArray, 0, sizeof(IBaseSource *) * ROUTER_MAX_SOURCE);
}

int IBaseRouter::getIndex(IBaseSource *source) {
    int i;
    for (i = 0; i < ROUTER_MAX_SOURCE; i++) {
        if (source == mSourceArray[i]) {
            return i;
        }
    }
    return -1;
}

IBaseSource *IBaseRouter::getObject(int index) {
    if (index < 0 || index >= ROUTER_MAX_SOURCE) {
        LOGE(LOG_TAG, "%s invalid index %d ", __func__, index);
        return NULL;
    }
    return mSourceArray[index];
}

void IBaseRouter::addObject(IBaseSource *source, int index) {
    if (index < 0 || index >= ROUTER_MAX_SOURCE) {
        LOGE(LOG_TAG, "%s invalid index %d ", __func__, index);
        return;
    }
    if (NULL != mSourceArray[index]) {
        dumpAllObject();
        assert(1);
    }
    mSourceArray[index] = source;
}

void IBaseRouter::removeObject(int index) {
    mSourceArray[index] = NULL;
}

void IBaseRouter::dumpAllObject() {
    LOGD(LOG_TAG, "========%s begin========", __func__);
    int i;
    for (i = 0; i < ROUTER_MAX_SOURCE; i++) {
        IBaseSource *source = mSourceArray[i];
        if (NULL != source) {
            LOGD(LOG_TAG, "%d ==> %s", i, source->dumpDescption());
        }
    }
    LOGD(LOG_TAG, "========%s end========", __func__);
}