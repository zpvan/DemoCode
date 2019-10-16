/*
 *  MtNetMessage.cpp
 *
 *  Created on: 2019-05-15
 *      Author: knox
 */

#include "common/MtNetMessage.h"

#include <stdlib.h>
#include <assert.h>
#include "common/MtConstant.h"
#include "common/MtLog.h"
#include "common/MtUtils.h"

#define LOG_TAG "MtNetMessage"

#define MAX(a,b) ((a)>(b)?(a):(b))

// public static 
MtNetMessage *MtNetMessage::createObject() {
    return new MtNetMessage();
}

// public static
void MtNetMessage::releaseObject(MtNetMessage *pMessage) {
    if (NULL == pMessage) {
        return;
    }
    pMessage->freeArray();
    pMessage->freeProp();
    pMessage->freeParam();
    pMessage->freeHeader();
    delete pMessage;
    pMessage = NULL;
}

// public static
/**
 * must release it outside
 **/
uint32_t MtNetMessage::messageMalloc(MtNetMessage **ppMessage, char *array, int len) {
    MtNetMessage *pMsg = new MtNetMessage();
    *ppMessage = pMsg;

    pMsg->mallocMsgHeader();
    NetMsgHeader *h = pMsg->mHeader;
    if (NULL == h) {
        LOGE(LOG_TAG, "%s error (malloc header failed)", __func__);
        return MALLOC_ERROR;
    }
    if (NULL == array) {
        // malloc an empty message
        // TODO Log (pMsg->mHeader == h?) (*ppMessage = pMsg?)
        // pMsg->mHeader = h;
        // *ppMessage = pMsg;
        return 0;
    }

    if (len < MESSAGE_HEADER_SIZE) {
        LOGE(LOG_TAG, "%s error ((length %d) < (header_size %d))", __func__, len, MESSAGE_HEADER_SIZE);
        return HEADER_SIZE_NO_ENOUGH_ERROR;
    }
    pMsg->buildMsgHeader(array);
    // check magic code
    if (h->magicCode[0] != 0x55 || 
        h->magicCode[1] != 0x66 ||
        h->magicCode[2] != 0x77 ||
        h->magicCode[3] != 0x88) {
        LOGE(LOG_TAG, "%s error (magic code 0x%02x 0x%02x 0x%02x 0x%02x)", __func__, *(array + 0), *(array + 1), *(array + 2), *(array + 3));
        return MAGIC_CODE_ERROR;
    }
    pMsg->setMsgType1();
    // check vertion number
    if (h->version > MSG_VERSION_NUMBER) {
        LOGE(LOG_TAG, "%s error (version number %d > supported %d)", __func__, h->version, MSG_VERSION_NUMBER);
        return VERSION_NUM_ERROR;
    }
    // TODO check msg type
    if (MSG_TYPE_NIL == pMsg->mType) {
        LOGE(LOG_TAG, "%s error (type %c%c%c)", __func__, *(array + 5), *(array + 6), *(array + 7));
        return MESSAGE_TYPE_ERROR;
    }
    pMsg->setMsgCode2();
    // TODO check msg code
    if (MSG_CODE_NIL == pMsg->mCode) {
        LOGE(LOG_TAG, "%s error (type %c%c%c, code 0x%x)", __func__, *(array + 5), *(array + 6), *(array + 7), h->msgCode);
        return MESSAGE_CODE_ERROR;
    }
    // TODO check size
    if ((len - MESSAGE_HEADER_SIZE) < MAX(h->paramSize, pMsg->caluParamSize())) {
        LOGE(LOG_TAG, "%s error (paramSize max(%u %u), real_size %u)", __func__, h->paramSize, pMsg->caluParamSize(), len - MESSAGE_HEADER_SIZE);
        return MESSAGE_LENG_ERROR;
    }
    pMsg->mallocMsgParam();
    if (NULL == pMsg->mParam) {
        *ppMessage = pMsg;
        return 0;
    }
    pMsg->buildMsgParam(array + MESSAGE_HEADER_SIZE);
    if (0 == pMsg->getPropSize()) {
        *ppMessage = pMsg;
        return 0;
    } else {
        LOGD(LOG_TAG, "%s prop_size %d", __func__, pMsg->getPropSize());
    }
    if ((len - MESSAGE_HEADER_SIZE - pMsg->caluParamSize()) < pMsg->getPropSize()) {
        LOGE(LOG_TAG, "%s error (prop_size %u, real_size %u)", __func__, pMsg->getPropSize(), (len - MESSAGE_HEADER_SIZE - pMsg->caluParamSize()));
        assert(0);
        return MESSAGE_LENG_ERROR;
    }
    pMsg->mallocMsgProp();
    pMsg->buildMsgProp(array + MESSAGE_HEADER_SIZE + pMsg->caluParamSize());
    
    return 0;
}

