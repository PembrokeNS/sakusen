#include "ui.h"

using namespace std;

namespace universeBuilder {

UI::UI() :
  currentPage(page_general)
{
}

void UI::changePage(GtkNotebookPage*, guint pageNum)
{
  storeCurrentPage();
  currentPage = static_cast<Page>(pageNum);
  updateCurrentPage();
}

void UI::storeCurrentPage()
{
  if (!universe) {
    return;
  }

  SAKUSEN_STATIC_ASSERT(page_max == 3);
    
  switch(currentPage) {
    case page_general:
      storeGeneralPage();
      break;
    case page_unitTypes:
      storeUnitTypesPage();
      break;
    case page_weaponTypes:
      storeWeaponTypesPage();
      break;
    default:
      throw logic_error("unhandled enum value");
  }
}

void UI::storeGeneralPage()
{
  Fatal("not implemented");
}

void UI::storeUnitTypesPage()
{
  Fatal("not implemented");
}

void UI::storeWeaponTypesPage()
{
  Fatal("not implemented");
}

void UI::updateCurrentPage()
{
  if (!universe) {
    return;
  }

  SAKUSEN_STATIC_ASSERT(page_max == 3);
    
  switch(currentPage) {
    case page_general:
      updateGeneralPage();
      break;
    case page_unitTypes:
      updateUnitTypesPage();
      break;
    case page_weaponTypes:
      updateWeaponTypesPage();
      break;
    default:
      throw logic_error("unhandled enum value");
  }
}

void UI::updateGeneralPage()
{
  Fatal("not implemented");
}

void UI::updateUnitTypesPage()
{
  Fatal("not implemented");
}

void UI::updateWeaponTypesPage()
{
  Fatal("not implemented");
}

}

