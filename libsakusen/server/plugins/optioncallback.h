#ifndef LIBSAKUSEN_SERVER_PLUGINS__OPTIONCALLBACK_H
#define LIBSAKUSEN_SERVER_PLUGINS__OPTIONCALLBACK_H

#include <boost/call_traits.hpp>

namespace sakusen {
namespace server {
namespace plugins {

/** \brief Interface for callbacks to plugins when options that they register
 * are altered */
template<typename T>
class OptionCallback {
  protected:
    OptionCallback() {}
    virtual ~OptionCallback() {}
  public:
    /** \brief Callback used when option value changed.
     *
     * The server calls this callback when it wants to inform a plugin that the
     * corresponding option value has been changed.
     *
     * \param name   Name of option.
     * \param newVal New value of option.
     * \return Empty string if everything is OK, otherwise a message describing
     * what has gone wrong. */
    virtual String setOption(
        const String& name,
        typename boost::call_traits<T>::param_type newVal
      ) = 0;
};

}}}

#endif // LIBSAKUSEN_SERVER_PLUGINS__OPTIONCALLBACK_H

