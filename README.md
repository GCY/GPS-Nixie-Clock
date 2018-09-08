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

g++ -o2 -o wxnixieclock.app wxnixieclock.cpp serialport.cpp connectargsdlg.cpp \`wx-config --cxxflags --libs\` -m64
