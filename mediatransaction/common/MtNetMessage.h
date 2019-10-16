/*
 *  MtNetMessage.h
 *
 *  Created on: 2019-05-15
 *      Author: knox
 */

#ifndef MT_NET_MESSAGE_H_
#define MT_NET_MESSAGE_H_

/**
 * Add new CMD or RES or EVT
 * 
 * should modify where mark between ***CMD-RES-EVT-BEGIN*** and ***CMD-RES-EVT-END***
 **/

#include <stdio.h>
#include <stdint.h>

#define MESSAGE_HEADER_SIZE                    18

// ERROR CODE
#define MALLOC_ERROR                           10
#define MAGIC_CODE_ERROR                       11
#define MESSAGE_TYPE_ERROR                     12
#define MESSAGE_CODE_ERROR                     13
#define MESSAGE_LENG_ERROR                     14
#define HEADER_SIZE_NO_ENOUGH_ERROR            15
#define HEADER_NULL_ERROR                      16
#define PARAM_NULL_ERROR                       17
#define INPUT_NULL_ERROR                       18
#define VERSION_NUM_ERROR                      19

// MODULE
#define MODULE_CODE_OSD                        1
#define MODULE_CODE_MED                        2
#define MODULE_CODE_UPG                        3
#define MODULE_CODE_MCL                        4

#define MSG_VERSION_NUMBER                     0

static uint32_t meSequentNumber = 0;
static uint32_t getSequentNumber() {
    return (meSequentNumber++);
}

typedef struct {
    char magicCode[4];      // 4 0x55 0x66 0x77 0x88
    char version;           // 1
    char msgType[3];        // 3 "CMD" "EVT" "RES"
    uint32_t msgNumber;     // 4
    uint32_t msgCode;       // 4
    uint16_t paramSize;     // 2
} NetMsgHeader;

// ***CMD-RES-EVT-BEGIN***
// reponse & event-----------------

typedef struct {
    uint32_t msg_number;
    uint32_t msg_code;
} param_res_confirm;                           // 8

typedef struct {
    uint32_t msg_number;
    uint32_t msg_code;
} param_evt_res_success;                       // 8

typedef struct {
    uint32_t msg_number;
    uint32_t msg_code;
    uint32_t module_number;
} param_evt_res_fail;                          // 12

typedef struct {
    char magic_code[4];
} param_res_exception;                         // 4

typedef struct {
    char supported_version;
    char exception_version;
} param_res_ver_excep;                         // 2

typedef struct {
    char exception_type[3]; 
} param_res_type_excep;                        // 3

typedef struct {
    uint32_t cur_number;
    uint32_t err_number;
} param_res_num_excep;                         // 8

typedef struct {
    uint32_t msg_number;
    uint32_t err_code;
} param_res_code_excep;                        // 8

typedef struct {
    uint32_t msg_number;
    uint32_t msg_code;
    uint16_t param_size;
    uint16_t struct_size;
} param_res_leng_excep;                        // 12

// media
typedef struct {
    uint16_t number;
    uint16_t sock_port;
    uint32_t shm_handle;
} param_res_mda_alloc_vfs;                     // 8

// cmd-----------------------------

// master controller
typedef struct {
    uint16_t number;
    uint16_t prop_size;
} param_mcl_source_available;                  // 4

typedef struct {
    uint16_t number;
} param_mcl_source_unavailable;                // 2

// usb camera
typedef struct {

} param_enable_ucam_service;                   // 0

typedef struct {

} param_disable_ucam_service;                  // 0

typedef struct {
    uint16_t number;
} param_start_ucam_preview;                    // 2

typedef struct {
    uint16_t number;
} param_stop_ucam_preview;                     // 2

// ipav
typedef struct {
    uint16_t number;
    uint8_t mac_addr[6];
} param_enable_ipav_service;                   // 8

typedef struct {
    uint16_t number;
} param_disable_ipav_service;                  // 2

typedef struct {
    uint16_t number;
} param_start_ipav_preview;                    // 2

typedef struct {
    uint16_t number;
} param_stop_ipav_preview;                     // 2

// media
typedef struct {
    uint16_t number;
    uint16_t video_type;                       // 0x21 H264 over TS, 0x64 MJPEG, 0x65 H264
    uint16_t width;
    uint16_t height;
    uint16_t mode;
    uint16_t audio_type;
} param_mda_alloc_vfs;                         // 12

typedef struct {
    uint16_t number;
} param_mda_free_vfs;                          // 2

// ***CMD-RES-EVT-END***
//---------------------------------

typedef enum {
    MSG_TYPE_NIL,
    MSG_TYPE_CMD,
    MSG_TYPE_EVT,
    MSG_TYPE_RES,
} MsgType;

