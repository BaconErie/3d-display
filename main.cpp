#include <gtk/gtk.h>
#include <glibmm.h>
#include <boost/process.hpp>
#include <iostream>
#include <boost/interprocess/shared_memory_object.hpp>
#include <thread>
#include <chrono>

struct gtk_signal_data {
    boost::process::child *cv_process;
    boost::process::opstream *to_cv_pipe;
    boost::process::ipstream *from_cv_pipe;
    boost::interprocess::shared_memory_object *shared_memory;
    GtkWidget *main_webcam_image;
    GtkWidget *fov_webcam_image;
    bool main_webcam_image_set = false;
    bool fov_webcam_image_set = false;
    bool is_red = false;
    Glib::Dispatcher *dispatcher;
};

void request_cv_process_update(void *user_data, Glib::Dispatcher* dispatcher) {
  while (true) {
    // Implementation to request an update from the cv process
    gtk_signal_data* data = static_cast<gtk_signal_data*>(user_data);
    //std::cout<< "Done casting user_data to gtk_signal_data." << std::endl;

    //std::cout<< "Extracting cv_pipe from data." << std::endl;
    boost::process::opstream* to_cv_pipe = data->to_cv_pipe;
    boost::process::ipstream* from_cv_pipe = data->from_cv_pipe;
    //std::cout<< "Done extracting cv_pipe from data." << std::endl;

    //std::cout<< "Update webcam image called. " << std::flush;

    //std::cout<< "The status of the pipe is " << to_cv_pipe->good() << std::endl;

    //std::cout<< "Sending face command to cv process." << std::endl;
    *to_cv_pipe << "face" << std::endl;
    //std::cout<< "Sent face command to cv process." << std::endl;

    //std::cout<< "Currently, the status of the pipe is " << from_cv_pipe->good() << " " << to_cv_pipe->good() << std::endl;

    std::string line;
    //std::cout<< "Reading line from cv process. The line is initially " << line << std::endl;
    std::getline(*from_cv_pipe, line);
    //std::cout<< "Read line from cv process: " << line << std::endl;

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

  GFile *image_file = g_file_new_for_path("/home/eric/3d-display/programs/program/build/Window.png");

  // //std::cout<< "Here's the uh image file: " << image_file << std::endl;
  // //std::cout<< "And is that equal to the null pointer? " << (image_file == NULL) << std::endl;

  //std::cout<< "Done creating GFile for the image" << std::endl;

  //std::cout<< "Setting the image file for the main webcam image." << std::endl;
  gtk_picture_set_file(GTK_PICTURE(main_webcam_image), image_file);
  //std::cout<< "Done setting the image file for the main webcam image." << std::endl;

  //std::cout<< "Here's what's being displayed by the main webcam image: " << gtk_picture_get_file(GTK_PICTURE(main_webcam_image)) << std::endl;
  //std::cout<< "And is that equal to the null pointer? " << (gtk_picture_get_file(GTK_PICTURE(main_webcam_image)) == NULL) << std::endl;

  if (gtk_picture_get_file(GTK_PICTURE(main_webcam_image)) == NULL) {
    std::cout << "Warning: The main webcam image file is NULL!" << std::endl;
  }

  //std::cout<< "Unreferencing the GFile." << std::endl;
  g_object_unref(image_file);
  //std::cout<< "Done unreferencing the GFile." << std::endl;

  //std::cout<< "End of update_webcam_image.\n\n\n" << std::endl;
}

void testing_idle_callback() {
    std::cout << "Idle callback triggered!" << std::endl;
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

  std::cout << "Starting cv process..." << std::endl;
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
  std::cout << "Done starting." << std::endl;

  to_cv_pipe << "face" << std::endl;
  std::cout << "OK THIS IS THE STATUS OF CV_PIPE " << from_cv_pipe.good() << std::endl;

  // Set up the shared memory
  boost::interprocess::shared_memory_object& shm_obj = *(data->shared_memory);

  shm_obj = boost::interprocess::shared_memory_object(boost::interprocess::open_or_create, "mi3ku_cv_shm", boost::interprocess::read_write);



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

  std::cout << "Idle adding the update webcam image" << from_cv_pipe.good() << std::endl;

  // Using C++ threading
  Glib::Dispatcher *dispatcher = data->dispatcher;

  std::cout << "Getting the dispatcher" << std::endl;

  dispatcher->connect([user_data]() {
      update_webcam_image(user_data);
  });

  std::cout << "Connected dispatcher" << std::endl;

  std::cout << "Trying to create the thread" << std::endl;

  std::thread webcam_update_thread(request_cv_process_update, user_data, dispatcher);

  std::cout << "Done creating thread" << std::endl;
  std::cout << "Detaching thread" << std::endl;

  webcam_update_thread.detach();

  std::cout << "Done creating the thread" << std::endl;

  // // Using GTK idle add to periodically update the webcam image
  // g_idle_add(G_SOURCE_FUNC(update_webcam_image), user_data);


  // Get the CSS provider
  css_provider = gtk_css_provider_new();
  gtk_css_provider_load_from_path(css_provider, "./ui/style.css");

  gtk_style_context_add_provider_for_display(gtk_widget_get_display(window),
                                            GTK_STYLE_PROVIDER(css_provider),
                                            GTK_STYLE_PROVIDER_PRIORITY_USER);

  // Show the window
  std::cout << "Showing window" << std::endl;
  std::cout << cv_process.id() << std::endl;
  std::cout << cv_process.running() << std::endl;
  gtk_window_present (GTK_WINDOW (window));
  std::cout << "Done showing window" << std::endl;

  // Clean up
  g_object_unref (builder);

  std::cout << "Clean up." << std::endl;
  std::cout << cv_process.id() << std::endl;
  std::cout << cv_process.running() << std::endl;

  std::cout << "OK THIS IS THE STATUS OF CV_PIPE AT THE END OF THING " << to_cv_pipe.good() << std::endl;
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
  boost::interprocess::shared_memory_object shared_memory;
  Glib::Dispatcher dispatcher;

  gtk_signal_data data_for_gtk_signals;

  GMainLoop *main_loop;
  main_loop = g_main_loop_new(NULL, FALSE);

  //g_source_new();

  std::cout << "Done with idle add." << std::endl;

  data_for_gtk_signals.cv_process = &cv_process;
  data_for_gtk_signals.to_cv_pipe = &to_cv_pipe;
  data_for_gtk_signals.from_cv_pipe = &from_cv_pipe;
  data_for_gtk_signals.shared_memory = &shared_memory;
  data_for_gtk_signals.dispatcher = &dispatcher;

  int status;

  app = gtk_application_new ("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect (app, "activate", G_CALLBACK (activate), &data_for_gtk_signals);
  g_signal_connect (app, "shutdown", G_CALLBACK (deactivate), &data_for_gtk_signals);
  std::cout << "Done with signal connect." << std::endl;

  // GFile *image_file = g_file_new_for_path("/home/eric/3d-display/programs/program/build/Window.png");
  // gtk_picture_set_file(GTK_PICTURE(data_for_gtk_signals.main_webcam_image), image_file);

  status = g_application_run (G_APPLICATION (app), argc, argv);
  std::cout << "Done running application." << std::endl;
  g_object_unref (app);

  return status;
}
