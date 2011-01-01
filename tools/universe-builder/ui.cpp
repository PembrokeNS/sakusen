#include "ui.h"

#include <sakusen/resourceinterface-methods.h>

#include <boost/lexical_cast.hpp>
#include <boost/filesystem/path.hpp>

using namespace std;
using namespace sakusen;

namespace universeBuilder {

UI::UI(Glib::RefPtr<Gnome::Glade::Xml> xml, ResourceInterface::Ptr rI) :
  resourceInterface(rI),
  currentPage(page_general)
{
#define GET_WIDGET(type, name)                 \
  Gtk::type* w##name = NULL;                   \
  xml->get_widget(#name, w##name);             \
  if (!w##name) {                              \
    throw std::runtime_error("missing "#name); \
  }

  GET_WIDGET(Window, MainWindow)
  GET_WIDGET(Notebook, MainNotebook)
  GET_WIDGET(Entry, InternalNameEntry)
  GET_WIDGET(Entry, ScriptModuleEntry)
  GET_WIDGET(Entry, ScriptFunctionEntry)
  GET_WIDGET(MenuItem, MenuFileNew)
  GET_WIDGET(MenuItem, MenuFileOpen)
  GET_WIDGET(MenuItem, MenuFileSave)

  /* Save pointers to widgets we'll need later */
  window = wMainWindow;
  internalNameEntry = wInternalNameEntry;
  scriptModuleEntry = wScriptModuleEntry;
  scriptFunctionEntry = wScriptFunctionEntry;

  /* Connect signals from widgets */
  wMainNotebook->signal_switch_page().
    connect(sigc::mem_fun(this, &UI::changePage));
  wMenuFileNew->signal_activate().
    connect(sigc::mem_fun(this, &UI::newUniverse));
  wMenuFileOpen->signal_activate().
    connect(sigc::mem_fun(this, &UI::openUniverse));
  wMenuFileSave->signal_activate().
    connect(sigc::mem_fun(this, &UI::saveUniverse));

  /* Run the application */
  Gtk::Main::run(*wMainWindow);
}

void UI::changePage(GtkNotebookPage*, guint pageNum)
{
  storeCurrentPage();
  currentPage = static_cast<Page>(pageNum);
  updateCurrentPage();
}

void UI::newUniverse()
{
  /** \todo Check for changes and offer warning of their loss */
  universe.reset(new Universe(
        "NewUniverse",
        "", // hash
        "", // scriptModule
        "", // scriptFunction
        "", // playerDataFunction
        vector<WeaponType>(),
        vector<UnitType>(),
        10, // logMinSpatialNoiseScale
        15  // logMaxSpatialNoiseScale
      ));
  /** \todo Set universe path */
  updateCurrentPage();
}

void UI::openUniverse()
{
  Gtk::FileChooserDialog dialog("Open universe",
          Gtk::FILE_CHOOSER_ACTION_OPEN);
  dialog.set_transient_for(*window);

  dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
  dialog.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);

  Gtk::FileFilter filter_text;
  filter_text.set_name("Sakusen universe files");
  filter_text.add_mime_type("application/x-sakusen");
  dialog.add_filter(filter_text);

  Gtk::FileFilter filter_any;
  filter_any.set_name("Any files");
  filter_any.add_pattern("*");
  dialog.add_filter(filter_any);

  boost::filesystem::path filename;
  {
    //Show the dialog and wait for a user response:
    int result = dialog.run();

    //Handle the response:
    switch(result)
    {
      case(Gtk::RESPONSE_OK):
        filename = dialog.get_filename();
        break;
      case(Gtk::RESPONSE_CANCEL):
        return;
      default:
        throw logic_error("unexpected button");
    }
  }

  /** \todo Check for changes and offer warning of their loss */

  Universe::Ptr loading;
  ResourceSearchResult result;
  String loadingPath;
  boost::tie(loading, result, loadingPath) =
    resourceInterface->search<Universe>(filename.leaf());
  switch (result) {
    default:
      throw logic_error(
          "invalid enumeration value"+boost::lexical_cast<String>(result)
        );
  }
  universe = loading;
  universePath = loadingPath;
  updateCurrentPage();
}

void UI::saveUniverse()
{
  storeCurrentPage();

  if (!universe) {
    SAKUSEN_DEBUG("no universe");
    /** \todo message box */
    return;
  }

  if (!universe->sanityCheck()) {
    SAKUSEN_DEBUG("sanity check failed");
    /** \todo message box */
    return;
  }

  bool success = resourceInterface->save(universe, universePath);
  if (!success) {
    SAKUSEN_DEBUG("save failed");
    /** \todo message box */
  }
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
  SAKUSEN_FATAL("not implemented");
}

void UI::storeUnitTypesPage()
{
  SAKUSEN_FATAL("not implemented");
}

void UI::storeWeaponTypesPage()
{
  SAKUSEN_FATAL("not implemented");
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
  SAKUSEN_FATAL("not implemented");
}

void UI::updateUnitTypesPage()
{
  SAKUSEN_FATAL("not implemented");
}

void UI::updateWeaponTypesPage()
{
  SAKUSEN_FATAL("not implemented");
}

}