// ***CMD-RES-EVT-BEGIN***
typedef enum {
    MSG_CODE_NIL                             = 0xFFFFFFFF,

    // CMD
    // 0x0002 ==> media
    MSG_CODE_CMD_ALLOC_VFS                   = 0x0002000D,
    MGS_CODE_CMD_FREE_VFS                    = 0x0002000F,
    // 0x0004 ==> master controller
    MSG_CODE_CMD_SOURCE_AVAILABLE            = 0x00040B03,
    MSG_CODE_CMD_SOURCE_UNAVAILABLE          = 0x00040B04,
    // 0x0005 ==> usb camera
    MSG_CODE_CMD_ENABLE_UCAM_SERVICE         = 0x00050001,
    MSG_CODE_CMD_DISABLE_UCAM_SERVICE        = 0x00050002,
    MSG_CODE_CMD_START_UCAM_PREVIEW          = 0x00050003,
    MSG_CODE_CMD_STOP_UCAM_PREVIEW           = 0x00050004,
    // 0x0005 ==> ipav
    MSG_CODE_CMD_ENABLE_IPAV_SERVICE         = 0x00050011,
    MSG_CODE_CMD_DISABLE_IPAV_SERVICE        = 0x00050012,
    MSG_CODE_CMD_START_IPAV_PREVIEW          = 0x00050013,
    MSG_CODE_CMD_STOP_IPAV_PREVIEW           = 0x00050014,

    // EVT
    // 0x0000 ==> common
    MSG_CODE_EVT_SUCCESS                     = 0x00000001,
    MSG_CODE_EVT_FAIL                        = 0x00000002,

    // RES
    // 0x0000 ==> common
    MSG_CODE_RES_CONFIRM                     = 0x00000000,
    MSG_CODE_RES_SUCCESS                     = 0x00000001,
    MSG_CODE_RES_FAIL                        = 0x00000002,
    MSG_CODE_RES_EXCEPTION                   = 0x00000003,
    MSG_CODE_RES_VER_EXCEP                   = 0x00000004,
    MSG_CODE_RES_TYPE_EXCEP                  = 0x00000005,
    MSG_CODE_RES_NUM_EXCEP                   = 0x00000006,
    MSG_CODE_RES_CODE_EXCEP                  = 0x00000007,
    MSG_CODE_RES_LENG_EXCEP                  = 0x00000008,

    MSG_CODE_RES_HS_NO_ENOUGH                = 0x00000100,
    // 0x0002 ==> media
    MSG_CODE_RES_ALLOC_VFS                   = 0x0002000E,
} MsgCode;
// ***CMD-RES-EVT-END***

class MtNetMessage {
public:
    // malloc header, param, prop
    static uint32_t messageMalloc(MtNetMessage **ppMessage, char *array, int len);
    // free header, param, prop, array
    static void messageFree(MtNetMessage *pMessage);

    static MtNetMessage *createObject();
    static void releaseObject(MtNetMessage *pMessage);

    NetMsgHeader *makeHeader(MsgType type, MsgCode code);
    NetMsgHeader *getHeader();
    void *makeEmptyParam();
    void *getParam();
    uint32_t makeProp(char *buffer, int len);
    // malloc array
    uint32_t makeArray();
    char *getArray();
    int getArrayLength();

    int getSupportedVersion();
    uint16_t getParamStructSize();

    MsgCode getMsgCode();
    MsgType getMsgType();
    uint16_t mCltPort;

    void printHeader();

private:
    MtNetMessage() : mHeader(NULL), mParam(NULL), mProp(NULL), mType(MSG_TYPE_NIL), mCode(MSG_CODE_NIL), mCltPort(0), mArray(NULL), mArrayLen(0) {}
    ~MtNetMessage() {}

    // malloc header
    void mallocMsgHeader();
    void buildMsgHeader(char *arrayOffsetHeader);

    // malloc param
    void mallocMsgParam();
    // ***CMD-RES-EVT-BEGIN***
    void buildMsgParam(char *arrayOffsetParam);
    void buildMsgCmdParam(char *arrayOffsetParam);
    void buildMsgEvtParam(char *arrayOffsetParam);
    void buildMsgResParam(char *arrayOffsetParam);
    // ***CMD-RES-EVT-END***

    // malloc prop
    void mallocMsgProp();
    void buildMsgProp(char *arrayOffsetProp);

    void freeArray();
    void freeProp();
    void freeParam();
    void freeHeader();

    int getParamSize();
    int getPropSize();

    void setMsgType1();
    // ***CMD-RES-EVT-BEGIN***
    void setMsgCode2();
    void setMsgCmdCode();
    void setMsgEvtCode();
    void setMsgResCode();
    // ***CMD-RES-EVT-END***

    void buildHeaderArray();
    // ***CMD-RES-EVT-BEGIN***
    void buildParamArray();
    void buildCmdParamArray(char *array);
    void buildEvtParamArray(char *array);
    void buildResParamArray(char *array);
    // ***CMD-RES-EVT-END***
    void buildPropArray();

    uint16_t caluParamSize();
    uint16_t caluCmdParamSize();
    uint16_t caluEvtParamSize();
    uint16_t caluResParamSize();
    
    NetMsgHeader *mHeader;  // 18
    void *mParam;
    char *mProp;
    MsgType mType;
    MsgCode mCode;
    char *mArray;
    int mArrayLen;
};

#endif // MT_NET_MESSAGE_H_