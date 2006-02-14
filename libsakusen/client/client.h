#ifndef CLIENT_H
#define CLIENT_H

namespace sakusen {
namespace client {

class Client {
  protected:
    Client();
    Client(const Client&);
  public:
    virtual ~Client();
    
    virtual Client* newCopy() = 0;
};

}}

#endif // CLIENT_H

