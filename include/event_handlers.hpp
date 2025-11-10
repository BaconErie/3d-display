#pragma once

#include <gtk/gtk.h>
#include <iostream>
#include <cmath>

#include "gtk_signal_data.hpp"
#include "shared.hpp"
#include "interlacer.hpp"

const float QR_CODE_WIDTH = 6;

namespace event_handlers {
    void on_calibrate_button_clicked (GtkWidget *widget, gpointer data);
    void on_fov_calibration_capture_clicked(GtkWidget *widget, gpointer data);
    void on_display_density_continue_clicked(GtkWidget *widget, gpointer data);
    void on_horizontal_displacement_continue_clicked(GtkWidget *widget, gpointer data);
    void on_vertical_displacement_continue_clicked(GtkWidget *widget, gpointer data);
    void on_measurements_continue_clicked(GtkWidget *widget, gpointer data);
    void on_start_display_clicked(GtkWidget *widget, gpointer data);
    void on_renderer_success();
}