// public static
void MtNetMessage::messageFree(MtNetMessage *p_message) {
    MtNetMessage::releaseObject(p_message);
}

// public
NetMsgHeader *MtNetMessage::makeHeader(MsgType type, MsgCode code) {
    freeHeader();
    mallocMsgHeader();
    if (NULL == mHeader) {
        LOGE(LOG_TAG, "%s error (header is null)", __func__);
        return NULL;
    }
    NetMsgHeader *header = mHeader;
    header->magicCode[0] = 0x55;
    header->magicCode[1] = 0x66;
    header->magicCode[2] = 0x77;
    header->magicCode[3] = 0x88;
    header->version = MSG_VERSION_NUMBER;
    if (MSG_TYPE_CMD == type) {
        header->msgType[0] = 'C';
        header->msgType[1] = 'M';
        header->msgType[2] = 'D';
    } else if (MSG_TYPE_EVT == type) {
        header->msgType[0] = 'E';
        header->msgType[1] = 'V';
        header->msgType[2] = 'T';
    } else if (MSG_TYPE_RES == type) {
        header->msgType[0] = 'R';
        header->msgType[1] = 'E';
        header->msgType[2] = 'S';
    }
    header->msgNumber = getSequentNumber();
    header->msgCode = code;

    setMsgType1();
    setMsgCode2();

    header->paramSize = caluParamSize();
}

NetMsgHeader *MtNetMessage::getHeader() {
    return mHeader;
}

void *MtNetMessage::makeEmptyParam() {
    freeParam();
    mallocMsgParam();
    return mParam;
}

void *MtNetMessage::getParam() {
    return mParam;
}

uint32_t MtNetMessage::makeProp(char *buffer, int len) {
    if (NULL == mHeader) {
        LOGE(LOG_TAG, "%s error (header is null)", __func__);
        return HEADER_NULL_ERROR;
    }
    if (NULL == mParam) {
        LOGE(LOG_TAG, "%s error (param is null)", __func__);
        return PARAM_NULL_ERROR;
    }
    if (NULL == buffer || 0 == len) {
        LOGE(LOG_TAG, "%s error (buffer is null or len is zero)", __func__);
        return INPUT_NULL_ERROR;
    }
    if (MSG_CODE_CMD_SOURCE_AVAILABLE == mCode) {
        param_mcl_source_available *param = (param_mcl_source_available *)mParam;
        param->prop_size = len;
        mallocMsgProp();
        buildMsgProp(buffer);
    }
    return 0;
}

MsgCode MtNetMessage::getMsgCode() {
    return mCode;
}

MsgType MtNetMessage::getMsgType() {
    return mType;
}

uint32_t MtNetMessage::makeArray() {
    if (NULL == mHeader) {
        LOGE(LOG_TAG, "%s error (header is null)", __func__);
        return HEADER_NULL_ERROR;
    }
    uint32_t len = MESSAGE_HEADER_SIZE;
    // LOGD(LOG_TAG, "add header (len %d)", len);
    len += getParamSize();
    // LOGD(LOG_TAG, "add param (len %d)", len);
    len += getPropSize();
    // LOGD(LOG_TAG, "add prop (len %d)", len);

    freeArray();
    
    mArray = (char *)malloc(len);
    if (NULL == mArray) {
        LOGE(LOG_TAG, "%s error (malloc failed %u)", __func__, len);
        return MALLOC_ERROR;
    }
    mArrayLen = len;

    buildHeaderArray();
    buildParamArray();
    buildPropArray();

    return 0;
}

char *MtNetMessage::getArray() {
    return mArray;
}

int MtNetMessage::getArrayLength() {
    return mArrayLen;
}

int MtNetMessage::getSupportedVersion() {
    return MSG_VERSION_NUMBER;
}

uint16_t MtNetMessage::getParamStructSize() {
    return caluParamSize();
}

void MtNetMessage::printHeader() {
    if (NULL == mHeader) {
        return;
    }
    LOGD(LOG_TAG, "===print_header begin===");
    LOGD(LOG_TAG, "magicCode 0x%02x 0x%02x 0x%02x 0x%02x", mHeader->magicCode[0], mHeader->magicCode[1], mHeader->magicCode[2], mHeader->magicCode[3]);
    LOGD(LOG_TAG, "version %d", mHeader->version);
    LOGD(LOG_TAG, "msgType %c %c %c", mHeader->msgType[0], mHeader->msgType[1], mHeader->msgType[2]);
    LOGD(LOG_TAG, "msgNumber %lu", mHeader->msgNumber);
    LOGD(LOG_TAG, "msgCode 0x%08x", mHeader->msgCode);
    LOGD(LOG_TAG, "paramSize %d", mHeader->paramSize);
    LOGD(LOG_TAG, "===print_header end===");
}

