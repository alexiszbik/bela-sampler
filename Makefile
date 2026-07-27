# Optional: build from the project directory on the board:
#   make -C /root/Bela/projects/Sampler
#
# Standard Bela build (deploy.sh) uses make -C /root/Bela PROJECT=Sampler and passes
# CPPFLAGS from src_include_paths.mk — see scripts/deploy.sh.

PROJECT ?= Sampler
BELA_DIR ?= /root/Bela

include $(BELA_DIR)/Makefile
include $(abspath $(dir $(lastword $(MAKEFILE_LIST))))/src_include_paths.mk

CPPFLAGS += $(SAMPLER_SRC_CPPFLAGS)
