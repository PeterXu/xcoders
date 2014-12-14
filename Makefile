TARGET = xcoders
PREFIX_BIN =
FFMPEG_ROOT=ffmpeg

CC = gcc
CXX = g++

# for video and ffmpeg
INCLUDES = -I${FFMPEG_ROOT}/
LIBS = -L${FFMPEG_ROOT}/build/win32/lib -lavcodec -lavutil -lavformat -lswscale
CFLAGS = -DXCODERS_EXPORTS

# for audio
INCLUDES += -Iaudio
LIBS += -Laudio/ilbc -lilbc -Laudio/signal_processing -lsgl
CFLAGS += -DXACODERS_EXPORTS

CFLAGS += -Wall -Werror -O2
ifdef WIN32
LINKFLAGS = -shared -Wl,--output-def,$(TARGET).def,--out-implib,$(TARGET).lib
else
LINKFLAGS = -shared 
endif

C_SOURCES = $(wildcard *.c)
C_OBJS = $(patsubst %.c, %.o, $(C_SOURCES))

CPP_SOURCES = \
	log.cpp \
	ffdecoder.cpp  \
	ffencoder.cpp  \
	ffparam.cpp \
	utils.cpp \
	xacoders.cpp \
	xcoders.cpp
CPP_OBJS = $(patsubst %.cpp, %.o, $(CPP_SOURCES))

.c.o:
	$(CC) -c -o $*.o $(CFLAGS) $(INCLUDES) $*.c
.cpp.o:
	$(CXX) -c -o $*.o $(CFLAGS) $(INCLUDES) $*.cpp

compile: $(CPP_OBJS) $(C_OBJS)
	make -C audio/ilbc
	make -C audio/signal_processing
	$(CXX) $(LINKFLAGS) -o $(TARGET).dll $^ $(LIBS)

clean:
	rm -f $(CPP_OBJS) $(C_OBJS)
	rm -f $(TARGET).dll $(TARGET).lib $(TARGET).def

distclean: clean
	make -C audio/ilbc clean
	make -C audio/signal_processing clean

install: $(TARGET)
	cp -f $(TARGET).dll $(PREFIX_BIN)

uninstall:
	rm -f $(PREFIX)/$(PREFIX_BIN)

rebuild: clean compile 
