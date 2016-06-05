export TOP=~/android
export PATH=$TOP/arm-eabi-4.8/bin:$PATH
export ARCH=arm
export SUBARCH=arm
export CROSS_COMPILE=arm-eabi-
make clean
#rm ~/android/mkboot/cm/zImage
#cp arch/arm/boot/zImage ~/android/mkboot/cm/
