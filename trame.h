#ifndef __TRAME_H__
#define __TRAME_H__
#include <gtk/gtk.h>

typedef struct trame_{
	int *tab;
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
	int f0;
	int f1;
	int f2;
	int TTL;
	char *protocol;
	char *header_checksum; 
	char *ip_source;
	char *ip_dest;
	
	//couche tcp
	char *source_port;
	char *destination_port;
	char* sequence_number;
	char* sequence_number_raw;
	char *next_sequence_number;
	char *acknowledgment_number;
	char *acknowledgment_number_raw;
	char *tcp_header_length;
	char *urg;
	char *ack;
	char *push;
	char *reset;
	char *syn;
	char *fin;
	int tcp_f0;
	int tcp_f1;
	int tcp_f2;
	int tcp_f3;
	int tcp_f4;
	int tcp_f5;
	char *window;
	char *tcp_checksum;
	//...
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
