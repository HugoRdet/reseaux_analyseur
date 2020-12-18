#ifndef __TRAME_H__
#define __TRAME_H__
#include <gtk/gtk.h>

typedef struct trame_{
	unsigned int *tab;
	int id;
	int nb_ligne_erreur;
	int nb_octet_erreur;
	int place;	
	char *nom_fichier;
	//couche ethernet
	char *mac_dest;
	char *mac_source;
	char *ip_type;
	
	//couche ip
	char *version;
	char *header_length;
	int header_length_;
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
	char *urgent;
	char *no_option;
	int *option_tab;
	char *option_length;
	char *option_mss;	
	char *option_wscale;
	char *option_ts_val;
	char *option_ts_ecr;
}trame;

typedef struct cell_{
	trame *obj;
	GtkWidget *bouton;
	GtkWidget *revealer;
	struct cell_ *suiv;
	struct cell_ **bouton_actif;
	int status_bouton_ip;
}cell;

typedef struct cell_button_{
	GtkWidget *button;
	struct cell_button_ *suiv;
}cell_button;

typedef struct trame_agrandie_{
	
	struct cell_ *obj;
	GtkWidget **tab_box_offset;
	int ligne;
	GtkWidget *box_trame;
	GtkWidget *box_offset;
	
	GtkWidget *expander_ethernet;
	GtkWidget *expander_ip;
	GtkWidget *expander_tcp;
	GtkWidget *expander_http;
	GtkWidget *expander_flags;
	GtkWidget *expander_flags_tcp;
	
	struct cell_button_ *u_box_actif;
	struct cell_button_ *b_box_actif;
	struct cell_button_ *b_mac_dest;
	struct cell_button_ *u_mac_dest;
	struct cell_button_ *b_mac_source;
	struct cell_button_ *u_mac_source;
	struct cell_button_ *b_ip_type;
	struct cell_button_ *u_ip_type;
	
	struct cell_button_ *b_version_hl;
	struct cell_button_ *u_version;
	struct cell_button_ *u_header_length;
	struct cell_button_ *b_total_length;
	struct cell_button_ *u_total_length;
	struct cell_button_ *b_identification;
	struct cell_button_ *u_identification;
	struct cell_button_ *b_flags_offset;
	struct cell_button_ *u_flags_offset;
	
	struct cell_button_ *b_reserved_bit;
	struct cell_button_ *u_reserved_bit;
	struct cell_button_ *b_dont_fragment;
	struct cell_button_ *u_dont_fragment;
	struct cell_button_ *b_more_fragment;
	struct cell_button_ *u_more_fragment;
	struct cell_button_ *b_frag_offset;
	struct cell_button_ *u_frag_offset;
	
	struct cell_button_ *b_ttl;
	struct cell_button_ *u_ttl;
	
	struct cell_button_ *b_protocol;
	struct cell_button_ *u_protocol;
	struct cell_button_ *b_header_checksum; 
	struct cell_button_ *u_header_checksum; 
	struct cell_button_ *b_ip_source;
	struct cell_button_ *u_ip_source;
	struct cell_button_ *b_ip_dest;
	struct cell_button_ *b_ip_dest_1;
	struct cell_button_ *u_ip_dest;
	
	struct cell_button_ *b_source_port;
	struct cell_button_ *u_source_port;
	struct cell_button_ *b_destination_port;
	struct cell_button_ *u_destination_port;
	struct cell_button_ *b_sequence_number;
	struct cell_button_ *u_sequence_number;
	struct cell_button_ *b_sequence_number_raw;
	struct cell_button_ *u_sequence_number_raw;
	struct cell_button_ *b_next_sequence_number;
	struct cell_button_ *u_next_sequence_number;
	struct cell_button_ *b_acknowledgment_number;
	struct cell_button_ *u_acknowledgment_number;
	struct cell_button_ *b_acknowledgment_number_raw;
	struct cell_button_ *u_acknowledgment_number_raw;
	struct cell_button_ *b_tcp_header_length;
	struct cell_button_ *u_tcp_header_length;
	struct cell_button_ *b_urg;
	struct cell_button_ *u_urg;
	struct cell_button_ *b_ack;
	struct cell_button_ *u_ack;
	struct cell_button_ *b_push;
	struct cell_button_ *u_push;
	struct cell_button_ *b_reset;
	struct cell_button_ *u_reset;
	struct cell_button_ *b_syn;
	struct cell_button_ *u_syn;
	struct cell_button_ *b_fin;
	struct cell_button_ *u_fin;
	struct cell_button_ *u_tcp_f;
	struct cell_button_ *b_tcp_f0;
	struct cell_button_ *b_tcp_f1;
	struct cell_button_ *b_tcp_f2;
	struct cell_button_ *b_tcp_f3;
	struct cell_button_ *b_tcp_f4;
	struct cell_button_ *b_tcp_f5;
	struct cell_button_ *b_window;
	struct cell_button_ *u_window;
	struct cell_button_ *b_tcp_checksum;
	struct cell_button_ *u_tcp_checksum;
	struct cell_button_ *b_urgent;
	struct cell_button_ *u_urgent;
	struct cell_button_ *b_no_option;
	struct cell_button_ *u_no_option;
	struct cell_button_ *b_option_tab;
	struct cell_button_ *u_option_tab;
	struct cell_button_ *b_option_length;
	struct cell_button_ *u_option_length;
	struct cell_button_ *b_option_mss;
	struct cell_button_ *u_option_mss;
	struct cell_button_ *b_option_wscale;
	struct cell_button_ *u_option_wscale;
	struct cell_button_ *b_option_ts_val;
	struct cell_button_ *u_option_ts_val;
	struct cell_button_ *b_option_ts_ecr;
	struct cell_button_ *u_option_ts_ecr;

	
		
}trame_agrandie;

void afficher_ligne(FILE *fichier);
int charge_trame(FILE *fichier_src,int *ligne,int nb_trame,cell **liste,GtkWidget *box_haut,GtkWidget *box_bas,char *filename);
int cherche_prochaine_ligne(FILE *fichier_src,int *pt_offset,int *ligne);
#endif
