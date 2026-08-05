# Header search paths for src/ (used by Makefile; deploy.sh mirrors this list).
# Paths are relative to the project root on the board, e.g. /root/Bela/projects/Sampler.

SAMPLER_SRC_INCLUDE_DIRS := program playback playback/sample engine mix dsp dsp/delay midi

SAMPLER_SRC_CPPFLAGS := $(addprefix -Isrc/,$(SAMPLER_SRC_INCLUDE_DIRS))

define SAMPLER_CPPFLAGS_FOR_PROJECT
$(addprefix -I$(1)/src/,$(SAMPLER_SRC_INCLUDE_DIRS))
endef
