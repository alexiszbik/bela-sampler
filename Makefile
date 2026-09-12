# Bela build (deploy.sh): make -C /root/Bela PROJECT=Sampler
# Optional local build on the board from this folder:
#   make -C /root/Bela/projects/Sampler -f Makefile.project

PROJECT ?= Sampler
BELA_DIR ?= /root/Bela

include $(abspath $(dir $(lastword $(MAKEFILE_LIST))))/Makefile.project
