#ifndef __TRAME_H__
#define __TRAME_H__
#include "types.h"




void afficher_ligne(FILE *fichier);
int charge_trame(FILE *fichier_src,int *ligne,int nb_trame,cell **liste,GtkWidget *box_haut,GtkWidget *box_bas,char *filename,int *erreur);
int cherche_prochaine_ligne(FILE *fichier_src,int *pt_offset,int *ligne);
#endif