// private
void MtNetMessage::mallocMsgHeader() {
    NetMsgHeader *header = (NetMsgHeader *)malloc(sizeof(NetMsgHeader));
    if (NULL == header) {
        LOGE(LOG_TAG, "%s error (malloc failed %lu)", __func__, sizeof(NetMsgHeader));
        return;
    }
    mHeader = header;
}

void MtNetMessage::buildMsgHeader(char *arrayOffsetHeader) {
    if (NULL == mHeader) {
        return;
    }
    char *array = arrayOffsetHeader;
    mHeader->magicCode[0] = *(array + 0);
    mHeader->magicCode[1] = *(array + 1);
    mHeader->magicCode[2] = *(array + 2);
    mHeader->magicCode[3] = *(array + 3);
    
    mHeader->version = *(array + 4);
    mHeader->msgType[0] = *(array + 5);
    mHeader->msgType[1] = *(array + 6);
    mHeader->msgType[2] = *(array + 7);
    
    mHeader->msgNumber = le_chars_2_uint32(array + 8);
    mHeader->msgCode = le_chars_2_uint32(array + 12);

    mHeader->paramSize = le_chars_2_uint16(array + 16);
}

void MtNetMessage::setMsgType1() {
    if (NULL == mHeader) {
        return;
    }
    if (mHeader->msgType[0] == 'C' && mHeader->msgType[1] == 'M' && mHeader->msgType[2] == 'D') {
        mType = MSG_TYPE_CMD;
    } else if (mHeader->msgType[0] == 'E' && mHeader->msgType[1] == 'V' && mHeader->msgType[2] == 'T') {
        mType = MSG_TYPE_EVT;
    } else if (mHeader->msgType[0] == 'R' && mHeader->msgType[1] == 'E' && mHeader->msgType[2] == 'S') {
        mType = MSG_TYPE_RES;
    }
}

void MtNetMessage::setMsgCode2() {
    if (NULL == mHeader) {
        return;
    }
    switch (mType) {
        case MSG_TYPE_CMD:
        {
            setMsgCmdCode();
            break;
        }

        case MSG_TYPE_EVT:
        {
            setMsgEvtCode();
            break;
        }

        case MSG_TYPE_RES:
        {
            setMsgResCode();
            break;
        }

        default:
        {
            LOGE(LOG_TAG, "%s error (unknown code 0x%x)", __func__, mHeader->msgCode);
            mCode = MSG_CODE_NIL;
            break;
        }
    }
}

void MtNetMessage::setMsgCmdCode() {
    if (NULL == mHeader) {
        return;
    }
    switch (mHeader->msgCode) {
        case MSG_CODE_CMD_ALLOC_VFS:
        case MGS_CODE_CMD_FREE_VFS:
        case MSG_CODE_CMD_SOURCE_AVAILABLE:
        case MSG_CODE_CMD_SOURCE_UNAVAILABLE:
        case MSG_CODE_CMD_ENABLE_UCAM_SERVICE:
        case MSG_CODE_CMD_DISABLE_UCAM_SERVICE:
        case MSG_CODE_CMD_START_UCAM_PREVIEW:
        case MSG_CODE_CMD_STOP_UCAM_PREVIEW:
        case MSG_CODE_CMD_ENABLE_IPAV_SERVICE:
        case MSG_CODE_CMD_DISABLE_IPAV_SERVICE:
        case MSG_CODE_CMD_START_IPAV_PREVIEW:
        case MSG_CODE_CMD_STOP_IPAV_PREVIEW:
        {
            mCode = (MsgCode) mHeader->msgCode;
            break;
        }

        default:
        {
            LOGE(LOG_TAG, "%s error (unknown code 0x%x)", __func__, mHeader->msgCode);
            mCode = MSG_CODE_NIL;
            break;
        }
    }
}

void MtNetMessage::setMsgEvtCode() {
    if (NULL == mHeader) {
        return;
    }
    switch (mHeader->msgCode) {
        case MSG_CODE_EVT_SUCCESS:
        case MSG_CODE_EVT_FAIL:
        {
            mCode = (MsgCode) mHeader->msgCode;
            break;
        }

        default:
        {
            LOGE(LOG_TAG, "%s error (unknown code 0x%x)", __func__, mHeader->msgCode);
            mCode = MSG_CODE_NIL;
            break;
        }
    }
}

void MtNetMessage::setMsgResCode() {
    if (NULL == mHeader) {
        return;
    }
    switch (mHeader->msgCode) {
        case MSG_CODE_RES_CONFIRM:
        case MSG_CODE_RES_SUCCESS:
        case MSG_CODE_RES_FAIL:
        case MSG_CODE_RES_EXCEPTION:
        case MSG_CODE_RES_VER_EXCEP:
        case MSG_CODE_RES_TYPE_EXCEP:
        case MSG_CODE_RES_NUM_EXCEP:
        case MSG_CODE_RES_CODE_EXCEP:
        case MSG_CODE_RES_LENG_EXCEP:
        case MSG_CODE_RES_HS_NO_ENOUGH:
        case MSG_CODE_RES_ALLOC_VFS:
        {
            mCode = (MsgCode) mHeader->msgCode;
            break;
        }

        default:
        {
            LOGE(LOG_TAG, "%s error (unknown code 0x%x)", __func__, mHeader->msgCode);
            mCode = MSG_CODE_NIL;
            break;
        }
    }
}

