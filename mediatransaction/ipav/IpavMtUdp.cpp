/*
 *  IpavMtUdp.cpp
 *
 *  Created on: 2019-05-15
 *      Author: knox
 */

#include "ipav/IpavMtUdp.h"

#include "common/MtConstant.h"

const char *IpavMtUdp::getThreadName() {
    return TNAME_IPAV_NET;
}