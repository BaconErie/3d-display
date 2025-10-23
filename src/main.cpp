#include <gtk/gtk.h>
#include <glibmm.h>

#include "gtk_signal_data.hpp"

#include <boost/process.hpp>
#include <iostream>
#include <thread>
#include <chrono>
#include "event_handlers.hpp"

void request_cv_process_update(void *user_data, Glib::Dispatcher* dispatcher) {
  while (true) {
    // Implementation to request an update from the cv process
    gtk_signal_data* data = static_cast<gtk_signal_data*>(user_data);

    boost::process::opstream* to_cv_pipe = data->to_cv_pipe;
    boost::process::ipstream* from_cv_pipe = data->from_cv_pipe;
    
    *to_cv_pipe << "face" << std::endl;
    
    std::string line;
    
    std::getline(*from_cv_pipe, line);

    if (line != "done") {
      std::cout<< "Line from cv process was not 'done', it was: " << line << std::endl;
      return;
    }

    dispatcher->emit();
  }
}

void update_webcam_image(void *user_data) {  

  gtk_signal_data* data = static_cast<gtk_signal_data*>(user_data);
  GtkWidget* main_webcam_image = data->main_webcam_image;
  GtkWidget* fov_webcam_image = data->fov_webcam_image;

  GFile *image_file = g_file_new_for_path("/home/eric/3d-display/programs/program/build/Window.png");

  gtk_picture_set_file(GTK_PICTURE(main_webcam_image), image_file);
  gtk_picture_set_file(GTK_PICTURE(fov_webcam_image), image_file);

  if (gtk_picture_get_file(GTK_PICTURE(main_webcam_image)) == NULL) {
    std::cout << "Warning: The main webcam image file is NULL!" << std::endl;
  }
  if (gtk_picture_get_file(GTK_PICTURE(fov_webcam_image)) == NULL) {
    std::cout << "Warning: The FOV webcam image file is NULL!" << std::endl;
  }


  g_object_unref(image_file);
}

// Signal handler for the button click
static void
on_hello_button_clicked (GtkWidget *widget,
                         gpointer   data)
{
  g_print ("Hello from UI file!\n");
}

