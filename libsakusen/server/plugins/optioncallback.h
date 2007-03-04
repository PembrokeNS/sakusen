#ifndef OPTIONCALLBACK_H
#define OPTIONCALLBACK_H

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
    virtual String setOption(
        const String& name,
        typename boost::call_traits<T>::param_type newVal
      ) = 0;
};

}}}

#endif // OPTIONCALLBACK_H

