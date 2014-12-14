export NDK=/opt/android-ndk-r8b
export SYSROOT=$NDK/platforms/android-8/arch-arm
export PATH=$PATH:$NDK/toolchains/arm-linux-androideabi-4.4.3/prebuilt/linux-x86/bin

./configure \
    --disable-cli \
    --enable-shared \
    --bit-depth=8 \
    --chroma-format=420 \
    --enable-strip \
    --enable-pic \
    --disable-avs \
    --disable-ffms \
    --disable-gpac \
    --host=arm-linux-androideabi \
    --sysroot=$SYSROOT \
    --cross-prefix=arm-linux-androideabi-

