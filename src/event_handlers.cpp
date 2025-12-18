#include "event_handlers.hpp"

void event_handlers::on_calibrate_button_clicked (GtkWidget *widget, gpointer _)
{
  // Switch to the calibration stack first
  shared_vars::is_current_cv_action_face = false;
  gtk_stack_set_visible_child_name(shared_vars::stack_widget, "fov_calibration_box");
}

void event_handlers::on_fov_calibration_capture_clicked(GtkWidget *widget, gpointer _)
{
  gtk_stack_set_visible_child_name(shared_vars::stack_widget, "measurements_calibration_box");
  shared_vars::is_current_cv_action_face = true;
}

void event_handlers::on_measurements_continue_clicked(GtkWidget *widget, gpointer _)
{
  std::string qr_code_distance_input(gtk_editable_get_chars(shared_vars::qr_code_distance_editable, 0, -1));
  bool was_parse_successful = false;

  try {
    parameters::qr_code_distance = std::stof(qr_code_distance_input);
    was_parse_successful = true;
  } catch (const std::invalid_argument& e) {
    std::cerr << "Invalid input for QR code distance: " << e.what() << std::endl;
  }

  if (!was_parse_successful) return;

  // Get the lenticule density

  std::string lenticule_density_input(gtk_editable_get_chars(shared_vars::lenticule_density_editable, 0, -1));
  was_parse_successful = false;

  try {
    parameters::lenticule_density = std::stof(lenticule_density_input);
    was_parse_successful = true;
  } catch (const std::invalid_argument& e) {
    std::cerr << "Invalid input for lenticule density: " << e.what() << std::endl;
  }

  if (!was_parse_successful) return;

  // Get the index of refraction
  std::string index_of_refraction_input(gtk_editable_get_chars(shared_vars::index_of_refraction_editable, 0, -1));
  was_parse_successful = false;

  try {
    parameters::index_of_refraction = std::stof(index_of_refraction_input);
    was_parse_successful = true;
  } catch (const std::invalid_argument& e) {
    std::cerr << "Invalid input for lenticule density: " << e.what() << std::endl;
  }

  if (!was_parse_successful) return;

  float qr_code_angular_size = std::atan2(QR_CODE_WIDTH/2, parameters::qr_code_distance) * 2 *(180.0/3.141592653589793238463);
  parameters::webcam_fov_deg = qr_code_angular_size * parameters::qr_code_inverse_proportion;

  std::cout << "QR Code distance: " << parameters::qr_code_distance << " in." << std::endl;
  std::cout << "Webcam FOV: " << qr_code_angular_size << " degrees" << std::endl; 
  std::cout << "Lenticule density: " << parameters::lenticule_density << " LPI" << std::endl;
  std::cout << "Index of refraction: " << parameters::index_of_refraction << std::endl;

  // Tell 3D renderer to display the measurement window
  std::vector<int64_t> message;
  message.push_back((int64_t)0);
  boost::asio::write(shared_vars::socket, boost::asio::buffer(message));

  // Switch to display density
  gtk_stack_set_visible_child_name(shared_vars::stack_widget, "display_density_calibration_box");
}

void event_handlers::on_display_density_continue_clicked(GtkWidget *widget, gpointer _)
{

  std::string green_to_red_line_distance_input(gtk_editable_get_chars(shared_vars::green_red_line_distance_editable, 0, -1));
  bool was_parse_successful = false;

  try {
    parameters::green_to_red_line_distance = std::stof(green_to_red_line_distance_input);
    was_parse_successful = true;
  } catch (const std::invalid_argument& e) {
    std::cerr << "Invalid input for green to red line distance: " << e.what() << std::endl;
  }

  if (!was_parse_successful) return;

  std::cout << "Distance from green to the red line: " << parameters::green_to_red_line_distance << " in." << std::endl;

  // Tell 3D renderer to hide the measurement window
  std::vector<int64_t> message;
  message.push_back((int64_t)1);
  boost::asio::write(shared_vars::socket, boost::asio::buffer(message));

  // Find pixels per lens, and send it to the 3D renderer
  // Also send the index of refraction
  float pixels_per_lens = 500.0 / parameters::green_to_red_line_distance / parameters::lenticule_density;
  std::cout << "Event handlers.cpp. Line 84. pixels_per_lens is " << pixels_per_lens << std::endl;
  boost::asio::write(shared_vars::socket, boost::asio::buffer({(int64_t)2}));
  boost::asio::write(shared_vars::socket, boost::asio::buffer({(float_t)pixels_per_lens}));
  boost::asio::write(shared_vars::socket, boost::asio::buffer({(float_t)parameters::index_of_refraction}));

  // Switch to the measurements calibration stack
  gtk_stack_set_visible_child_name(shared_vars::stack_widget, "main_box");
}

void event_handlers::on_start_display_clicked(GtkWidget *widget, gpointer _) {
  GtkButton *button = GTK_BUTTON(widget);

  gtk_button_set_label(button, "Loading 3D renderer...");
  gtk_widget_set_sensitive(widget, false);

  shared_vars::acceptor.open(shared_vars::endpoint.protocol());
  shared_vars::acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
  shared_vars::acceptor.bind(shared_vars::endpoint);
  shared_vars::acceptor.listen(1);

  std::thread t(shared_vars::listen_for_renderer_socket_and_call_dispatcher);
  t.detach();
}

void event_handlers::on_renderer_success() {
    shared_vars::is_renderer_active = true;
    gtk_button_set_label(GTK_BUTTON(gtk_builder_get_object(shared_vars::builder, "start_display_button")), "Display active");
}