void MtNetMessage::mallocMsgParam() {
    uint64_t size = caluParamSize();
    if (0 == size) {
        LOGI(LOG_TAG, "size of param is 0 (code %d)", mCode);
        return;
    } else {
        void *param = (void *)malloc(size);
        if (NULL == param) {
            LOGE(LOG_TAG, "%s error (failed code %d, size %lu)", __func__, mCode, size);
            return;
        }
        memset(param, 0, size);
        mParam = param;
    }
}

void MtNetMessage::buildMsgParam(char *arrayOffsetParam) {
    switch (mType) {
        case MSG_TYPE_CMD:
        {
            buildMsgCmdParam(arrayOffsetParam);
            break;
        }

        case MSG_TYPE_EVT:
        {
            buildMsgEvtParam(arrayOffsetParam);
            break;
        }

        case MSG_TYPE_RES:
        {
            buildMsgResParam(arrayOffsetParam);
            break;
        }

        default:
        {
            LOGE(LOG_TAG, "%s error (type %d)", __func__, mType);
            break;
        }
    }
}

void MtNetMessage::buildMsgCmdParam(char *arrayOffsetParam) {
    if (NULL == mParam) {
        return;
    }
    char *array = arrayOffsetParam;
    switch (mCode) {
        case MSG_CODE_CMD_SOURCE_AVAILABLE:
        {
            param_mcl_source_available *param = (param_mcl_source_available *)mParam;
            param->number = le_chars_2_uint16(array);
            param->prop_size = le_chars_2_uint16(array + 2);
            break;
        }

        case MSG_CODE_CMD_SOURCE_UNAVAILABLE:
        {
            param_mcl_source_unavailable *param = (param_mcl_source_unavailable *)mParam;
            param->number = le_chars_2_uint16(array);
            break;
        }

        case MSG_CODE_CMD_ALLOC_VFS:
        {
            // should not be here
            break;
        }

        case MGS_CODE_CMD_FREE_VFS:
        {
            // should not be here
            break;
        }

        case MSG_CODE_CMD_ENABLE_UCAM_SERVICE:
        {
            // no param
            break;
        }

        case MSG_CODE_CMD_DISABLE_UCAM_SERVICE:
        {
            // no param
            break;
        }

        case MSG_CODE_CMD_START_UCAM_PREVIEW:
        {
            param_start_ucam_preview *param = (param_start_ucam_preview *)mParam;
            param->number = le_chars_2_uint16(array);
            break;
        }

        case MSG_CODE_CMD_STOP_UCAM_PREVIEW:
        {
            param_stop_ucam_preview *param = (param_stop_ucam_preview *)mParam;
            param->number = le_chars_2_uint16(array);
            break;
        }

        case MSG_CODE_CMD_ENABLE_IPAV_SERVICE:
        {
            param_enable_ipav_service *param = (param_enable_ipav_service *)mParam;
            param->number = le_chars_2_uint16(array);
            param->mac_addr[0] = *(array + 2);
            param->mac_addr[1] = *(array + 3);
            param->mac_addr[2] = *(array + 4);
            param->mac_addr[3] = *(array + 5);
            param->mac_addr[4] = *(array + 6);
            param->mac_addr[5] = *(array + 7);
            break;
        }

        case MSG_CODE_CMD_DISABLE_IPAV_SERVICE:
        {
            param_disable_ipav_service *param = (param_disable_ipav_service *)mParam;
            param->number = le_chars_2_uint16(array);
            break;
        }

        case MSG_CODE_CMD_START_IPAV_PREVIEW:
        {
            param_start_ipav_preview *param = (param_start_ipav_preview *)mParam;
            param->number = le_chars_2_uint16(array);
            break;
        }

        case MSG_CODE_CMD_STOP_IPAV_PREVIEW:
        {
            param_stop_ipav_preview *param = (param_stop_ipav_preview *)mParam;
            param->number = le_chars_2_uint16(array);
            break;
        }

        default:
        {
            LOGE(LOG_TAG, "%s error (code %d)", __func__, mCode);
            break;
        }
    }
}

