#!/bin/bash
#
# Copyright (c) 2019-2020 P3TERX <https://p3terx.com>
#
# This is free software, licensed under the MIT License.
# See /LICENSE for more information.
#
# https://github.com/P3TERX/Actions-OpenWrt
# File name: diy-part1.sh
# Description: OpenWrt DIY script part 1 (Before Update feeds)
# 复制E8820V1配置文件到编译目录
cp -rf $GITHUB_WORKSPACE/patchs/E8820V1/* $GITHUB_WORKSPACE/openwrt/target/linux/ath79/

#备用方案
sed -i 's/git.openwrt.org\/feed\/packages.git;openwrt-21.02/github.com\/Lienol\/openwrt-packages.git;21.02/g' feeds.conf.default
sed -i 's/git.openwrt.org\/project\/luci.git;openwrt-21.02/github.com\/coolsnowwolf\/luci.git;master/g' feeds.conf.default
# 增加软件包
sed -i '$a src-git helloworld https://github.com/fw876/helloworld.git;master' feeds.conf.default
sed -i '$a src-git kenzo https://github.com/kenzok8/openwrt-packages.git;master' feeds.conf.default
sed -i '$a src-git small https://github.com/kenzok8/small.git;master' feeds.conf.default
sed -i '$a src-git small8 https://github.com/kenzok8/small-package.git;main' feeds.conf.default

# 删除冲突包
# rm -rf feeds/smpackage/{base-files,dnsmasq,firewall*,fullconenat,libnftnl,nftables,ppp,opkg,ucl,upx,vsftpd-alt,miniupnpd-iptables,wireless-regdb}

# 修改默认dnsmasq为dnsmasq-full
sed -i 's/dnsmasq/dnsmasq-full firewall iptables block-mount coremark kmod-nf-nathelper kmod-nf-nathelper-extra kmod-ipt-raw kmod-ipt-raw6 kmod-tun/g' include/target.mk

# 修改默认编译LUCI进系统
sed -i 's/ppp-mod-pppoe/iptables-mod-tproxy iptables-mod-extra ipset ip-full ppp ppp-mod-pppoe default-settings luci curl ca-certificates/g' include/target.mk

# 单独拉取软件包
git clone -b Lienol-default-settings https://github.com/yuos-bit/other package/default-settings
git clone -b main --single-branch https://github.com/yuos-bit/other package/yuos
git clone -b main --single-branch https://github.com/siwind/luci-app-usb_printer.git package/yuos/luci-app-usb_printer
git clone -b master --single-branch https://github.com/MilesPoupart/luci-app-vssr package/yuos/luci-app-vssr

# 修改/tools/Makefile
# sed -i '11a tools-y += ucl upx\n$(curdir)/upx/compile := $(curdir)/ucl/compile' tools/Makefile
cp -rf $GITHUB_WORKSPACE/patchs/5.4/package/* $GITHUB_WORKSPACE/openwrt/tools/
# rpcd
# cp -rf $GITHUB_WORKSPACE/patchs/5.4/rpcd/* package/system/rpcd/

# 应用UPX补丁包
patch -p1 < $GITHUB_WORKSPACE/openwrt/tools/upx/patches/010-fix-build-with-gcc11.patch
patch -p1 < $GITHUB_WORKSPACE/openwrt/tools/ucl/patches/001-autoconf-compat.patch


#删除重复包
rm -rf feeds/small8/{base-files,dnsmasq,firewall*,fullconenat,libnftnl,nftables,ppp,opkg,ucl,upx,vsftpd-alt,miniupnpd-iptables,wireless-regdb}

