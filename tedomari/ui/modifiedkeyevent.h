#ifndef UI__MODIFIEDKEYEVENT_H
#define UI__MODIFIEDKEYEVENT_H

#include "ui/modifiedkey.h"

namespace tedomari {
namespace ui {

class ModifiedKeyEvent {
  public:
    ModifiedKeyEvent(const String& name);
    ModifiedKeyEvent(Key k, const std::set<String>& mod, bool d) :
      modifiedKey(k, mod), down(d) {}
  private:
    ModifiedKey modifiedKey;
    bool down;
  public:
    inline bool operator==(const ModifiedKeyEvent& op) const {
      return modifiedKey == op.modifiedKey && down == op.down;
    }
    inline const ModifiedKey& getModifiedKey() const { return modifiedKey; }
    inline bool isKeyDown() const { return down; }
    inline bool isKeyUp() const { return !down; }
};

class ModifiedKeyEventHash {
  private:
    ModifiedKeyHash keyHasher;
  public:
    inline size_t operator()(const ModifiedKeyEvent& k) const {
      size_t hash = keyHasher(k.getModifiedKey());
      if (k.isKeyDown()) {
        hash *= 7;
      }
      return hash;
    }
};

}}

#endif // UI__MODIFIEDKEYEVENT_H

