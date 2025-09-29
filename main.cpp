#include <gtk/gtk.h>

// Signal handler for the button click
static void
on_hello_button_clicked (GtkWidget *widget,
                         gpointer   data)
{
  g_print ("Hello from UI file!\n");
}

static void
activate (GtkApplication *app,
          gpointer        user_data)
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
  GtkWidget *fov_webcam_image = GTK_WIDGET (gtk_builder_get_object (builder, "fov_webcam_image"));
  if (fov_webcam_image) {
    GFile *image_file = g_file_new_for_path("/home/eric/3d-display/programs/program/fov-webcam-placeholder.jpg");
    gtk_picture_set_file(GTK_PICTURE(fov_webcam_image), image_file);
    g_object_unref(image_file);
  } else {
    g_warning("Could not find FOV webcam image widget");
  }

  // Main Webcam Image Placeholder
  GtkWidget *main_webcam_image = GTK_WIDGET (gtk_builder_get_object (builder, "main_webcam_image"));
  if (main_webcam_image) {
    GFile *image_file = g_file_new_for_path("/home/eric/3d-display/programs/program/fov-webcam-placeholder.jpg");
    gtk_picture_set_file(GTK_PICTURE(main_webcam_image), image_file);
    g_object_unref(image_file);
  } else {
    g_warning("Could not find Main webcam image widget");
  }

  // Get the CSS provider
  css_provider = gtk_css_provider_new();
  gtk_css_provider_load_from_path(css_provider, "./ui/style.css");

  gtk_style_context_add_provider_for_display(gtk_widget_get_display(window),
                                            GTK_STYLE_PROVIDER(css_provider),
                                            GTK_STYLE_PROVIDER_PRIORITY_USER);

  // Show the window
  gtk_window_present (GTK_WINDOW (window));

  // Clean up
  g_object_unref (builder);
}

int
main (int    argc,
      char **argv)
{
  GtkApplication *app;
  int status;

  app = gtk_application_new ("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return status;
}
