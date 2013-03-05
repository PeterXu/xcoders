TARGET := libyuv
SUFFIX := a
LIBDIRS :=
LIBS := 

OS=
ARCH=
ARCH_ABI=

ifeq ($(OS),Android)
HOST := /opt/arm-linux-androideabi-4.6/bin/arm-linux-androideabi-
else
HOST :=
endif

CC := $(HOST)g++
CXX := $(CC)
AR := $(HOST)ar crs


RM := rm -f
PS := cc

CPPFLAGS = -g -Wall -O3 #-march=i486
CPPFLAGS += $(addprefix -I,$(INCLUDES))
CPPFLAGS += -MMD
CPPFLAGS += 

LDFLAGS := -fPIC
LDFLAGS += $(addprefix -L,$(LIBDIRS)) $(addprefix -l,$(LIBS))

LOCAL_PATH := .

#
# include headers
#

INCLUDES := . \
    $(LOCAL_PATH)/include


#
# source files
#

#SOURCE := $(wildcard *.$(PS))
SOURCE := \
    source/compare.cc           \
    source/compare_common.cc    \
    source/compare_posix.cc     \
    source/convert.cc           \
    source/convert_argb.cc      \
    source/convert_from.cc      \
    source/convert_from_argb.cc \
    source/cpu_id.cc            \
    source/format_conversion.cc \
    source/planar_functions.cc  \
    source/rotate.cc            \
    source/rotate_argb.cc       \
    source/row_any.cc           \
    source/row_common.cc        \
    source/row_mips.cc          \
    source/row_posix.cc         \
    source/scale.cc             \
    source/scale_argb.cc        \
    source/scale_mips.cc        \
    source/video_common.cc      \

# TODO(fbarchard): Enable mjpeg encoder.
#   source/mjpeg_decoder.cc
ifeq ($(ARCH_ABI),armeabi-v7a)
CPPFLAGS += -DLIBYUV_NEON
SOURCE += \
        source/compare_neon.cc.neon \
        source/rotate_neon.cc.neon  \
        source/row_neon.cc.neon     \
        source/scale_neon.cc.neon   \
        source/scale_neon_argb.cc.neon
endif

#
# objects and build
#

OBJS := $(patsubst %.$(PS),%.o,$(SOURCE))
DEPS := $(patsubst %.o,%.d,$(OBJS))
MISSING_DEPS := $(filter-out $(wildcard $(DEPS)),$(DEPS))
MISSING_DEPS_SOURCES := $(wildcard $(patsubst %.d,%.$(PS),$(MISSING_DEPS)))

.PHONY : all deps objs clean distclean rebuild

all : $(TARGET)

deps : $(DEPS)
	$(CC) -MM -MMD $(SOURCE)


objs : $(OBJS)

clean :
	@$(RM) $(OBJS)
	@$(RM) $(DEPS)

distclean : clean
	@$(RM) $(TARGET)

rebuild: distclean all

ifneq ($(MISSING_DEPS),)
$(MISSING_DEPS) :
	@$(RM) $(patsubst %.d,%.o,$@)
endif

-include $(DEPS)

#
# generate target
#

ifeq ($(SUFFIX),so)
$(TARGET) : $(OBJS)
	$(CC) -shared -o $(TARGET).so $(OBJS) $(LDFLAGS) 
else
ifeq ($(SUFFIX),a)
$(TARGET) : $(OBJS)
	$(AR) $(TARGET).a $(OBJS) 
else
$(TARGET) : $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LDFLAGS) 
endif
endif

