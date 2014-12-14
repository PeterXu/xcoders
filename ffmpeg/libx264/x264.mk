# LOCAL_PATH is libx264

OS := Android
include $(LOCAL_PATH)/config-$(TARGET_ARCH).mak
include $(LOCAL_PATH)/config.mak

X264OBJS :=
include $(LOCAL_PATH)/Makefile

# collect objects
X264OBJS += $(OBJS) $(OBJASM) $(OBJSO)

X264NAME := lib$(NAME)
X264LIBS := $(foreach,NAME,$(X264LIBS),lib$(NAME))

# for LOCAL_CFLAGS
X264CFLAGS  = -O3 -fno-fast-math  -Wall -std=gnu99
X264CFLAGS += -fPIC -fomit-frame-pointer -s -fno-tree-vectorize
X264CFLAGS += -DTARGET_CONFIG=\"config-$(TARGET_ARCH).h\"
X264CFLAGS += -mcpu=cortex-a8 -mfpu=neon
X264CFLAGS += -DPIC -DHIGH_BIT_DEPTH=0 -DBIT_DEPTH=8

ALL_S_FILES := $(wildcard $(LOCAL_PATH)/common/$(TARGET_ARCH)/*.S)
ALL_S_FILES := $(addprefix common/$(TARGET_ARCH)/, $(notdir $(ALL_S_FILES)))

ifneq ($(ALL_S_FILES),)
ALL_S_OBJS := $(patsubst %.S,%.o,$(ALL_S_FILES))
C_OBJS := $(filter-out $(ALL_S_OBJS),$(X264OBJS))
S_OBJS := $(filter $(ALL_S_OBJS),$(X264OBJS))
else
C_OBJS := $(X264OBJS)
S_OBJS :=
endif

C_FILES := $(patsubst %.o,%.c,$(C_OBJS))
S_FILES := $(patsubst %.o,%.S,$(S_OBJS))

X264FILES := $(sort $(S_FILES)) $(sort $(C_FILES))
