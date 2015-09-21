# Copyright 2013-present Barefoot Networks, Inc. 
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


THIS_DIR := $(dir $(lastword $(MAKEFILE_LIST)))
LIB_DIR := $(THIS_DIR)/build/gcc-local/lib

DEBUG := 1

MODULE := indigo
MODULE_BASEDIR := $(THIS_DIR)/submodules/indigo
include $(MODULE_BASEDIR)/init.mk

DEPENDMODULES := indigo OFConnectionManager OFStateManager debug_counter cjson \
    BigList BigHash minimatch murmur slot_allocator Configuration BigRing \
	SocketManager loci timer_wheel OS AIM 

MODULE_DIRS := $(MODULE_BASEDIR)/modules \
    $(MODULE_BASEDIR)/submodules/infra/modules \
    $(MODULE_BASEDIR)/submodules/bigcode/modules

ifndef VERBOSE
    VERBOSE := @
endif

include $(BUILDER)/builder.mk
include $(BUILDER)/dirs.mk
include $(BUILDER)/dependmodules.mk

GLOBAL_CFLAGS += -DINDIGO_LINUX_TIME
GLOBAL_CFLAGS += -DINDIGO_MEM_STDLIB

indigo-lib : libraries 
include $(BUILDER)/targets.mk

