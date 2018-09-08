# Nixie-Clock

![alt text](https://1.bp.blogspot.com/-yudi6DA56mM/WZk1IM_Z4DI/AAAAAAAALVc/Igc8mz3Kq1sGENdTJ9W_CU33nhVXYXnrACLcBGAs/s1600/DSC_0580.JPG)

command

'1' : clock mode
</br>
'2' : temperature mode
</br>
'3' : humidity mode
</br>
'4' : flash mode
</br>
'5' : counter mode
</br>
'6' : '1' + '2' + '3'
</br>
's' | 'S' : set time
</br></br>
</br>
wxNixieClock is time sync tool for Nixie Clock project.(Only MAC-OS-X)</br>
g++ -o2 -o wxnixieclock.app wxnixieclock.cpp serialport.cpp connectargsdlg.cpp \`wx-config --cxxflags --libs\` -m64</br>
</br>
</br>
![alt text](https://github.com/GCY/Nixie-Clock/blob/master/wxNixieClock/pic.png)
</br>
</br>

1. Pair BT of MAC and Nixie Clock.</br>
2. Open wxNixieClock select to tools -> Connect Device -> cu.BT device driver</br>
3. Click SyncTime button</br>
4. Done!</br>
</br>
Install wxWidgets dependency in terminal</br>
</br>
1. user$ /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)" </br>
2. brew install wxwidgets </br>