static void
activate (GtkApplication *app,
          void        *user_data)
{
  GtkBuilder *builder;
  GtkWidget *window;
  GtkCssProvider *css_provider;
  GError *error = NULL;

  // Create a builder and load the UI file
  builder = gtk_builder_new ();
  if (!gtk_builder_add_from_file (builder, "ui/main.ui", &error)) {
    g_critical ("Error loading UI file: %s", error->message);
    g_error_free (error);
    return;
  }

  // Get the cv_process from user_data
  gtk_signal_data* data = static_cast<gtk_signal_data*>(user_data);
  boost::process::child& cv_process = *(data->cv_process);
  boost::process::opstream& to_cv_pipe = *(data->to_cv_pipe);
  boost::process::ipstream& from_cv_pipe = *(data->from_cv_pipe);
  GtkWidget *main_webcam_image = data->main_webcam_image;
  GtkWidget *fov_webcam_image = data->fov_webcam_image;

  data->builder = builder;

  cv_process = boost::process::child(
    "./cv", 
    boost::process::std_out > from_cv_pipe, 
    boost::process::std_err > boost::process::null,
    boost::process::std_in < to_cv_pipe
  );
  if (!cv_process.running()) {
    std::cerr << "Error: Could not start cv process." << std::endl;
    return;
  }

  to_cv_pipe << "face" << std::endl;



  // Get the main window from the builder
  window = GTK_WIDGET (gtk_builder_get_object (builder, "main_window"));
  gtk_window_set_application (GTK_WINDOW (window), app);

  
  // Get the horizontal_displacement_diagram picture widget and set its file
  GtkWidget *horizontal_displacement_diagram = GTK_WIDGET (gtk_builder_get_object (builder, "horizontal_displacement_diagram"));
  if (horizontal_displacement_diagram) {
    GFile *image_file = g_file_new_for_path("/home/eric/3d-display/programs/program/horizontal-displacement-diagram.png");
    gtk_picture_set_file(GTK_PICTURE(horizontal_displacement_diagram), image_file);
    g_object_unref(image_file);
  } else {
    g_warning("Could not find diagram picture widget");
  }

  // Get the vertical_displacement_diagram picture widget and set its file
  GtkWidget *vertical_displacement_diagram = GTK_WIDGET (gtk_builder_get_object (builder, "vertical_displacement_diagram"));
  if (vertical_displacement_diagram) {
    GFile *image_file = g_file_new_for_path("/home/eric/3d-display/programs/program/vertical-displacement-diagram.png");
    gtk_picture_set_file(GTK_PICTURE(vertical_displacement_diagram), image_file);
    g_object_unref(image_file);
  } else {
    g_warning("Could not find diagram picture widget");
  }

  // FOV Webcam Image Placeholder
  fov_webcam_image = GTK_WIDGET (gtk_builder_get_object (builder, "fov_webcam_image"));
  if (fov_webcam_image) {
    // Store in the struct so other functions can access it
    data->fov_webcam_image = fov_webcam_image;
    
    GFile *image_file = g_file_new_for_path("/home/eric/3d-display/programs/program/fov-webcam-placeholder.jpg");
    gtk_picture_set_file(GTK_PICTURE(fov_webcam_image), image_file);
    g_object_unref(image_file);
  } else {
    g_warning("Could not find FOV webcam image widget");
  }

  // Main Webcam Image Placeholder
  main_webcam_image = GTK_WIDGET (gtk_builder_get_object (builder, "main_webcam_image"));
  if (main_webcam_image) {
    // Store in the struct so other functions can access it
    data->main_webcam_image = main_webcam_image;
    
    GFile *image_file = g_file_new_for_path("/home/eric/3d-display/programs/program/build/Window.png");
    gtk_picture_set_file(GTK_PICTURE(main_webcam_image), image_file);
    g_object_unref(image_file);
  } else {
    g_warning("Could not find Main webcam image widget");
  }



  // Using C++ threading
  Glib::Dispatcher *dispatcher = data->dispatcher;

  dispatcher->connect([user_data]() {
      update_webcam_image(user_data);
  });

  std::thread webcam_update_thread(request_cv_process_update, user_data, dispatcher);

  webcam_update_thread.detach();

  // // Using GTK idle add to periodically update the webcam image
  // g_idle_add(G_SOURCE_FUNC(update_webcam_image), user_data);


  // Get the CSS provider
  css_provider = gtk_css_provider_new();
  gtk_css_provider_load_from_path(css_provider, "./ui/style.css");

  gtk_style_context_add_provider_for_display(gtk_widget_get_display(window),
                                            GTK_STYLE_PROVIDER(css_provider),
                                            GTK_STYLE_PROVIDER_PRIORITY_USER);

  // Set the stack pointer
  data->stack_widget = GTK_WIDGET(gtk_builder_get_object(builder, "main_stack"));

  // Connect signal for buttons
  GtkWidget *calibrate_button = GTK_WIDGET(gtk_builder_get_object(builder, "calibrate_button"));
  GtkWidget *fov_calibration_capture_button = GTK_WIDGET(gtk_builder_get_object(builder, "fov_calibration_capture_button"));
  GtkWidget *display_density_continue_button = GTK_WIDGET(gtk_builder_get_object(builder, "display_density_continue_button"));
  GtkWidget *horizontal_displacement_continue_button = GTK_WIDGET(gtk_builder_get_object(builder, "horizontal_displacement_continue_button"));
  GtkWidget *vertical_displacement_continue_button = GTK_WIDGET(gtk_builder_get_object(builder, "vertical_displacement_continue_button"));
  GtkWidget *measurements_continue_button = GTK_WIDGET(gtk_builder_get_object(builder, "measurements_continue_button"));

  g_signal_connect(calibrate_button, "clicked", G_CALLBACK(event_handlers::on_calibrate_button_clicked), user_data);
  g_signal_connect(fov_calibration_capture_button, "clicked", G_CALLBACK(event_handlers::on_fov_calibration_capture_clicked), user_data);
  g_signal_connect(display_density_continue_button, "clicked", G_CALLBACK(event_handlers::on_display_density_continue_clicked), user_data);
  g_signal_connect(horizontal_displacement_continue_button, "clicked", G_CALLBACK(event_handlers::on_horizontal_displacement_continue_clicked), user_data);
  g_signal_connect(vertical_displacement_continue_button, "clicked", G_CALLBACK(event_handlers::on_vertical_displacement_continue_clicked), user_data);
  g_signal_connect(measurements_continue_button, "clicked", G_CALLBACK(event_handlers::on_measurements_continue_clicked), user_data);

  // Set up the entry pointers
  data->qr_code_distance_editable = GTK_EDITABLE(gtk_builder_get_object(builder, "qr_code_distance_entry"));
  data->lenticule_density_editable = GTK_EDITABLE(gtk_builder_get_object(builder, "lenticule_density_entry"));
  data->green_red_line_distance_editable = GTK_EDITABLE(gtk_builder_get_object(builder, "green_red_line_distance_entry"));
  data->horizontal_displacement_editable = GTK_EDITABLE(gtk_builder_get_object(builder, "horizontal_displacement_entry"));
  data->vertical_displacement_editable = GTK_EDITABLE(gtk_builder_get_object(builder, "vertical_displacement_entry"));


  // Show the window

  gtk_window_present (GTK_WINDOW (window));

  // Don't clean up builder
  // // Clean up 
  // g_object_unref (builder);

}

static void deactivate(GtkApplication *app, void *data) {
  std::cout << "Deactivate triggered. Cleaning up memory." << std::endl;
}

int
main (int    argc,
      char **argv)
{
  GtkApplication *app;
  boost::process::child cv_process;
  boost::process::opstream to_cv_pipe;
  boost::process::ipstream from_cv_pipe;
  Glib::Dispatcher dispatcher;

  gtk_signal_data data_for_gtk_signals;

  GMainLoop *main_loop;
  main_loop = g_main_loop_new(NULL, FALSE);

  data_for_gtk_signals.cv_process = &cv_process;
  data_for_gtk_signals.to_cv_pipe = &to_cv_pipe;
  data_for_gtk_signals.from_cv_pipe = &from_cv_pipe;
  data_for_gtk_signals.dispatcher = &dispatcher;

  int status;

  app = gtk_application_new ("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect (app, "activate", G_CALLBACK (activate), &data_for_gtk_signals);
  g_signal_connect (app, "shutdown", G_CALLBACK (deactivate), &data_for_gtk_signals);

  // GFile *image_file = g_file_new_for_path("/home/eric/3d-display/programs/program/build/Window.png");
  // gtk_picture_set_file(GTK_PICTURE(data_for_gtk_signals.main_webcam_image), image_file);

  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return status;
}
