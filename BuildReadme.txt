xcoder build instructions
=========================

1. ���û���
(a) ��װvs2008
(b) ��װcygwin: http://ftp.mozilla.org/pub/mozilla.org/mozilla/libraries/win32/MozillaBuildSetup-1.5.1.exe
(c) ����MozillaBuild��װĿ¼�µ�start-msvc9.bat������ɹ�����һ��������

2. ����
(a) ��ѹ��IvyVideo��c:\workspace
(b) ��ѹ��xcoders��c:\workspace

3. ����
����MozillaBuild�µ�start-msvc9.bat�����У����д������
(a) ����libx264
cd /c/workspace/IvyVideo/trunk/ivyffmpeg/ffmpeg/libx264
sh run_cfg_mingw.sh
make

(b) ����ffmpeg
cd /c/workspace/IvyVideo/trunk/ivyffmpeg/ffmpeg
sh run_cfg_mingw.sh
make

(c) ����xcoders
cd workspace/xcoders
make

4. �ռ���̬��

(a). ϵͳdlls(��xcoders����)
libgcc_s_dw2-1.dll, libstdc++-6.dll, libpthread-2.dll

(2). �������ɵ�dlls����Ŀ¼
workspace/xcoders/xcoders.dll
workspace/IvyVideo/trunk/ivyffmpeg/ffmpeg/libavcodec/avcodec.dll
workspace/IvyVideo/trunk/ivyffmpeg/ffmpeg/libavformat/avformat-52.dll
workspace/IvyVideo/trunk/ivyffmpeg/ffmpeg/libavutil/avutil-50.dll
workspace/IvyVideo/trunk/ivyffmpeg/ffmpeg/libswscale/swscale-0.dll
workspace/IvyVideo/trunk/ivyffmpeg/ffmpeg/libx264/libx264-125.dll

5. ���������ҵ���dll��Ŀ��������ͬһĿ¼���ɡ�
