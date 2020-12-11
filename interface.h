#ifndef __INTERFACE_H__
#define __INTERFACE_H__
#include "trame.h"

typedef struct box_{
	GtkWidget **pvbox_haut;
	GtkWidget **pvbox_bas;
	GtkWidget **window;
	cell **liste;
	int taille_liste;
}box;

void action_bouton_ip(GtkWidget *pWidget, gpointer pData);
//initialisation de la fenetre
void init_menu(GtkWidget* grille,box* pvbox);
void init_panneau(GtkWidget *grille,GtkWidget **pvbox_haut_,GtkWidget **pvbox_bas_);
GtkWidget *init_fenetre(int largeur,int hauteur,char *titre,GtkWidget** grid);

//fermeture de la fenêtre et arret de la boucle evenementielle
void fermeture_fenetre(GtkWidget *pWidget, gpointer pData);

void affiche_trame_terminal(GtkWidget *pWidget, gpointer pData);
void remplir_arbre(GtkWidget *new_box, gpointer pData,int statut);
void assigne_ui_liste(cell **liste,GtkWidget* box_haut, GtkWidget* box_bas);
#endif