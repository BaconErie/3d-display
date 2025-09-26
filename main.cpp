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
