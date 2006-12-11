#ifndef IHEIGHTFIELD_H
#define IHEIGHTFIELD_H

namespace sakusen {

class IHeightfield {
  protected:
    IHeightfield() {}
    IHeightfield(const IHeightfield&) {}
    virtual ~IHeightfield() {}
  public:
    virtual sint32 getHeightAt(sint32 x, sint32 y) const = 0;
    inline sint32 getHeightAt(const Position& p) const {
      return getHeightAt(p.x, p.y);
    }
    virtual sint32 getMaxHeightIn(const Rectangle<sint32>&) const = 0;
};

}

#endif // IHEIGHTFIELD_H