void MtNetMessage::buildMsgEvtParam(char *arrayOffsetParam) {
    if (NULL == mParam) {
        return;
    }
    char *array = arrayOffsetParam;
    switch (mCode) {
        case MSG_CODE_EVT_SUCCESS:
        {
            param_evt_res_success *param = (param_evt_res_success *)mParam;
            param->msg_number = le_chars_2_uint32(array);
            param->msg_code = le_chars_2_uint32(array + 4);
            break;
        }

        case MSG_CODE_EVT_FAIL:
        {
            param_evt_res_fail *param = (param_evt_res_fail *)mParam;
            param->msg_number = le_chars_2_uint32(array);
            param->msg_code = le_chars_2_uint32(array + 4);
            param->module_number = le_chars_2_uint32(array + 8);
            break;
        }

        default:
        {
            LOGE(LOG_TAG, "%s error (code %d)", __func__, mCode);
            break;
        }
    }
}

void MtNetMessage::buildMsgResParam(char *arrayOffsetParam) {
    if (NULL == mParam) {
        return;
    }
    char *array = arrayOffsetParam;
    switch (mCode) {
        case MSG_CODE_RES_CONFIRM:
        {
            param_res_confirm *param = (param_res_confirm *)mParam;
            param->msg_number = le_chars_2_uint32(array);
            param->msg_code = le_chars_2_uint32(array + 4);
            break;
        }

        case MSG_CODE_RES_SUCCESS:
        {
            param_evt_res_success *param = (param_evt_res_success *)mParam;
            param->msg_number = le_chars_2_uint32(array);
            param->msg_code = le_chars_2_uint32(array + 4);
            break;
        }

        case MSG_CODE_RES_FAIL:
        {
            param_evt_res_fail *param = (param_evt_res_fail *)mParam;
            param->msg_number = le_chars_2_uint32(array);
            param->msg_code = le_chars_2_uint32(array + 4);
            param->module_number = le_chars_2_uint32(array + 8);
            break;
        }

        case MSG_CODE_RES_EXCEPTION:
        {
            param_res_exception *param = (param_res_exception *)mParam;
            param->magic_code[0] = *(array);
            param->magic_code[1] = *(array + 1);
            param->magic_code[2] = *(array + 2);
            param->magic_code[3] = *(array + 3);
            break;
        }

        case MSG_CODE_RES_VER_EXCEP:
        {
            param_res_ver_excep *param = (param_res_ver_excep *)mParam;
            param->supported_version = *(array);
            param->exception_version = *(array + 1);
            break;
        }

        case MSG_CODE_RES_TYPE_EXCEP:
        {
            param_res_type_excep *param = (param_res_type_excep *)mParam;
            param->exception_type[0] = *(array);
            param->exception_type[1] = *(array + 1);
            param->exception_type[2] = *(array + 2);
            break;
        }

        case MSG_CODE_RES_NUM_EXCEP:
        {
            param_res_num_excep *param = (param_res_num_excep *)mParam;
            param->cur_number = le_chars_2_uint32(array);
            param->err_number = le_chars_2_uint32(array + 4);
            break;
        }

        case MSG_CODE_RES_CODE_EXCEP:
        {
            param_res_code_excep *param = (param_res_code_excep *)mParam;
            param->msg_number = le_chars_2_uint32(array);
            param->err_code = le_chars_2_uint32(array + 4);
            break;
        }

        case MSG_CODE_RES_LENG_EXCEP:
        {
            param_res_leng_excep *param = (param_res_leng_excep *)mParam;
            param->msg_number = le_chars_2_uint32(array);
            param->msg_code = le_chars_2_uint32(array + 4);
            param->param_size = le_chars_2_uint16(array + 8);
            param->struct_size = le_chars_2_uint16(array + 10);
            break;
        }

        case MSG_CODE_RES_ALLOC_VFS:
        {
            param_res_mda_alloc_vfs *param = (param_res_mda_alloc_vfs *)mParam;
            param->number = le_chars_2_uint16(array);
            param->sock_port = le_chars_2_uint16(array + 2);
            if (0 == param->sock_port) {
                param->shm_handle = le_chars_2_uint32(array + 4);
            } else {
                param->shm_handle = 0;
            }
            break;
        }

        default:
        {
            LOGE(LOG_TAG, "%s error (code %d)", __func__, mCode);
            break;
        }
    }
}

void MtNetMessage::mallocMsgProp() {
    char *prop = (char *)malloc(getPropSize());
    if (NULL == prop) {
        LOGE(LOG_TAG, "%s error (failed size %u)", __func__, getPropSize());
        return;
    }
    memset(prop, 0, getPropSize());
    mProp = prop;
}

void MtNetMessage::buildMsgProp(char *arrayOffsetProp) {
    char *array = arrayOffsetProp;
    if (NULL == array) {
        LOGE(LOG_TAG, "%s error (array is null)", __func__);
        return;
    }
    memcpy(mProp, array, getPropSize());
}

void MtNetMessage::freeArray() {
    if (NULL == mArray) {
        return;
    }
    free(mArray);
    mArray = NULL;

    mArrayLen = 0;
}

void MtNetMessage::freeProp() {
    if (NULL == mProp) {
        return;
    }
    free(mProp);
    mProp = NULL;
}

