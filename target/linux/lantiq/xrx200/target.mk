ARCH:=mips
SUBTARGET:=xrx200
BOARDNAME:=Lantiq xRX200
FEATURES:=squashfs jffs2 atm
CFLAGS+= -mtune=34kc

DEFAULT_PACKAGES+=LTQBASE kmod-pppoa ppp-mod-pppoa linux-atm atm-tools br2684ctl kmod-ltq-dsl-vr9 ltq-dsl-app swconfig kmod-lantiq_ppa_xrx200 \
		kmod-vpe kmod-lantiq-vpe

define Target/Description
	Lantiq xRX200
endef
