#ifndef UNIVERSE_BUILDER__UI_H
#define UNIVERSE_BUILDER__UI_H

#include <gtkmm.h>
#include <libglademm.h>

#include <sakusen/universe.h>

namespace universeBuilder {

class UI {
  public:
    UI(Glib::RefPtr<Gnome::Glade::Xml>, sakusen::ResourceInterface::Ptr);
  private:
    void changePage(GtkNotebookPage* page, guint pageNum);

    void newUniverse();
    void openUniverse();
    void saveUniverse();

    void storeCurrentPage();
    void storeGeneralPage();
    void storeUnitTypesPage();
    void storeWeaponTypesPage();
    
    void updateCurrentPage();
    void updateGeneralPage();
    void updateUnitTypesPage();
    void updateWeaponTypesPage();

    Gtk::Window* window;
    Gtk::Entry* internalNameEntry;
    Gtk::Entry* scriptModuleEntry;
    Gtk::Entry* scriptFunctionEntry;

    sakusen::ResourceInterface::Ptr resourceInterface;
    sakusen::Universe::Ptr universe;
    sakusen::String universePath;

    enum Page {
      page_general,
      page_unitTypes,
      page_weaponTypes,
      page_max
    };

    Page currentPage;
};

}

#endif // UNIVERSE_BUILDER__UI_H

