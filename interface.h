#ifndef __INTERFACE_H__
#define __INTERFACE_H__

#include <gtk/gtk.h>


//initialisation de la fenetre
GtkWidget *init_fenetre(int longueur,int largeur,char *titre);

//fermeture de la fenêtre et arret de la boucle evenementielle
void fermeture_fenetre(GtkWidget *pWidget, gpointer pData);

#endif