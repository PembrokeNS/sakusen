#ifndef PATROLLERCLIENT_H
#define PATROLLERCLIENT_H

#include "client.h"

namespace sakusen {
namespace server {
namespace test {

/* A toy client whose only purpose is to tell unit zero to patrol */

class PatrollerClient : public Client {
  private:
    PatrollerClient();
  public:
    PatrollerClient(Point<sint32> patrolTo);
    ~PatrollerClient() {};
  private:
    Point<sint32> patrolTo;
    Point<sint32> patrolFrom;
    bool headedOutward;
  public:
    void sendUpdate(const Update& update);
};

}}}

#endif // PATROLLERCLIENT_H

