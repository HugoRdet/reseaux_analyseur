# BabyShark

BabyShark est un décodeur simple de paquets hexadécimaux, inspiré de Wireshark.
Il peut lire des trames depuis un fichier *.txt* et en produire une interprétation intelligible.

### Installation
##### Prérequis :  
git  
make
#### Utilisateur Unix
Cloner le dépôt depuis la page github du projet :
```
git clone "https://github.com/HugoRdet/reseaux_analyseur"
```
Ou extraire directement l'archive téléchargée et compiler depuis la destination.
```
cd reseaux_analyseur 
```
```
make clean update && make
```
### Lancement
#### iOS
Double-cliquer simplement sur l'exécutable produit, ou taper dans un terminal :
```
./projet_reseaux
```
#### Linux & BSD
L'archive fournit un fichier *.desktop*.
Il faut autoriser son exécution : 
```
sudo chmod +x babyshark.desktop
```
Et déplacer l'icone de l'application dans le dossier .icons : 
```
mv ./icones/babyshark.png ~/.icons/babyshark.png
```
Il suffit maintenant de double cliquer sur le fichier *.desktop* pour lancer l'application

### Utilisation 
* Ouvrir un fichier *.txt* avec l'icone "Open File".
* Cliquer sur une trame dans le panneau supérieur fait apparaître la vue arborescente correspondante.
* Le menu "Options" dans le panneau inférieur permet d'afficher l'arborescence dans une nouvelle fenêtre et de surligner en temps réel les octets correspondants dans un second panneau.
* Sauvegarder la vue arborescente dans un fichier *.txt* avec l'icone "Save File"
