ifeq ($(PARAM_FILE), )
	PARAM_FILE:=./Makefile.param
	include $(PARAM_FILE)
endif

OBJ1 = myucam
OBJ2 = myipav
TASK = $(OBJ1) $(OBJ2)
# TASK = $(OBJ2)

# FFMPEG INCLUDE & LIBRARY
FF_PATH = ffmpeg
FF_INCLUDE = -I$(FF_PATH)/include
FF_LIB_PATH = -L$(FF_PATH)/lib
FF_LIB = -lavdevice -lavformat -lavcodec -lavutil
FF_LDFLAGS = $(FF_INCLUDE) $(FF_LIB_PATH) $(FF_LIB)

# LOG4CPP INCLUDE & LIBRARY
LOG_PATH = log4cpp
LOG_INCLUDE = -I$(LOG_PATH)/include
LOG_LIB_PATH = -L$(LOG_PATH)/lib
LOG_LIB = -llog4cpp
LOG_LDFLAGS = $(LOG_LIB_PATH)

# CUR INCLUDE
CUR_PATH = .
CUR_INCLUDE = -I$(CUR_PATH)

CFLAGS += $(FF_INCLUDE) $(CUR_INCLUDE)

ifeq ($(HICHIP), 3531DV100)
CFLAGS += -DHI3531DV100
endif

MPP_TYPE = $(shell basename `readlink ../../mpp`)
ifeq ($(MPP_TYPE), mpp_single)
    CFLAGS += -DDISABLE_CLEAR_AO
endif

CFLAGS += -DDECODER \
          -DFSC600 \
          -std=c++0x \
          $(LOG_INCLUDE) \
          -DLOG4CPP

CXXFLAGS = $(CFLAGS)
CPPFLAGS = $(CXXFLAGS)

CFILES = 

INTERFACE_DIR = interface
COMMON_DIR = common

UCAM_DIR = ucam

UCAM_CXXFILES = $(COMMON_DIR)/MtNetMessage.cpp \
                $(COMMON_DIR)/RtpOutput.cpp \
                $(INTERFACE_DIR)/IBaseRouter.cpp \
                $(INTERFACE_DIR)/IMediaTransactionUdp.cpp \
                $(INTERFACE_DIR)/IRunnable.cpp \
                $(UCAM_DIR)/FFCamera.cpp \
                $(UCAM_DIR)/ucam_main.cpp \
                $(UCAM_DIR)/UcamMtUdp.cpp \
                $(UCAM_DIR)/UcamRouter.cpp \
                $(UCAM_DIR)/UcamScanner.cpp \
                $(UCAM_DIR)/UcamServer.cpp \
                $(UCAM_DIR)/UcamStream.cpp

IPAV_DIR = ipav

IPAV_CXXFILES = $(COMMON_DIR)/MtNetMessage.cpp \
                $(COMMON_DIR)/RtpOutput.cpp \
                $(INTERFACE_DIR)/IBaseRouter.cpp \
                $(INTERFACE_DIR)/IMediaTransactionUdp.cpp \
                $(INTERFACE_DIR)/IRunnable.cpp \
                $(IPAV_DIR)/ipav_main.cpp \
                $(IPAV_DIR)/IpavInput.cpp \
                $(IPAV_DIR)/IpavMtUdp.cpp \
                $(IPAV_DIR)/IpavRouter.cpp \
                $(IPAV_DIR)/IpavServer.cpp \
                $(IPAV_DIR)/IpavStream.cpp

LDFLAGS = -lm \
          -lpthread \
          -llog4cpp

default: clean
	make install

clean:
	rm -f $(TASK) $(IPAV_CXXFILES:.cpp=.o) $(UCAM_CXXFILES:.cpp=.o)

$(OBJ1): $(UCAM_CXXFILES:.cpp=.o)
	$(CXX) -o $(OBJ1) $(UCAM_CXXFILES:.cpp=.o) $(LDFLAGS) $(FF_LDFLAGS) $(LOG_LIB_PATH)

$(OBJ2): $(IPAV_CXXFILES:.cpp=.o)
	$(CXX) -o $(OBJ2) $(IPAV_CXXFILES:.cpp=.o) $(LDFLAGS) $(LOG_LIB_PATH)

install: $(TASK)
	rm -f $(UCAM_CXXFILES:.cpp=.o) $(IPAV_CXXFILES:.cpp=.o)
	# mkdir -p $(PREFIX_DIR)/usr/local/bin
	# $(STRIP) $(OBJ1)
	# cp $(OBJ1) $(PREFIX_DIR)/usr/local/bin
    # $(STRIP) $(OBJ2)
	# cp $(OBJ2) $(PREFIX_DIR)/usr/local/bin
