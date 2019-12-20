# Nixie-Clock

This is GPS Nixie clock,include IN12B, IN14, IN16, and IN2 version.</br>

![alt text](https://github.com/GCY/Nixie-Clock/blob/master/nixie%20clock.png)

## Firmware
### Parameter

__SERIAL__ : output info to serial port.</br>
DOT : use nixie tube dot.</br>
__GPS__ : time sync with Neo-6M - u-blox chip, this function only for IN14 IN16 version, and time sync separator is GPGGA[7] = {'$','G','P','G','G','A',','};</br>
<pre><code>
#define __SERIAL__ 1
#define DOT 1
#define __GPS__ 1
</code></pre>

gps_update : gps sync period, unit is ms, 300000ms = 5min.</br> 
<pre><code>
const unsigned long gps_update = 300000;
</code></pre>

Type select, IN_14 == IN_16.</br> 
<pre><code>
enum{
  IN_12B = 100,
  IN_14,
  IN_2
};
</code></pre>

### Command

case '1' : clock mode
</br>
case '2' : temperature mode
</br>
case '3' : humidity mode
</br>
case '4' : flash mode
</br>
case '5' : counter mode
</br>
case '6' : '1' + '2' + '3'
</br>
case 's' | 'S' : set time
</br></br>

## Android time sync tool
Android App modified BlueTerm project. </br>

## PC Software - wxNixieClock
</br>
wxNixieClock is time sync tool for Nixie Clock project.(Only MAC-OS-X)</br>
### Build
g++ -o2 -o wxnixieclock.app wxnixieclock.cpp serialport.cpp connectargsdlg.cpp \`wx-config --cxxflags --libs\` -m64</br>
</br>
</br>
![alt text](https://github.com/GCY/Nixie-Clock/blob/master/wxNixieClock/pic.png)
</br>
</br>

[![Audi R8](http://img.youtube.com/vi/tJzohsqhTxs/0.jpg)](https://youtu.be/tJzohsqhTxs)

### Use
</br>
1. Pair BT of MAC and Nixie Clock.</br>
2. Open wxNixieClock to select tools -> Connect Device -> cu.BT device driver</br>
3. Click SyncTime button</br>
4. Done!</br>
</br>
### Dependency
</br>
Install wxWidgets dependency in terminal</br>
</br>
1. user$ /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)" </br>
2. brew install wxwidgets </br>
