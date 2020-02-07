#! /bin/bash

cd ~/sanity_test/vpn/
screen -dmS http bash 0206_http.sh astrill-hk
sleep 60
screen -dmS vpn bash 0206_vpn.sh $1 $2 astrill-hk


# 1. Transfer the astrillvpn installation file
# 2. Install astrillvpn
# 3. Install WSL
    # Enable-WindowsOptionalFeature -Online -FeatureName Microsoft-Windows-Subsystem-Linux
    # Invoke-WebRequest -Uri https://aka.ms/wsl-ubuntu-1604 -OutFile Ubuntu.appx -UseBasicParsing
    # Add-AppxPackage Ubuntu.appx 
# 4. Git clone / install.sh
# git clone https://github.com/zpx2012/sanity_test.git
# 5. Change FIN_Wait2/Timewait timeout
# HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\Tcpip\Parameters
# TcpFinWait2Delay
# TcpTimedWaitDelay
# 209.95.60.176