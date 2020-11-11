#include "interface.h"
//initialisation de la fenetre
GtkWidget *init_fenetre(int largeur,int hauteur,char *titre){
	//declaration pointeur fenetre
	GtkWidget *fenetre;
	
	
	//initialisation de la fenetre
	fenetre= gtk_window_new(GTK_WINDOW_TOPLEVEL);
	
	//positions de la fenetre lors de l ouverture
	gtk_window_set_position(GTK_WINDOW(fenetre), GTK_WIN_POS_CENTER );
	
	//taille de la fenetre
	gtk_window_set_default_size(GTK_WINDOW(fenetre), hauteur, largeur);
	
	//titre de la fenetre
	gtk_window_set_title(GTK_WINDOW(fenetre), titre);
	
	return fenetre;
}


//fermeture de la fenêtre et arret de la boucle evenementielle
void fermeture_fenetre(GtkWidget *pWidget, gpointer pData){
	//arret de la boucle evenementielle
	gtk_main_quit();
}

void affiche_trame_terminal(GtkWidget *pWidget, gpointer pData){
	trame *tmp_trame=(trame *)pData;
	printf("Mac Dest: %.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n",(tmp_trame->mac_dest)[0],(tmp_trame->mac_dest)[1],(tmp_trame->mac_dest)[2],
										(tmp_trame->mac_dest)[3],(tmp_trame->mac_dest)[4],(tmp_trame->mac_dest)[5]);
	
	
}


void creer_model_arbre(GtkTreeStore *model, trame *trame){
		
}

void remplir_arbre(GtkWidget *pWidget, gpointer pData){

	cell *tmp_cell=(cell *)pData;	
	GtkTreeStore *model;

	enum{
		INFOS_COLUMN,
		N_COLUMNS
	};

	GtkTreeIter iter1;
	GtkTreeIter iter2;

	model = gtk_tree_store_new(N_COLUMNS, G_TYPE_STRING);
	
	gtk_tree_store_append(model, &iter1, NULL);
	gtk_tree_store_set(model, &iter1, INFOS_COLUMN, "Ethernet", -1);
	gtk_tree_store_append(model, &iter2, &iter1);
	gtk_tree_store_set(model, &iter2, INFOS_COLUMN, tmp_cell->obj->mac_source, -1);
	gtk_tree_store_append(model, &iter2, &iter1);
	gtk_tree_store_set(model, &iter2, INFOS_COLUMN, tmp_cell->obj->mac_dest, -1);

	tmp_cell->arbre = gtk_tree_view_new_with_model(GTK_TREE_MODEL(model));
	
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;

	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes("", renderer, "text", INFOS_COLUMN, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tmp_cell->arbre), column);
	gtk_widget_grab_focus(tmp_cell->arbre);
	
	return;
}

