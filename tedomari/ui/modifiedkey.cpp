#include "modifiedkey.h"

using namespace std;

using namespace sakusen;
using namespace tedomari::ui;

ModifiedKey::ModifiedKey(const String& name) : key(K_Unknown), modifiers()
{
  list<String> components = stringUtils_split(name, "-");
  if (components.empty()) {
    return;
  }
  key = tedomari::ui::getKey(components.back());
  components.pop_back();
  
  if (key != K_Unknown) {
    for (list<String>::iterator modifier = components.begin();
        modifier != components.end(); ++modifier) {
      modifiers.insert(*modifier);
    }
  }
}

