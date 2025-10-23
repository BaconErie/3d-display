#include "event_handlers.hpp"

void event_handlers::on_calibrate_button_clicked (GtkWidget *widget, gpointer data)
{
  gtk_signal_data* signal_data = static_cast<gtk_signal_data*>(data);
  GtkWidget* stack_widget = signal_data->stack_widget;
  GtkBuilder *builder = static_cast<gtk_signal_data*>(data)->builder;

  // Switch to the calibration stack first
  gtk_stack_set_visible_child_name(GTK_STACK(stack_widget), "fov_calibration_box");
}

void event_handlers::on_fov_calibration_capture_clicked(GtkWidget *widget, gpointer data)
{
  gtk_signal_data* signal_data = static_cast<gtk_signal_data*>(data);
  GtkWidget* stack_widget = signal_data->stack_widget;

  // Switch to the display density calibration stack page
  gtk_stack_set_visible_child_name(GTK_STACK(stack_widget), "measurements_calibration_box");
}

void event_handlers::on_measurements_continue_clicked(GtkWidget *widget, gpointer data)
{

  gtk_signal_data* signal_data = static_cast<gtk_signal_data*>(data);
  GtkWidget* stack_widget = signal_data->stack_widget;

  std::string qr_code_distance_input(gtk_editable_get_chars(signal_data->qr_code_distance_editable, 0, -1));
  float qr_code_distance = 0.0; 
  bool was_parse_successful = false;

  try {
    qr_code_distance = std::stof(qr_code_distance_input);
    was_parse_successful = true;
  } catch (const std::invalid_argument& e) {
    std::cerr << "Invalid input for QR code distance: " << e.what() << std::endl;
  }

  if (!was_parse_successful) return;

  // Get the lenticule density

  std::string lenticule_density_input(gtk_editable_get_chars(signal_data->lenticule_density_editable, 0, -1));
  float lenticule_density = 0.0; 
  was_parse_successful = false;

  try {
    lenticule_density = std::stof(lenticule_density_input);
    was_parse_successful = true;
  } catch (const std::invalid_argument& e) {
    std::cerr << "Invalid input for lenticule density: " << e.what() << std::endl;
  }

  if (!was_parse_successful) return;

  std::cout << "QR Code distance: " << qr_code_distance << " in." << std::endl;
  std::cout << "Lenticule density: " << lenticule_density << " LPI" << std::endl;

  // Switch back to the main calibration stack
  gtk_stack_set_visible_child_name(GTK_STACK(stack_widget), "display_density_calibration_box");
}

void event_handlers::on_display_density_continue_clicked(GtkWidget *widget, gpointer data)
{
  gtk_signal_data* signal_data = static_cast<gtk_signal_data*>(data);
  GtkWidget* stack_widget = signal_data->stack_widget;

  std::string green_to_red_line_distance_input(gtk_editable_get_chars(signal_data->green_red_line_distance_editable, 0, -1));
  float green_to_red_line_distance = 0.0; 
  bool was_parse_successful = false;

  try {
    green_to_red_line_distance = std::stof(green_to_red_line_distance_input);
    was_parse_successful = true;
  } catch (const std::invalid_argument& e) {
    std::cerr << "Invalid input for green to red line distance: " << e.what() << std::endl;
  }

  if (!was_parse_successful) return;

  std::cout << "Distance from green to the red line: " << green_to_red_line_distance << " in." << std::endl;

  // Switch to the horizontal displacement calibration stack
  gtk_stack_set_visible_child_name(GTK_STACK(stack_widget), "horizontal_displacement_calibration_box");
}

void event_handlers::on_horizontal_displacement_continue_clicked(GtkWidget *widget, gpointer data)
{

  gtk_signal_data* signal_data = static_cast<gtk_signal_data*>(data);
  GtkWidget* stack_widget = signal_data->stack_widget;

  std::string horizontal_displacement_input(gtk_editable_get_chars(signal_data->horizontal_displacement_editable, 0, -1));
  float horizontal_displacement = 0.0; 
  bool was_parse_successful = false;

  try {
    horizontal_displacement = std::stof(horizontal_displacement_input);
    was_parse_successful = true;
  } catch (const std::invalid_argument& e) {
    std::cerr << "Invalid input for horizontal displacement: " << e.what() << std::endl;
  }

  if (!was_parse_successful) return;

  std::cout << "Horizontal displacement: " << horizontal_displacement << " in." << std::endl;

  // Switch to the vertical displacement calibration stack
  gtk_stack_set_visible_child_name(GTK_STACK(stack_widget), "vertical_displacement_calibration_box");
}

void event_handlers::on_vertical_displacement_continue_clicked(GtkWidget *widget, gpointer data)
{

  gtk_signal_data* signal_data = static_cast<gtk_signal_data*>(data);
  GtkWidget* stack_widget = signal_data->stack_widget;

  std::string vertical_displacement_input(gtk_editable_get_chars(signal_data->vertical_displacement_editable, 0, -1));
  float vertical_displacement = 0.0; 
  bool was_parse_successful = false;

  try {
    vertical_displacement = std::stof(vertical_displacement_input);
    was_parse_successful = true;
  } catch (const std::invalid_argument& e) {
    std::cerr << "Invalid input for vertical displacement: " << e.what() << std::endl;
  }

  if (!was_parse_successful) return;

  std::cout << "Vertical displacement: " << vertical_displacement << " in." << std::endl;

  // Switch to the measurements calibration stack
  gtk_stack_set_visible_child_name(GTK_STACK(stack_widget), "main_box");
}