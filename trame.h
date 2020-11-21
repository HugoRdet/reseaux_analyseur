#ifndef __TRAME_H__
#define __TRAME_H__
#include <gtk/gtk.h>

typedef struct trame_{
	unsigned int *tab;
	int id;
	int nb_ligne_erreur;
	int nb_octet_erreur;
	
	
	//couche ethernet
	char *mac_dest;
	char *mac_source;
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
	GtkWidget *arbre;
	GtkWidget *bouton;
	struct cell_ *suiv;
	int status_bouton_ip;
}cell;

void afficher_ligne(FILE *fichier);
int charge_trame(FILE *fichier_src,int *ligne,int nb_trame,cell **liste,GtkWidget *box_haut,GtkWidget *box_bas);
int cherche_prochaine_ligne(FILE *fichier_src,int *pt_offset,int *ligne);
#endif
