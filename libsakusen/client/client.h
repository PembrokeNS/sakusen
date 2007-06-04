#ifndef LIBSAKUSEN_CLIENT__CLIENT_H
#define LIBSAKUSEN_CLIENT__CLIENT_H

namespace sakusen {
namespace client {

/** \brief Represents a client from the perspective of another client
 *
 * This class serves no purpose at time of writing.
 */
class Client {
  protected:
    Client();
    Client(const Client&);
  public:
    virtual ~Client();
    
    virtual Client* newCopy() = 0;
};

}}

#endif // LIBSAKUSEN_CLIENT__CLIENT_H

