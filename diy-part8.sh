#!/bin/bash
#
# Copyright (c) 2019-2020 P3TERX <https://p3terx.com>
#
# This is free software, licensed under the MIT License.
# See /LICENSE for more information.
#
# https://github.com/P3TERX/Actions-OpenWrt
# File name: diy-part8.sh
# Description: OpenWrt DIY script part 1 (Before Update feeds)
# 复制E8820V1配置文件到编译目录
cp -rf $GITHUB_WORKSPACE/patchs/E8820V1/* $GITHUB_WORKSPACE/openwrt/target/linux/ath79/
