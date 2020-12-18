# BabyShark
BabyShark is a simple hex-packet decoder, inspired from Wireshark.
It parses network frames from .txt files and display the data in an intelligible way.
![](https://github.com/HugoRdet/reseaux_analyseur/tree/main/screenshots/screen_shot_1.png | width=48)

### Install
##### Requirements :  
git  
make
#### Unix user
Simply clone the repository and compile from there.
```
git clone "https://github.com/HugoRdet/reseaux_analyseur" && cd reseaux_analyseur 
```
```
make clean update && make
```
### Launch
#### iOS
Simply double-click on the output file or type in a terminal:
```
./projet_reseaux
```
#### Linux & BSD
The archive comes with a .desktop file.

First allow it to be executed : 
```
sudo chmod +x babyshark.desktop
```
Then move the icon in your .icons folder :
```
mv ./icones/babyshark.png ~/.icons/babyshark.png
```
Now you can double clik on the .desktop file to launch the application.
### Usage 
* Fetch a .txt file using the "Open File" icon.
* Display the data in the tree view by clicking on the frame in the upper panel.
* Click on the option menu in the bottom pannel to open the frame in a new window,
which grants the ability to highlight in real time the corresponding bytes of the frame.
* Save the newly parsed data as *.txt file* using the "Save File" icon.
