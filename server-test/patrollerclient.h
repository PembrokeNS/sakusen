#ifndef PATROLLERCLIENT_H
#define PATROLLERCLIENT_H

#include <boost/scoped_ptr.hpp>

#include "client.h"

namespace sakusen {
namespace server {
namespace test {

/* A toy client whose only purpose is to tell unit zero to patrol and shoot at
 * the first thing it sees */

class PatrollerClient : public Client {
  public:
    PatrollerClient(ClientId, Point<sint32> patrolTo);
    ~PatrollerClient() {};
  private:
    Point<sint32> patrolTo;
    Point<sint32> patrolFrom;
    bool headedOutward;
    boost::scoped_ptr<SensorReturns> otherUnit;
  public:
    void flushOutgoing(Time) {}
    void queueUpdate(const Update& update);
};

}}}

#endif // PATROLLERCLIENT_H

