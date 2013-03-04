TARGET = xcoders.dll
PREFIX_BIN =
IVYFFMPEG_ROOT=../IvyVideo/trunk/ivyffmpeg

CC = gcc
CXX = g++
INCLUDES = -I${IVYFFMPEG_ROOT}/ffmpeg/
LIBS = -L${IVYFFMPEG_ROOT}/ffmpeg/build/win32/lib -lavcodec -lavutil -lavformat -lswscale 
CFLAGS = -Wall -Werror -DXCODERS_EXPORTS
LINKFLAGS = -shared -Wl,--export-all-symbols,--output-def,xcoders.def,--out-implib,xcoders.lib

C_SOURCES = $(wildcard *.c)
C_OBJS = $(patsubst %.c, %.o, $(C_SOURCES))

CPP_SOURCES = LogTrace.cpp \
	FFmpegDecoder.cpp  \
	FFmpegEncoder.cpp  \
	FFmpegVideoParam.cpp \
	FFmpegAudioParam.cpp \
	xcoders.cpp
CPP_OBJS = $(patsubst %.cpp, %.o, $(CPP_SOURCES))

.c.o:
	$(CC) -c -o $*.o $(CFLAGS) $(INCLUDES) $*.c
.cpp.o:
	$(CXX) -c -o $*.o $(CFLAGS) $(INCLUDES) $*.cpp

compile: $(CPP_OBJS) $(C_OBJS)
	$(CXX) $(LINKFLAGS) -o $(TARGET) $^ $(LIBS)

clean:
	rm -f $(CPP_OBJS) $(C_OBJS)
	rm -f $(TARGET)

install: $(TARGET)
	cp $(TARGET) $(PREFIX_BIN)

uninstall:
	rm -f $(PREFIX)/$(PREFIX_BIN)

rebuild: clean compile 
