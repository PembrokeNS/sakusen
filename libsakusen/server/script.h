#ifndef LIBSAKUSEN_SERVER__SCRIPT_H
#define LIBSAKUSEN_SERVER__SCRIPT_H

namespace sakusen {
namespace server {

/* \brief Scripts provide hooks at various events
 *
 * Typically a script will be provided by a Universe, to allow for things like
 * auto-layering of units, or by a MapTemplate, to allow for things like
 * victory conditions and other more unusual map-specific effects. */
class Script : boost::noncopyable {
  public:
    typedef boost::shared_ptr<Script> Ptr;
};

}}

#endif // LIBSAKUSEN_SERVER__SCRIPT_H

