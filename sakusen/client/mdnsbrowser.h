#ifndef LIBSAKUSEN_CLIENT__MDNSBROWSER_H
#define LIBSAKUSEN_CLIENT__MDNSBROWSER_H

#include <boost/utility.hpp>
#include <boost/function.hpp>
#include <boost/scoped_ptr.hpp>
#include <sakusen/libsakusen-global.h>
#include <sakusen/servedgame.h>

namespace sakusen {
namespace client {

/** \brief Browse game servers using Avahi and mDNS.
 *
 * MdnsBrowser will prod Avahi into querying for Sakusen servers and will
 * repeatedly call the callbacks when servers appear or go away, allowing you
 * to show the user a server list that will continually update itself.
 *
 * This class is not thread-safe, in that each instance must be accessed from
 * only a single thread (or protected by locks), but you can have as many
 * instances as you like, each accessed from a possibly different thread. Each
 * will get its own Avahi event loop thread, and the callbacks will be called
 * from this thread.
 *
 * Uses the pimpl idiom to avoid users having to know any details of Avahi.
 *
 * \todo Maybe it would be better to not have an Avahi event loop thread and
 * just crank its event loop from the client's event loop. Discuss.
 *
 * \todo Allow a way of discovering and selecting which browsing domain(s) to
 * use.
 */
class MdnsBrowser : public boost::noncopyable {
  public:
  MdnsBrowser(boost::function<void(std::auto_ptr<ServedGame>)> cb_appeared,
      boost::function<void(String)> cb_gone);
  ~MdnsBrowser();

  private:
  class Implementation;
  boost::scoped_ptr<Implementation> pimpl;
};

}
}

#endif /*LIBSAKUSEN_CLIENT__MDNSBROWSER_H*/
