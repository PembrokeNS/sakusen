#include <gtkmm.h>
#include <libglademm.h>

#include "ui.h"

using namespace universeBuilder;

int main(int argc, char *argv[])
{
  Gtk::Main kit(argc, argv);
  Glib::RefPtr<Gnome::Glade::Xml> refXml =
    Gnome::Glade::Xml::create("universe-builder.glade");

#define GET_WIDGET(type, name, variable)       \
  Gtk::type* variable = NULL;                  \
  refXml->get_widget(#name, variable);         \
  if (!variable) {                             \
    throw std::runtime_error("missing "#name); \
  }

  GET_WIDGET(Window, MainWindow, window)
  GET_WIDGET(Notebook, MainNotebook, notebook)
  
  UI ui;

  notebook->signal_switch_page().connect(sigc::mem_fun(ui, &UI::changePage));

  Gtk::Main::run(*window);

  return 0;
}

