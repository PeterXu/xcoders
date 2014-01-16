xcoder build instructions
=========================

1. download

http://ftp.mozilla.org/pub/mozilla.org/mozilla/libraries/win32/MozillaBuildSetup-1.5.1.exe

Then install and start it

2. unzip sources

mkdir workspace

unzip ivyffmpeg.zip into "workspace/IvyVideo/trunk"

unzip xcoders.zip into "workspace/"

3. build x264
cd workspace/IvyVideo/trunk/ivyffmpeg/ffmpeg/libx264

sh run_cfg_mingw.sh

make

4. build ffmpeg

cd workspace/IvyVideo/trunk/ivyffmpeg/ffmpeg

sh run_cfg_mingw.sh

make

5. build xcoders

cd workspace/xcoders

make

6. collect all dlls into the same directory

(1). env dlls(built-in)

libgcc_s_dw2-1.dll, libstdc++-6.dll, libpthread-2.dll

(2). target dlls

workspace/xcoders/xcoders.dll

workspace/IvyVideo/trunk/ivyffmpeg/ffmpeg/libavcodec/avcodec.dll

workspace/IvyVideo/trunk/ivyffmpeg/ffmpeg/libavformat/avformat-52.dll

workspace/IvyVideo/trunk/ivyffmpeg/ffmpeg/libavutil/avutil-50.dll

workspace/IvyVideo/trunk/ivyffmpeg/ffmpeg/libswscale/swscale-0.dll

workspace/IvyVideo/trunk/ivyffmpeg/ffmpeg/libx264/libx264-125.dll

