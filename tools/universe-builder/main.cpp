#include <gtkmm.h>
#include <libglademm.h>

int main(int argc, char *argv[])
{
  Gtk::Main kit(argc, argv);
  Glib::RefPtr<Gnome::Glade::Xml> refXml =
    Gnome::Glade::Xml::create("universe-builder.glade");
  Gtk::Window* window = NULL;
  refXml->get_widget("MainWindow", window);
  if (window) {
    //Gtk::Window window;
    Gtk::Main::run(*window);
  } else {
    throw std::logic_error("missing MainWindow");
  }

  return 0;
}

