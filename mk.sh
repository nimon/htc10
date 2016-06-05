export TOP=~/android
export PATH=$TOP/linux-android-4.9/bin:$PATH

make ARCH=arm64 CROSS_COMPILE=~/android/linux-android-4.9/bin/aarch64-linux-android- O=out msm_defconfig
make ARCH=arm64 CROSS_COMPILE=~/android/linux-android-4.9/bin/aarch64-linux-android- O=out -j16


