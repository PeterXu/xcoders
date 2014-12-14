xcoders
=======

xcoders: for audio/video encoder/decoder



build for win32
==============
1. prepare
(a) install vs2008
(b) install cygwin: http://ftp.mozilla.org/pub/mozilla.org/mozilla/libraries/win32/MozillaBuildSetup-1.5.1.exe
(c) run MozillaBuild/start-msvc9.bat

2. source code
(a) decompress xcoders into c:\workspace
    uri: https://github.com/PeterXu/xcoders/archive/master.zip

(b) decompress ffmpeg into c:\workspace\xcoders
    uri: https://github.com/PeterXu/ffmpeg/archive/master.zip

3. compile
Run MozillaBuild/start-msvc9.bat
(a) compile libx264
cd /c/workspace/xcoders/ffmpeg/libx264
sh run_cfg_mingw.sh
make

(b) compile ffmpeg
cd /c/workspace/xcoders/ffmpeg
sh run_cfg_mingw.sh
make

(c) compile xcoders
cd /c/workspace/xcoders
make

