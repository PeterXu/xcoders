# LOCAL_PATH is one of libavutil, libavcodec, libavformat, or libswscale

OS := Android
include $(LOCAL_PATH)/../config-$(TARGET_ARCH).mak
include $(LOCAL_PATH)/../config.mak

OBJS :=
OBJS-yes :=
MMX-OBJS-yes :=
include $(LOCAL_PATH)/Makefile

# collect objects
OBJS-$(HAVE_MMX) += $(MMX-OBJS-yes)
OBJS += $(OBJS-yes)

FFNAME := lib$(NAME)
FFLIBS := $(foreach,NAME,$(FFLIBS),lib$(NAME))

# for LOCAL_CPPFLAGS
FFCPPFLAGS = -DHAVE_AV_CONFIG_H -Wno-sign-compare -Wno-switch -Wno-pointer-sign
FFCPPFLAGS += -D_ISOC99_SOURCE -D_POSIX_C_SOURCE=200112 -D_FILE_OFFSET_BITS=32 -D_LARGEFILE_SOURCE -DPIC

# for LOCAL_CFLAGS
FFCFLAGS  = -DHAVE_AV_CONFIG_H -Wno-sign-compare -Wno-switch -Wno-pointer-sign
FFCFLAGS += -DTARGET_CONFIG=\"config-$(TARGET_ARCH).h\"
FFCFLAGS += -std=c99 -fPIC -pthread -Wdeclaration-after-statement -Wall -Wno-switch -Wdisabled-optimization -Wpointer-arith -Wredundant-decls -Wno-pointer-sign -Wcast-qual -Wwrite-strings -Wundef -fno-math-errno -fno-tree-vectorize

ALL_S_FILES := $(wildcard $(LOCAL_PATH)/$(TARGET_ARCH)/*.S)
ALL_S_FILES := $(addprefix $(TARGET_ARCH)/, $(notdir $(ALL_S_FILES)))

ifneq ($(ALL_S_FILES),)
ALL_S_OBJS := $(patsubst %.S,%.o,$(ALL_S_FILES))
C_OBJS := $(filter-out $(ALL_S_OBJS),$(OBJS))
S_OBJS := $(filter $(ALL_S_OBJS),$(OBJS))
else
C_OBJS := $(OBJS)
S_OBJS :=
endif

C_FILES := $(patsubst %.o,%.c,$(C_OBJS))
S_FILES := $(patsubst %.o,%.S,$(S_OBJS))

FFFILES := $(sort $(S_FILES)) $(sort $(C_FILES))
