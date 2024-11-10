#!/bin/bash
#=================================================
# Copyright (c) 2019-2020 P3TERX <https://p3terx.com>
#
# This is free software, licensed under the MIT License.
# See /LICENSE for more information.
#
# https://github.com/P3TERX/Actions-OpenWrt
# File name: diy-part5.sh
# Description: OpenWrt DIY script part 2 (After Update feeds)
#=================================================

# 修改默认IP
sed -i 's/192.168.1.1/192.168.8.1/g' package/base-files/files/bin/config_generate
# 修改网关
#sed -i 's/192.168.$((addr_offset++)).1/192.168.$((addr_offset++)).1/g' package/base-files/files/bin/config_generate
# 修改默认wifi名称ssid为Zte_E8820-v1
sed -i 's/ssid=OpenWrt/ssid=Zte_E8820v1_${devidx}/g' package/kernel/mac80211/files/lib/wifi/mac80211.sh
#修改默认wifi开启
sed -i 's/wireless.radio${devidx}.disabled=1/wireless.radio${devidx}.disabled=0/g' package/kernel/mac80211/files/lib/wifi/mac80211.sh
