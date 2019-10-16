/*
 *  UcamMtUdp.cpp
 *
 *  Created on: 2019-05-15
 *      Author: knox
 */

#include "ucam/UcamMtUdp.h"

#include "common/MtConstant.h"

const char *UcamMtUdp::getThreadName() {
    return TNAME_UCAM_NET;
}