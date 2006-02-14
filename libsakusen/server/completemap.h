#ifndef COMPLETEMAP_H
#define COMPLETEMAP_H

#include "map.h"

namespace sakusen {
namespace server {

class CompleteMap : public Map {
  private:
    CompleteMap();
  protected:
    CompleteMap(const Map&);
    CompleteMap(const MapTemplate&);
  public:
    virtual ~CompleteMap() {}
  private:
    Heightfield heightfield;
  public:
    static Map* newMap(const MapTemplate& t);
};

}}

#endif // COMPLETEMAP_H

