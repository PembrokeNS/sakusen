#ifndef UI__ALERT_H
#define UI__ALERT_H

namespace tedomari {
namespace ui {

class Alert {
  public:
    Alert() : message() {}
    Alert(const String& m) : message(m) {}
  private:
    String message;
  public:
    const String& getMessage() const { return message; }
};

}}

#endif // UI__ALERT_H