void MtNetMessage::freeParam() {
    if (NULL == mParam) {
        return;
    }
    free(mParam);
    mParam = NULL;
}

void MtNetMessage::freeHeader() {
    if (NULL == mHeader) {
        return;
    }
    free(mHeader);
    mHeader = NULL;

    mType = MSG_TYPE_NIL;
    mCode = MSG_CODE_NIL;
}

int MtNetMessage::getParamSize() {
    if (NULL == mHeader) {
        return 0;
    }
    return mHeader->paramSize;
}

int MtNetMessage::getPropSize() {
    if (NULL == mParam) {
        return 0;
    }
    int prop_size = 0;
    if (MSG_CODE_CMD_SOURCE_AVAILABLE == mCode) {
        param_mcl_source_available *param = (param_mcl_source_available *)mParam;
        prop_size = param->prop_size;
    }
    return prop_size;
}

void MtNetMessage::buildHeaderArray() {
    if (NULL == mHeader) {
        return;
    }
    if (NULL == mArray) {
        return;
    }
    *(mArray + 0) = mHeader->magicCode[0];
    *(mArray + 1) = mHeader->magicCode[1];
    *(mArray + 2) = mHeader->magicCode[2];
    *(mArray + 3) = mHeader->magicCode[3];

    *(mArray + 4) = mHeader->version;

    *(mArray + 5) = mHeader->msgType[0];
    *(mArray + 6) = mHeader->msgType[1];
    *(mArray + 7) = mHeader->msgType[2];

    uint32_2_le_chars4(mHeader->msgNumber, mArray + 8);

    uint32_2_le_chars4(mHeader->msgCode, mArray + 12);

    uint16_2_le_chars2(mHeader->paramSize, mArray + 16);
}

void MtNetMessage::buildParamArray() {
    if (NULL == mParam) {
        return;
    }
    if (NULL == mArray) {
        return;
    }
    char *array = mArray + MESSAGE_HEADER_SIZE;
    switch (mType) {
        case MSG_TYPE_CMD:
        {
            buildCmdParamArray(array);
            break;
        }

        case MSG_TYPE_EVT:
        {
            buildEvtParamArray(array);
            break;
        }

        case MSG_TYPE_RES:
        {
            buildResParamArray(array);
            break;
        }
    }
}

void MtNetMessage::buildCmdParamArray(char *array) {
    if (NULL == mParam) {
        return;
    }
    
    switch (mCode) {
        case MSG_CODE_CMD_SOURCE_AVAILABLE:
        {
            param_mcl_source_available *param = (param_mcl_source_available *)mParam;
            uint16_2_le_chars2(param->number, array);
            uint16_2_le_chars2(param->prop_size, array + 2);
            break;
        }

        case MSG_CODE_CMD_SOURCE_UNAVAILABLE:
        {
            param_mcl_source_unavailable *param = (param_mcl_source_unavailable *)mParam;
            uint16_2_le_chars2(param->number, array);
            break;
        }

        case MSG_CODE_CMD_ALLOC_VFS:
        {
            param_mda_alloc_vfs *param = (param_mda_alloc_vfs *)mParam;
            uint16_2_le_chars2(param->number, array);
            uint16_2_le_chars2(param->video_type, array + 2);
            uint16_2_le_chars2(param->width, array + 4);
            uint16_2_le_chars2(param->height, array + 6);
            uint16_2_le_chars2(param->mode, array + 8);
            uint16_2_le_chars2(param->audio_type, array + 10);
            break;
        }

        case MGS_CODE_CMD_FREE_VFS:
        {
            param_mda_alloc_vfs *param = (param_mda_alloc_vfs *)mParam;
            uint16_2_le_chars2(param->number, array);
            break;
        }

        case MSG_CODE_CMD_START_UCAM_PREVIEW:
        {
            param_start_ucam_preview *param = (param_start_ucam_preview *)mParam;
            uint16_2_le_chars2(param->number, array);
            break;
        }

        case MSG_CODE_CMD_STOP_UCAM_PREVIEW:
        {
            param_stop_ucam_preview *param = (param_stop_ucam_preview *)mParam;
            uint16_2_le_chars2(param->number, array);
            break;
        }

        case MSG_CODE_CMD_ENABLE_IPAV_SERVICE:
        {
            param_enable_ipav_service *param = (param_enable_ipav_service *)mParam;
            uint16_2_le_chars2(param->number, array);
            *(array + 2) = param->mac_addr[0];
            *(array + 3) = param->mac_addr[1];
            *(array + 4) = param->mac_addr[2];
            *(array + 5) = param->mac_addr[3];
            *(array + 6) = param->mac_addr[4];
            *(array + 7) = param->mac_addr[5];
            break;
        }

        case MSG_CODE_CMD_DISABLE_IPAV_SERVICE:
        {
            param_disable_ipav_service *param = (param_disable_ipav_service *)mParam;
            uint16_2_le_chars2(param->number, array);
            break;
        }

        case MSG_CODE_CMD_START_IPAV_PREVIEW:
        {
            param_start_ipav_preview *param = (param_start_ipav_preview *)mParam;
            uint16_2_le_chars2(param->number, array);
            break;
        }

        case MSG_CODE_CMD_STOP_IPAV_PREVIEW:
        {
            param_stop_ipav_preview *param = (param_stop_ipav_preview *)mParam;
            uint16_2_le_chars2(param->number, array);
            break;
        }

        default:
        {
            break;
        }
    }
}

