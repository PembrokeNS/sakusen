#ifndef TEDOMARI__UI__ALERT_H
#define TEDOMARI__UI__ALERT_H

namespace tedomari {
namespace ui {

/** \brief A message displayed to the user in the GUI
 *
 * A single Alert is a message of some sort which is expected to be displayed
 * to the user through the GUI (in particular, through an AlertDisplay).  For
 * example, a message "Factory completed".
 *
 * \todo Alerts need many more associated properties, such as a location,
 * player, client, unit, and/or sensor return to which they pertain. */
class Alert {
  public:
    /** brief Default constructor
     *
     * Creates an Alert with an empty message */
    Alert() : message() {}
    /** \brief Constructor for string Alert
     *
     * \param m String message for Alert */
    explicit Alert(const String& m) : message(m) {}
  private:
    String message;
  public:
    /** \name Accessors */
    //@{
    const String& getMessage() const { return message; }
    //@}
};

}}

#endif // TEDOMARI__UI__ALERT_H

