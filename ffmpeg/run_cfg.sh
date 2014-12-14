export NDK=/opt/android-ndk-r8b
export SYSROOT=$NDK/platforms/android-8/arch-arm
export PATH=$PATH:$NDK/toolchains/arm-linux-androideabi-4.4.3/prebuilt/linux-x86/bin

./configure \
	--enable-gpl \
	--enable-shared \
	--enable-pic \
	--disable-doc  --disable-ffmpeg --disable-ffplay --disable-ffserver \
	--enable-avfilter --enable-avfilter-lavf \
	--enable-postproc \
	--enable-pthreads --disable-mpegaudio-hp --disable-ipv6 \
	--disable-altivec --disable-amd3dnow --disable-amd3dnowext \
	--disable-mmx --disable-mmx2 --disable-sse --disable-ssse3 \
	--disable-debug --disable-optimizations --disable-stripping \
	--enable-zlib \
	--disable-bsfs --disable-devices --disable-outdevs --disable-indevs --disable-hwaccels \
	--disable-encoders \
        --enable-libx264 \
	--enable-encoder=aac --enable-encoder=flv --enable-encoder=mpeg4 --enable-encoder=libx264 \
	--disable-decoders \
	--enable-decoder=aac --enable-decoder=flv --enable-decoder=h264 --enable-decoder=mpeg4 \
	--disable-demuxers \
	--enable-demuxer=aac --enable-demuxer=flv --enable-demuxer=h264 \
	--disable-muxers \
	--enable-muxer=flv --enable-muxer=h264 \
	--disable-protocols \
	--enable-protocol=file --enable-protocol=http --enable-protocol=rtp \
	--disable-parsers \
	--enable-parser=aac --enable-parser=h264 --enable-parser=mpeg4video \
	--disable-filters \
	--enable-filter=null \
        --extra-cflags=-I./libx264 \
        --extra-ldflags=-L./build/android/obj/local/armeabi-v7a \
        --arch=arm \
        --host-cc=arm-linux-androideabi-gcc \
        --sysroot=$SYSROOT \
        --cross-prefix=arm-linux-androideabi-

