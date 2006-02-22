#include "ui/key.h"

#include "gnu_extensions.h"
#include "stringutils.h"

using namespace sakusen;
using namespace __gnu_cxx;

namespace tedomari {
namespace ui {

hash_map<String, Key, StringHash> initializeKeyLookup()
{
  hash_map<String, Key, StringHash> l;
  /* TODO: lots more names of keys */
  l[";"] = K_SEMICOLON;
  return l;
}

hash_map<String, Key, StringHash> keyLookup = initializeKeyLookup();

Key getKey(const String& name)
{
  hash_map<String, Key, StringHash>::iterator key = keyLookup.find(name);
  if (key == keyLookup.end()) {
    return K_Unknown;
  } else {
    return key->second;
  }
}

}}

