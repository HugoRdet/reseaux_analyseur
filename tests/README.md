# BabyShark

BabyShark est un décodeur de paquets hexadécimaux inspiré de Wireshark.

Il peut lire des trames de réseau contenues dans un fichier *.txt* et en afficher le contenu
dans une interface simple d'utilisation.

### Protocoles reconnus

BabyShark est capable de décoder les entêtes de type:
* Ethernet
* IPv4 (ignore les options)
* TCP
* HTTP

### Structure du code

- **projet_reseaux.c** contient le programme principal. Il permet de lancer l'interface graphique.
- **interface.c** génère l'interface graphique et est en particulier responsable des différents affichages.
- **trame.c** est responsable de la lecture des fichiers *.txt* entrants, et de l'interprétation des données 
hexadécimales.
