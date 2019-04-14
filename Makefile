ifneq ($(KERNELRELEASE),)

obj-m := rpi-i2s-audio.o

else

KDIR ?= /lib/modules/`uname -r`/build

all:
	$(MAKE) -C $(KDIR) M=$$PWD modules

clean:
	$(MAKE) -C $(KDIR) M=$$PWD clean

endif
