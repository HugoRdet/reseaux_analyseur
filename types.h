#ifndef __TYPES_H__
#define __TYPES_H__
#include <gtk/gtk.h>



typedef struct box_{
	GtkWidget **pvbox_haut;
	GtkWidget **pvbox_bas;
	GtkWidget **window;
	struct cell_ **liste;
	int taille_liste;
}box;

typedef struct cell_{
	struct trame_ *obj;
	GtkWidget *bouton;
	GtkWidget *revealer;
	struct cell_ *suiv;
	struct cell_ **bouton_actif;
	int status_bouton_ip;
}cell;


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
	int tcp_header_length_;
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
	
	// couche http
	char *methode_;
	int methode;
	char *http;
}trame;


typedef struct cell_button_{
	GtkWidget *button;
	struct cell_button_ *suiv;
}cell_button;

typedef struct cell_button_option_{
	struct cell_button_ *button_0;
	struct cell_button_ *button_1;
	struct cell_button_ *button_2;
	GtkWidget *expander;
}cell_button_option;


typedef struct trame_agrandie_{
	
	struct cell_ *obj;
	cell_button **tab_offset;
	GtkWidget *box_haut;
	GtkWidget *box_bas;
	
	GtkWidget *expander_ethernet;
	GtkWidget *expander_ip;
	GtkWidget *expander_tcp;
	GtkWidget *expander_http;
	GtkWidget *expander_flags;
	GtkWidget *expander_flags_tcp;
	GtkWidget *expander_options_tcp;
	
	GtkWidget *label_bas;
	
	cell_button *box_haut_actif;
	cell_button *box_bas_actif;
	//couche ethernet
	cell_button *mac_dest;
	cell_button *mac_source;
	cell_button *ip_type;
	
	
	//couche ip
	cell_button *version;
	cell_button *header_length;
	
	cell_button *total_length;
	cell_button *identification;
	cell_button *flags_offset;
	cell_button *reserved_bit;
	cell_button *dont_fragment;
	cell_button *more_fragment;
	cell_button *flags_value;
	cell_button *frag_offset;
	cell_button *f0;
	cell_button *f1;
	cell_button *f2;
	cell_button *TTL;
	cell_button *protocol;
	cell_button *header_checksum; 
	cell_button *ip_source;
	cell_button *ip_dest;
	
	//couche tcp
	cell_button *source_port;
	cell_button *destination_port;
	cell_button *sequence_number;
	cell_button *sequence_number_raw;
	cell_button *next_sequence_number;
	cell_button *acknowledgment_number;
	cell_button *acknowledgment_number_raw;
	cell_button *tcp_header_length;
	cell_button *urg;
	cell_button *ack;
	cell_button *push;
	cell_button *reset;
	cell_button *syn;
	cell_button *fin;
	cell_button *tcp_f0;
	cell_button *tcp_f1;
	cell_button *tcp_f2;
	cell_button *tcp_f3;
	cell_button *cp_f4;
	cell_button *tcp_f5;
	cell_button *window;
	cell_button *tcp_checksum;
	cell_button *urgent;
	cell_button *no_option;
	cell_button *option_length;
	cell_button *option_mss;	
	cell_button *option_wscale;
	cell_button *option_ts_val;
	cell_button *option_ts_ecr;
	
	cell_button_option **option_tab;
	
	// couche http
	cell_button *methode;
	cell_button *http;
	
	
}trame_agrandie;



#endif