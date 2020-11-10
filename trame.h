#ifndef __TRAME_H__
#define __TRAME_H__
#include <gtk/gtk.h>

typedef struct trame_{
	//quand un champ sera erroné il sera placé a -1
	unsigned int *tab;
	int id;
	// nb_ligne_erreur est a -1 si il n y aucune erreur
	int nb_ligne_erreur;
	// la taille maximale d une trame est de 1518 octets
	//soit 94*16+14
	
	//couche ethernet
	int *mac_dest;
	int *mac_source;
	int *ip_type;
	
	//couche ip
	int version_HL;
	int DSF;
	int *total_length;
	int *identification;
	int *flags_frag_offset;
	int TTL;
	int protocol;
	int *header_checksum;
	int *ip_source;
	int *ip_dest;
	
	//couche tcp
	int source_port;
	int destination_port;
	int tcp_segment_len;
}trame;

typedef struct cell_{
	trame *obj;
	GtkWidget *bouton;
	struct cell_ *suiv;
}cell;

void afficher_ligne(FILE *fichier);
int charge_trame(FILE *fichier_src,int *ligne,int nb_trame,cell **liste,GtkWidget *box);
int cherche_prochaine_ligne(FILE *fichier_src,int *pt_offset,int *ligne);
#endif