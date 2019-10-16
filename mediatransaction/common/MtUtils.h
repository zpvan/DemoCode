/*
 *  MtUtils.h
 *
 *  Created on: 2019-05-15
 *      Author: knox
 */

#ifndef MT_UTILS_H_
#define MT_UTILS_H_

#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include <stdint.h>

inline bool sys_big_endian() {
    return (*(uint16_t *)"\0\xff" < 0x100);
}

// uint16_t to char[], little endian
inline void uint16_2_le_chars2(uint16_t num, char *bytes) {
    if (sys_big_endian()) {
        bytes[0] = *(((char *) &num) + 1);
        bytes[1] = *(((char *) &num) + 0);
    } else {
        bytes[0] = *(((char *) &num) + 0);
        bytes[1] = *(((char *) &num) + 1);
    }
}

// uint32_t to char[], little endian
inline void uint32_2_le_chars4(uint32_t num, char *bytes) {
    if (sys_big_endian()) {
        bytes[0] = *(((char *) &num) + 3);
        bytes[1] = *(((char *) &num) + 2);
        bytes[2] = *(((char *) &num) + 1);
        bytes[3] = *(((char *) &num) + 0);
    } else {
        bytes[0] = *(((char *) &num) + 0);
        bytes[1] = *(((char *) &num) + 1);
        bytes[2] = *(((char *) &num) + 2);
        bytes[3] = *(((char *) &num) + 3);
    }
}

// uint16_t to char[], big endian
inline void uint16_2_be_chars2(uint16_t num, char *bytes) {
    // always big endian
    num = htons(num);

    bytes[0] = *(((char *) &num) + 0);
    bytes[1] = *(((char *) &num) + 1);
}

// uint32_t to char[], big endian
inline void *uint32_2_be_chars(uint32_t num, char *bytes) {
    // always big endian
    num = htonl(num);

    bytes[0] = *(((char *) &num) + 0);
    bytes[1] = *(((char *) &num) + 1);
    bytes[2] = *(((char *) &num) + 2);
    bytes[3] = *(((char *) &num) + 3);
}

//--------------------------------------------

// char[] to uint16_t, little endian
inline uint16_t le_chars_2_uint16(char *bytes) {
    uint16_t num = 0;
    if (sys_big_endian()) {
        *(((char *) &num) + 0) = bytes[1];
        *(((char *) &num) + 1) = bytes[0];
    } else {
        *(((char *) &num) + 0) = bytes[0];
        *(((char *) &num) + 1) = bytes[1];
    }
    return num;
}

// char[] to uint32_t, little endian
inline uint32_t le_chars_2_uint32(char *bytes) {
    uint32_t num = 0;
    if (sys_big_endian()) {
        *(((char *) &num) + 0) = bytes[3];
        *(((char *) &num) + 1) = bytes[2];
        *(((char *) &num) + 2) = bytes[1];
        *(((char *) &num) + 3) = bytes[0];
    } else {
        *(((char *) &num) + 0) = bytes[0];
        *(((char *) &num) + 1) = bytes[1];
        *(((char *) &num) + 2) = bytes[2];
        *(((char *) &num) + 3) = bytes[3];
    }
    return num;
}

// char[] to uint16_t, big endian
inline uint16_t be_chars_2_uint16(char *bytes) {
    uint16_t num = 0;
    *(((char *) &num) + 0) = bytes[0];
    *(((char *) &num) + 1) = bytes[1];
    num = ntohs(num);
    return num;
}

// char[] to uint32_t, big endian
inline uint32_t be_chars_2_uint32(char *bytes) {
    uint32_t num = 0;
    *(((char *) &num) + 0) = bytes[0];
    *(((char *) &num) + 1) = bytes[1];
    *(((char *) &num) + 2) = bytes[2];
    *(((char *) &num) + 3) = bytes[3];
    num = ntohl(num);
    return num;
}

//--------------------------------------------

static void dump_hex(const char *tag, char *buf, int len) {
    int i;
    char hex[len * 5];
    memset(hex, 0, len * 5);
    for (i = 0; i < len; i++) {
        if (i == 0) {
            sprintf(hex, "0x%02x", (unsigned char)buf[i]);
        } else {
            sprintf(hex, "%s 0x%02x", hex, (unsigned char)buf[i]);
        }
    }
    LOG_DUMP("%s: %s", tag, hex);
}

//--------------------------------------------

inline void set_uint8(char *&buffer, uint8_t value) {
    *buffer = value;
    buffer += 1;
}

inline void set_uint16(char *&buffer, uint16_t value) {
    value = htons(value);
    memcpy(buffer, &value, 2);
    buffer += 2;
}

inline void set_uint32(char *&buffer, uint32_t value) {
    value = htonl(value);
    memcpy(buffer, &value, 4);
    buffer += 4;
}

inline void set_uint64(char *&buffer, uint64_t value) {
    uint64_t network_order;
    if (sys_big_endian()) {
        network_order = value;
    } else {
        network_order = (uint64_t) (  (uint64_t)  (value << 56) | (uint64_t)  (((uint64_t) 0x00ff0000 << 32) & (value << 40))
        | (uint64_t)  ( ((uint64_t)  0x0000ff00 << 32) & (value << 24)) | (uint64_t)  (((uint64_t)  0x000000ff << 32) & (value << 8))
        | (uint64_t)  ( ((uint64_t)  0x00ff0000 << 8) & (value >> 8)) | (uint64_t)     ((uint64_t)  0x00ff0000 & (value >> 24))
        | (uint64_t)  (  (uint64_t)  0x0000ff00 & (value >> 40)) | (uint64_t)  ((uint64_t)  0x00ff & (value >> 56)) );
    }
    memcpy(buffer, &value, 8);
    buffer += 8;
}

//---------------------------------------------

#endif // MT_UTILS_H_