xcoder build instructions
=========================

1. 配置环境
(a) 安装vs2008
(b) 安装cygwin: http://ftp.mozilla.org/pub/mozilla.org/mozilla/libraries/win32/MozillaBuildSetup-1.5.1.exe
(c) 运行MozillaBuild安装目录下的start-msvc9.bat，将会成功启动一个命令行

2. 代码
(a) 解压缩IvyVideo到c:\workspace
(b) 解压缩xcoders到c:\workspace

3. 编译
启动MozillaBuild下的start-msvc9.bat命令行，进行代码编译
(a) 编译libx264
cd /c/workspace/IvyVideo/trunk/ivyffmpeg/ffmpeg/libx264
sh run_cfg_mingw.sh
make

(b) 编译ffmpeg
cd /c/workspace/IvyVideo/trunk/ivyffmpeg/ffmpeg
sh run_cfg_mingw.sh
make

(c) 编译xcoders
cd workspace/xcoders
make

4. 收集动态库

(a). 系统dlls(在xcoders里面)
libgcc_s_dw2-1.dll, libstdc++-6.dll, libpthread-2.dll

(2). 编译生成的dlls所在目录
workspace/xcoders/xcoders.dll
workspace/IvyVideo/trunk/ivyffmpeg/ffmpeg/libavcodec/avcodec.dll
workspace/IvyVideo/trunk/ivyffmpeg/ffmpeg/libavformat/avformat-52.dll
workspace/IvyVideo/trunk/ivyffmpeg/ffmpeg/libavutil/avutil-50.dll
workspace/IvyVideo/trunk/ivyffmpeg/ffmpeg/libswscale/swscale-0.dll
workspace/IvyVideo/trunk/ivyffmpeg/ffmpeg/libx264/libx264-125.dll

5. 将上面所找到的dll与目标程序放在同一目录即可。
