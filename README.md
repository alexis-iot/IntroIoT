# IntroIoT
Intro to IoT Project

qemu-system-arm -M versatilepb -kernel output/images/zImage -dtb output/images/versatile-pb.dtb -drive file=output/images/rootfs.ext2,if=scsi,format=raw -append "root=/dev/sda console=ttyAMA0,115200" -serial stdio -net nic,model=rtl8139 -net user,hostfwd=tcp::2222-:22

ssh -p 2222 alexis@localhost

make -f makefile-arm

scp -P 2222 test alexis@localhost:~/