void MtNetMessage::buildEvtParamArray(char *array) {
    if (NULL == mParam) {
        return;
    }
    switch (mCode)
    {
        case MSG_CODE_EVT_SUCCESS:
        {
            param_evt_res_success *param = (param_evt_res_success *)mParam;
            uint32_2_le_chars4(param->msg_number, array);
            uint32_2_le_chars4(param->msg_code, array + 4);
            break;
        }

        case MSG_CODE_EVT_FAIL:
        {
            param_evt_res_fail *param = (param_evt_res_fail *)mParam;
            uint32_2_le_chars4(param->msg_number, array);
            uint32_2_le_chars4(param->msg_code, array + 4);
            uint32_2_le_chars4(param->module_number, array + 8);
            break;
        }
    
        default:
            break;
    }
}

void MtNetMessage::buildResParamArray(char *array) {
    if (NULL == mParam) {
        return;
    }
    switch (mCode)
    {
        case MSG_CODE_RES_CONFIRM:
        {
            param_res_confirm *param = (param_res_confirm *)mParam;
            uint32_2_le_chars4(param->msg_number, array);
            uint32_2_le_chars4(param->msg_code, array + 4);
            break;
        }

        case MSG_CODE_RES_SUCCESS:
        {
            param_evt_res_success *param = (param_evt_res_success *)mParam;
            uint32_2_le_chars4(param->msg_number, array);
            uint32_2_le_chars4(param->msg_code, array + 4);
            break;
        }

        case MSG_CODE_RES_FAIL:
        {
            param_evt_res_fail *param = (param_evt_res_fail *)mParam;
            uint32_2_le_chars4(param->msg_number, array);
            uint32_2_le_chars4(param->msg_code, array + 4);
            uint32_2_le_chars4(param->module_number, array + 8);
            break;
        }

        case MSG_CODE_RES_EXCEPTION:
        {
            param_res_exception *param = (param_res_exception *)mParam;
            *(array + 0) = param->magic_code[0];
            *(array + 1) = param->magic_code[1];
            *(array + 2) = param->magic_code[2];
            *(array + 3) = param->magic_code[3];
            break;
        }

        case MSG_CODE_RES_VER_EXCEP:
        {
            param_res_ver_excep *param = (param_res_ver_excep *)mParam;
            *(array + 0) = param->supported_version;
            *(array + 1) = param->exception_version;
            break;
        }

        case MSG_CODE_RES_TYPE_EXCEP:
        {
            param_res_type_excep *param = (param_res_type_excep *)mParam;
            *(array + 0) = param->exception_type[0];
            *(array + 1) = param->exception_type[1];
            *(array + 2) = param->exception_type[2];
            break;
        }

        case MSG_CODE_RES_NUM_EXCEP:
        {
            param_res_num_excep *param = (param_res_num_excep *)mParam;
            uint32_2_le_chars4(param->cur_number, array);
            uint32_2_le_chars4(param->err_number, array + 4);
            break;
        }

        case MSG_CODE_RES_CODE_EXCEP:
        {
            param_res_code_excep *param = (param_res_code_excep *)mParam;
            uint32_2_le_chars4(param->msg_number, array);
            uint32_2_le_chars4(param->err_code, array + 4);
            break;
        }

        case MSG_CODE_RES_LENG_EXCEP:
        {
            param_res_leng_excep *param = (param_res_leng_excep *)mParam;
            uint32_2_le_chars4(param->msg_number, array);
            uint32_2_le_chars4(param->msg_code, array + 4);
            uint16_2_le_chars2(param->param_size, array + 8);
            uint16_2_le_chars2(param->struct_size, array + 10);
            break;
        }

        case MSG_CODE_RES_ALLOC_VFS:
        {
            param_res_mda_alloc_vfs *param = (param_res_mda_alloc_vfs *)mParam;
            uint16_2_le_chars2(param->number, array);
            break;
        }

        default:
        {
            break;
        }
    }
}

void MtNetMessage::buildPropArray() {
    if (NULL == mProp) {
        return;
    }
    if (NULL == mArray) {
        return;
    }
    char *array = mArray + MESSAGE_HEADER_SIZE + getParamSize();
    memcpy(array, mProp, getPropSize());
}

