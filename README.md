# Simple Qt screen rotation manager

Reads from accelerometer sensors, and rotate display according to the readings.

Works only in X11 for now.

Similar to the current solution implemented in Gnome, but works on all other desktop environments as well (KDE, XFCE, etc).

## Compilation requirements

 - cmake
 - gcc
 - Qt5 (with modules x11extras, sensors)
 - xrandr
 - XInput (Xi)
 
On ubuntu, run the following command to install dependencies:
```
sudo apt install -y git cmake build-essential qtbase5-dev libxrandr-dev libxi-dev libqt5x11extras5-dev libqt5sensors5-dev 
```

## Building
```
git clone https://github.com/GuLinux/ScreenRotator
mkdir ScreenRotator/build
cd ScreenRotator/build
cmake ..
make all
sudo make install
```

## Links

Main icon: https://www.iconfinder.com/icons/326583/orientation_rotation_screen_icon#size=256

