#ifndef LIBSAKUSEN_SERVER__PLAYERDATA_H
#define LIBSAKUSEN_SERVER__PLAYERDATA_H

namespace sakusen {
namespace server {

class PlayerData : private boost::noncopyable {
  public:
    typedef boost::shared_ptr<PlayerData> Ptr;
  protected:
    PlayerData() {}
  public:
    virtual inline ~PlayerData() = 0;
};

inline PlayerData::~PlayerData() {}

}}

#endif // LIBSAKUSEN_SERVER__PLAYERDATA_H