// ***CMD-RES-EVT-BEGIN***
uint16_t MtNetMessage::caluParamSize() {
    uint16_t s = 0;
    switch (getMsgType()) {
        case MSG_TYPE_CMD:
        {
            s = caluCmdParamSize();
            break;
        }

        case MSG_TYPE_EVT:
        {
            s = caluEvtParamSize();
            break;
        }

        case MSG_TYPE_RES:
        {
            s = caluResParamSize();
            break;
        }

        default:
            break;
    }
    return s;
}

uint16_t MtNetMessage::caluCmdParamSize() {
    uint16_t s = 0;
    switch (getMsgCode()) {
        case MSG_CODE_CMD_ALLOC_VFS:
        {
            s = (uint16_t) sizeof(param_mda_alloc_vfs);
            break;
        }

        case MGS_CODE_CMD_FREE_VFS:
        {
            s = (uint16_t) sizeof(param_mda_free_vfs);
            break;
        }

        case MSG_CODE_CMD_SOURCE_AVAILABLE:
        {
            s = (uint16_t) sizeof(param_mcl_source_available);
            break;
        }

        case MSG_CODE_CMD_SOURCE_UNAVAILABLE:
        {
            s = (uint16_t) sizeof(param_mcl_source_unavailable);
            break;
        }

        case MSG_CODE_CMD_ENABLE_UCAM_SERVICE:
        {
            // s = (uint16_t) sizeof(param_enable_ucam_service);
            break;
        }

        case MSG_CODE_CMD_DISABLE_UCAM_SERVICE:
        {
            // s = (uint16_t) sizeof(param_disable_ucam_service);
            break;
        }

        case MSG_CODE_CMD_START_UCAM_PREVIEW:
        {
            s = (uint16_t) sizeof(param_start_ucam_preview);
            break;
        }

        case MSG_CODE_CMD_STOP_UCAM_PREVIEW:
        {
            s = (uint16_t) sizeof(param_stop_ucam_preview);
            break;
        }

        case MSG_CODE_CMD_ENABLE_IPAV_SERVICE:
        {
            s = (uint16_t) sizeof(param_enable_ipav_service);
            break;
        }

        case MSG_CODE_CMD_DISABLE_IPAV_SERVICE:
        {
            s = (uint16_t) sizeof(param_disable_ipav_service);
            break;
        }

        case MSG_CODE_CMD_START_IPAV_PREVIEW:
        {
            s = (uint16_t) sizeof(param_start_ipav_preview);
            break;
        }

        case MSG_CODE_CMD_STOP_IPAV_PREVIEW:
        {
            s = (uint16_t) sizeof(param_stop_ipav_preview);
            break;
        }

        default:
            break;
    }
    return s;
}

uint16_t MtNetMessage::caluEvtParamSize() {
    uint16_t s = 0;
    switch (getMsgCode()) {
        case MSG_CODE_EVT_SUCCESS:
        {
            s = (uint16_t) sizeof(param_evt_res_success);
            break;
        }

        case MSG_CODE_EVT_FAIL:
        {
            s = (uint16_t) sizeof(param_evt_res_fail);
            break;
        }

        default:
            break;
    }
    return s;
}

uint16_t MtNetMessage::caluResParamSize() {
    uint16_t s = 0;
    switch (getMsgCode()) {
        case MSG_CODE_RES_CONFIRM:
        {
            s = (uint16_t) sizeof(param_res_confirm);
            break;
        }

        case MSG_CODE_RES_SUCCESS:
        {
            s = (uint16_t) sizeof(param_evt_res_success);
            break;
        }

        case MSG_CODE_RES_FAIL:
        {
            s = (uint16_t) sizeof(param_evt_res_fail);
            break;
        }

        case MSG_CODE_RES_EXCEPTION:
        {
            s = (uint16_t) sizeof(param_res_exception);
            break;
        }

        case MSG_CODE_RES_VER_EXCEP:
        {
            s = (uint16_t) sizeof(param_res_ver_excep);
            break;
        }

        case MSG_CODE_RES_TYPE_EXCEP:
        {
            s = (uint16_t) sizeof(param_res_type_excep);
            break;
        }

        case MSG_CODE_RES_NUM_EXCEP:
        {
            s = (uint16_t) sizeof(param_res_num_excep);
            break;
        }

        case MSG_CODE_RES_CODE_EXCEP:
        {
            s = (uint16_t) sizeof(param_res_code_excep);
            break;
        }

        case MSG_CODE_RES_LENG_EXCEP:
        {
            s = (uint16_t) sizeof(param_res_leng_excep);
            break;
        }

        case MSG_CODE_RES_HS_NO_ENOUGH:
        {
            break;
        }

        case MSG_CODE_RES_ALLOC_VFS:
        {
            s = (uint16_t) sizeof(param_res_mda_alloc_vfs);
            break;
        }

        default:
            break;
    }
    return s;    
}

// ***CMD-RES-EVT-END***