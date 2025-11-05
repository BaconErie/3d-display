#include "event_handlers.hpp"

void event_handlers::on_calibrate_button_clicked (GtkWidget *widget, gpointer _)
{
  // Switch to the calibration stack first
  shared::is_current_cv_action_face = false;
  gtk_stack_set_visible_child_name(shared::stack_widget, "fov_calibration_box");
}

void event_handlers::on_fov_calibration_capture_clicked(GtkWidget *widget, gpointer _)
{
  gtk_stack_set_visible_child_name(shared::stack_widget, "measurements_calibration_box");
  shared::is_current_cv_action_face = true;
}

void event_handlers::on_measurements_continue_clicked(GtkWidget *widget, gpointer _)
{

  std::string qr_code_distance_input(gtk_editable_get_chars(shared::qr_code_distance_editable, 0, -1));
  bool was_parse_successful = false;

  try {
    parameters::qr_code_distance = std::stof(qr_code_distance_input);
    was_parse_successful = true;
  } catch (const std::invalid_argument& e) {
    std::cerr << "Invalid input for QR code distance: " << e.what() << std::endl;
  }

  if (!was_parse_successful) return;

  // Get the lenticule density

  std::string lenticule_density_input(gtk_editable_get_chars(shared::lenticule_density_editable, 0, -1));
  float lenticule_density = 0.0; 
  was_parse_successful = false;

  try {
    lenticule_density = std::stof(lenticule_density_input);
    was_parse_successful = true;
  } catch (const std::invalid_argument& e) {
    std::cerr << "Invalid input for lenticule density: " << e.what() << std::endl;
  }

  if (!was_parse_successful) return;

  float qr_code_angular_size = std::atan2(QR_CODE_WIDTH/2, parameters::qr_code_distance) * 2 *(180.0/3.141592653589793238463);
  parameters::webcam_fov_deg = qr_code_angular_size * parameters::qr_code_inverse_proportion;

  std::cout << "QR Code distance: " << parameters::qr_code_distance << " in." << std::endl;
  std::cout << "Webcam FOV: " << qr_code_angular_size << " degrees" << std::endl; 
  std::cout << "Lenticule density: " << lenticule_density << " LPI" << std::endl;

  // Switch back to the main calibration stack
  gtk_stack_set_visible_child_name(shared::stack_widget, "display_density_calibration_box");
}

void event_handlers::on_display_density_continue_clicked(GtkWidget *widget, gpointer _)
{

  std::string green_to_red_line_distance_input(gtk_editable_get_chars(shared::green_red_line_distance_editable, 0, -1));
  bool was_parse_successful = false;

  try {
    parameters::green_to_red_line_distance = std::stof(green_to_red_line_distance_input);
    was_parse_successful = true;
  } catch (const std::invalid_argument& e) {
    std::cerr << "Invalid input for green to red line distance: " << e.what() << std::endl;
  }

  if (!was_parse_successful) return;

  std::cout << "Distance from green to the red line: " << parameters::green_to_red_line_distance << " in." << std::endl;

  // Switch to the horizontal displacement calibration stack
  gtk_stack_set_visible_child_name(shared::stack_widget, "horizontal_displacement_calibration_box");
}

void event_handlers::on_horizontal_displacement_continue_clicked(GtkWidget *widget, gpointer _)
{
  std::string horizontal_displacement_input(gtk_editable_get_chars(shared::horizontal_displacement_editable, 0, -1));
  bool was_parse_successful = false;

  try {
    parameters::horizontal_displacement = std::stof(horizontal_displacement_input);
    was_parse_successful = true;
  } catch (const std::invalid_argument& e) {
    std::cerr << "Invalid input for horizontal displacement: " << e.what() << std::endl;
  }

  if (!was_parse_successful) return;

  std::cout << "Horizontal displacement: " << parameters::horizontal_displacement << " in." << std::endl;

  // Switch to the vertical displacement calibration stack
  gtk_stack_set_visible_child_name(shared::stack_widget, "vertical_displacement_calibration_box");
}

void event_handlers::on_vertical_displacement_continue_clicked(GtkWidget *widget, gpointer _)
{
  std::string vertical_displacement_input(gtk_editable_get_chars(shared::vertical_displacement_editable, 0, -1));
  bool was_parse_successful = false;

  try {
    parameters::vertical_displacement = std::stof(vertical_displacement_input);
    was_parse_successful = true;
  } catch (const std::invalid_argument& e) {
    std::cerr << "Invalid input for vertical displacement: " << e.what() << std::endl;
  }

  if (!was_parse_successful) return;

  std::cout << "Vertical displacement: " << parameters::vertical_displacement << " in." << std::endl;

  // Switch to the measurements calibration stack
  gtk_stack_set_visible_child_name(shared::stack_widget, "main_box");
}