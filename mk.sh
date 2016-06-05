export TOP=~/android
export PATH=$TOP/linux-android-4.9/bin:$PATH

make ARCH=arm64 CROSS_COMPILE=~/android/linux-android-4.9/bin/aarch64-linux-android- O=out msm_defconfig
make ARCH=arm64 CROSS_COMPILE=~/android/linux-android-4.9/bin/aarch64-linux-android- O=out -j16

#./dtbToolCM -o dt.img -s 2048 -d "htc,project-id = <" -p ./scripts/dtc/ ./arch/arm/boot/





