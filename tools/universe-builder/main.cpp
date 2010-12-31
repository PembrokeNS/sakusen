#include <iostream>

#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>

#include <sakusen/resources/fileresourceinterface.h>
#include <sakusen/fileutils.h>

#include "ui.h"

using namespace std;
using namespace boost::filesystem;
using namespace sakusen;
using namespace sakusen::resources;
using namespace universeBuilder;

int main(int argc, char *argv[])
{
  ResourceInterface::Ptr resourceInterface(
      FileResourceInterface::create(
        fileUtils_configDirectory()/SAKUSEN_RESOURCES_SUBDIR,
        false
      )
    );
  Gtk::Main kit(argc, argv);
  vector<path> potentialGladeDirs;
  // Current directory
  potentialGladeDirs.push_back(path());
  // Directory of executable
  potentialGladeDirs.push_back(path(argv[0]).parent_path());
  BOOST_FOREACH(const path& p, potentialGladeDirs) {
    path gladeFile = p / "universe-builder.glade";
    if (exists(gladeFile)) {
      Glib::RefPtr<Gnome::Glade::Xml> refXml =
        Gnome::Glade::Xml::create(gladeFile.file_string());
  
      UI ui(refXml, resourceInterface);

      return 0;
    }
  }

  cerr << "Unable to find glade file.  Following directories were searched:\n";
  BOOST_FOREACH(const path& p, potentialGladeDirs) {
    cerr << p.directory_string() << '\n';
  }
  return 1;
}

