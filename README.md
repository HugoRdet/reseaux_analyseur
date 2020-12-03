# BabyShark
BabyShark is a simple hex-packet decoder, inspired from Wireshark.
It parses network frames from .txt files and display the data in an intelligible way.
### Install
##### Requirements :  
git  
make
#### Unix user
Simply clone the repository and compile from there.
```
git clone "https://github.com/HugoRdet/reseaux_analyseur" &&
cd reseaux_analyseur &&
 make
```
### Usage 
* Fetch a .txt file using the "Open File" icon.
* Display the data in the tree view by clicking on the frame in the upper panel.
* Save the newly parsed data using the "Save File" icon.
