#ifndef LIBSAKUSEN__MATERIELPROVIDER_H
#define LIBSAKUSEN__MATERIELPROVIDER_H

namespace sakusen {

/** \brief Interface to a thing that provides materiel (aka resources).
 */
class MaterielProvider {
  protected:
    inline virtual ~MaterielProvider() = 0;
  public:
    /** \name Materiel request functions
     *
     * For each of energy and metal, you can request an amount, and the amount
     * you actually get (between 0 and the amount requested, inclusive) is
     * returned. */
    //@{
    virtual uint32 requestEnergy(uint32 amount) = 0;
    virtual uint32 requestMetal(uint32 amount) = 0;
    //@}
};

inline MaterielProvider::~MaterielProvider() {}

}

#endif // LIBSAKUSEN__MATERIELPROVIDER_H

