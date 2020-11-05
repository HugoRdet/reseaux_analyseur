#ifndef __INTERFACE_H__
#define __INTERFACE_H__

#include <gtk/gtk.h>

void print_hello (GtkWidget *widget, gpointer   data);
void activate (GtkApplication *app,gpointer        user_data);

#endif