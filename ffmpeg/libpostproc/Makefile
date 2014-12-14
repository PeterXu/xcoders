ifneq ($(OS),Android)
include $(SUBDIR)../config.mak
endif

NAME = postproc
FFLIBS = avutil

HEADERS = postprocess.h

OBJS = postprocess.o

ifneq ($(OS),Android)
include $(SUBDIR)../subdir.mak
endif
