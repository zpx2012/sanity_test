sudo apt-get install -y vnc4server expect x-window-system-core gdm gnome-panel gnome-settings-daemon metacity nautilus gnome-terminal gtk2-engines-pixbuf
sudo DEBIAN_FRONTEND=noninteractive apt-get install -y ubuntu-desktop
sudo echo "/usr/sbin/lightdm" > /etc/X11/default-display-manager
sudo chmod a+x ~/sanity_test/vpn/install/vncserver.exp
~/sanity_test/vpn/install/vncserver.exp
echo '''#!/bin/sh
# Uncomment the following two lines for normal desktop:
export XKL_XMODMAP_DISABLE=1
 unset SESSION_MANAGER
# exec /etc/X11/xinit/xinitrc
unset DBUS_SESSION_BUS_ADDRESS
gnome-panel &
gnome-settings-daemon &
metacity &
nautilus &
gnome-terminal &''' > ~/.vnc/xstartup
vncserver -kill :1
vncserver :1
sudo dpkg -i ~/astrill-setup-linux64.deb
