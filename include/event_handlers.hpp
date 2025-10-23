#pragma once

#include <gtk/gtk.h>
#include "gtk_signal_data.hpp"
#include <iostream>

void on_calibrate_button_clicked (GtkWidget *widget, gpointer data);
void on_fov_calibration_capture_clicked(GtkWidget *widget, gpointer data);
void on_display_density_continue_clicked(GtkWidget *widget, gpointer data);
void on_horizontal_displacement_continue_clicked(GtkWidget *widget, gpointer data);
void on_vertical_displacement_continue_clicked(GtkWidget *widget, gpointer data);
void on_measurements_continue_clicked(GtkWidget *widget, gpointer data);