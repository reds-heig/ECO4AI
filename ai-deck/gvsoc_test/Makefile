# Copyright (C) 2017 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

ifndef GAP_SDK_HOME
  $(error Source sourceme in gap_sdk first)
endif
MODEL_PREFIX = e4a_collision


# Set variables
# io=host
io=uart
PMSIS_OS = freertos

APP_CFLAGS += -DMODEL_QUANTIZED

QUANT_BITS=8

# load the model pre-quantized by TensorFlow
# if set to false, will quantize using images from the /samples folder
MODEL_PREQUANTIZED = false

NNTOOL_SCRIPT=models/nntool_scripts/nntool_chevalet

# TRAINED_MODEL=models/WINSTON.onnx
TRAINED_MODEL=models/BOURRICOT.onnx

CORE=8

BUILD_DIR=BUILD
ifeq ($(QUANT_BITS),8)
  MODEL_SUFFIX = _SQ8BIT
  MODEL_SQ8=1
else
  MODEL_POW2=1
  MODEL_SUFFIX = _16BIT
endif
# MODEL_FP16=1
ONNX=1

# Test Image
# IMAGE=images/frame_1.ppm

# Model Informations for C Code
AT_INPUT_WIDTH=200
AT_INPUT_HEIGHT=200
AT_INPUT_COLORS=1
AT_CONSTRUCT = $(MODEL_PREFIX)CNN_Construct
AT_DESTRUCT = $(MODEL_PREFIX)CNN_Destruct
AT_CNN = $(MODEL_PREFIX)CNN
AT_L2_MEM_ADDR = $(MODEL_PREFIX)_L2_Memory
AT_L1_MEM_ADDR = $(MODEL_PREFIX)_L1_Memory
AT_L3_RAM_ADDR = $(MODEL_PREFIX)_L3_Memory
AT_L3_ADDR = $(MODEL_PREFIX)_L3_Flash

FLASH_TYPE ?= HYPER
RAM_TYPE   ?= HYPER
MODEL_SEC_L3_FLASH=

ifeq '$(FLASH_TYPE)' 'HYPER'
  MODEL_L3_FLASH=AT_MEM_L3_HFLASH
  MODEL_L3_CONST=hflash
else ifeq '$(FLASH_TYPE)' 'DEFAULT'
  MODEL_L3_FLASH=AT_MEM_L3_DEFAULTFLASH
endif

ifeq '$(RAM_TYPE)' 'HYPER'
  MODEL_L3_RAM=AT_MEM_L3_HRAM
  MODEL_L3_EXEC=hram
else ifeq '$(RAM_TYPE)' 'DEFAULT'
  MODEL_L3_RAM=AT_MEM_L3_DEFAULTRAM
endif

MODEL_INPUT=l2

# Use JPEG ENCODER
CONFIG_GAP_LIB_JPEG = 1

$(info Building GAP8 mode with $(QUANT_BITS) bit quantization)

# MODEL_SUFFIX = _FP16

include model_decl.mk

CLUSTER_STACK_SIZE=2048
CLUSTER_SLAVE_STACK_SIZE=2048
TOTAL_STACK_SIZE = $(shell expr $(CLUSTER_STACK_SIZE) \+ $(CLUSTER_SLAVE_STACK_SIZE) \* 7)

MODEL_L1_MEMORY=$(shell expr 60000 \- $(TOTAL_STACK_SIZE))
MODEL_L2_MEMORY=300000
MODEL_L3_MEMORY=8000000
FREQ_CL=170 # max 170
FREQ_FC=250

CPX_TXQ_SIZE=1
CPX_RXQ_SIZE=1


pulpChip = GAP
PULP_APP = e4a_collision
USE_PMSIS_BSP=1

APP = e4a_collision
APP_SRCS += $(MODEL_GEN_C) $(MODEL_EXPRESSIONS) $(MODEL_COMMON_SRCS) $(CNN_LIB)
APP_SRCS += e4a_collision.c lib/cpx/src/com.c lib/cpx/src/cpx.c

APP_CFLAGS += -gdwarf-2 -g -O3 -mno-memcpy -fno-tree-loop-distribute-patterns -fstack-usage -fno-exceptions
APP_CFLAGS += -DNUM_CORES=$(CORE)
# APP_CFLAGS += -I. -I$(GAP_SDK_HOME)/utils/power_meas_utils -I$(MODEL_HEADERS)
APP_CFLAGS += -I. -I$(MODEL_COMMON_INC) -I$(TILER_EMU_INC) -I$(TILER_INC) $(CNN_LIB_INCLUDE) -I$(realpath $(MODEL_BUILD))
APP_CFLAGS += $(MODEL_SIZE_CFLAGS)
APP_CFLAGS += -DAT_IMAGE=$(IMAGE) -DPERF -DAT_MODEL_PREFIX=$(MODEL_PREFIX) $(MODEL_SIZE_CFLAGS)
APP_CFLAGS += -DSTACK_SIZE=$(CLUSTER_STACK_SIZE) -DSLAVE_STACK_SIZE=$(CLUSTER_SLAVE_STACK_SIZE)
APP_CFLAGS += -DconfigUSE_TIMERS=1 -DINCLUDE_xTimerPendFunctionCall=1 -DFS_PARTITIONTABLE_OFFSET=0x40000
APP_CFLAGS +=  -DFREQ_FC=$(FREQ_FC) -DFREQ_CL=$(FREQ_CL) -DTXQ_SIZE=$(CPX_TXQ_SIZE) -DRXQ_SIZE=$(CPX_RXQ_SIZE) 
APP_CFLAGS += -DAT_CONSTRUCT=$(AT_CONSTRUCT) -DAT_DESTRUCT=$(AT_DESTRUCT) -DAT_CNN=$(AT_CNN) -DAT_L3_ADDR=$(AT_L3_ADDR) -DCURR_DIR=$(CURDIR)
APP_INC = lib/cpx/inc
APP_CFLAGS += -DAIDECK -DTRAINED_MODEL=$(TRAINED_MODEL)

ifneq '$(platform)' 'gvsoc'
ifdef MEAS
APP_CFLAGS += -DGPIO_MEAS
endif
else
APP_CFLAGS += -DGVSOC
endif


READFS_FILES=$(abspath $(MODEL_TENSORS))

all:: model

clean:: clean_model

include model_rules.mk
$(info APP_SRCS... $(APP_SRCS))
$(info APP_CFLAGS... $(APP_CFLAGS))
RUNNER_CONFIG = $(CURDIR)/config.ini
include $(RULES_DIR)/pmsis_rules.mk

