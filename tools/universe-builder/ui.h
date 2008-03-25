#ifndef UNIVERSE_BUILDER__UI_H
#define UNIVERSE_BUILDER__UI_H

#include <gtkmm.h>

#include "universe.h"

namespace universeBuilder {

class UI {
  public:
    UI();

    void changePage(GtkNotebookPage* page, guint pageNum);
  private:
    void storeCurrentPage();
    void storeGeneralPage();
    void storeUnitTypesPage();
    void storeWeaponTypesPage();
    
    void updateCurrentPage();
    void updateGeneralPage();
    void updateUnitTypesPage();
    void updateWeaponTypesPage();

    Gtk::Entry* internalNameEntry;
    Gtk::Entry* scriptModuleEntry;
    Gtk::Entry* scriptFunctionEntry;

    sakusen::Universe::Ptr universe;

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

