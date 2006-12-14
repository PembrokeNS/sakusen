#ifndef IREFERENT_H
#define IREFERENT_H

namespace sakusen {

class IRef;

/** \brief Base class for all classes which need to be the target of a Ref
 *
 * The reason for this class is to provide a common basis for comparisons.  For
 * example, if we have
 *
 * \code
 * class Foo {}
 * class Bar {}
 * class Baz : Foo, Bar {}
 * \endcode
 * 
 * then a Baz* and Bar* to the same object will have different values.
 * Thus, without knowing about the inheritance, the generic code can't tell
 * that corresponding Ref<Baz> and Ref<Bar> are Refs to the same object.  Since
 * the hash_maps involved in this system are keyed by pointer, this would be a
 * problem.
 *
 * However, if all of them inherit from a common base class, like this:
 *
 * \code
 * class Foo : virtual IReferent {}
 * class Bar : virtual IReferent {}
 * class Baz : Foo, Bar {}
 * \endcode
 * 
 * then we can always cast to an IReferent* and compare those.
 */
class IReferent {
  protected:
    IReferent() {}
    IReferent(const IReferent&) {}
  public:
    virtual ~IReferent() {}
    /** \brief Allow an object to obtain a Ref to itself
     *
     * When an object is added to an IRefContainer, this method should be
     * called, passing a reference to that object.  Thus, by overriding this
     * method, an object can obtain a Ref to itself, and pass that Ref out
     * to other parts of the code where it would otherwise pass 'this'.
     *
     * At time of writing, server::LayeredUnit makes use of this feature.
     * */
    virtual void supplyRef(const IRef&) {}
};

}

#endif // IREFERENT_H

