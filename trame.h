#ifndef __TRAME_H__
#define __TRAME_H__
#include <gtk/gtk.h>

typedef struct trame_{
	unsigned int *tab;
	int id;
	int nb_ligne_erreur;
	int nb_octet_erreur;
	int place;	
	
	//couche ethernet
	char *mac_dest;
	char *mac_source;
	char *ip_type;
	
	//couche ip
	char *version;
	char *header_length;

	char *total_length;
	char *identification;
	char *flags_offset;
	char *reserved_bit;
	char *dont_fragment;
	char *more_fragment;
	char *frag_offset;
	int TTL;
	char *protocol;
	char *header_checksum; 
	char *ip_source;
	char *ip_dest;
	
	//couche tcp
	int source_port;
	int destination_port;
	int stream_index;
	int tcp_seg_len;
	char* sequence_number;
	int sequence_number_raw;
	char *next_sequence_number;
	char *acknowledgment_number;
	int acknowledgment_number_raw;
	char *tcp_header_length;
}trame;

typedef struct cell_{
	trame *obj;
	GtkWidget *bouton;
	GtkWidget *revealer;
	struct cell_ *suiv;
	int status_bouton_ip;
	
	
}cell;

void afficher_ligne(FILE *fichier);
int charge_trame(FILE *fichier_src,int *ligne,int nb_trame,cell **liste,GtkWidget *box_haut,GtkWidget *box_bas);
int cherche_prochaine_ligne(FILE *fichier_src,int *pt_offset,int *ligne);
#endif
