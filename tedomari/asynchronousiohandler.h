#ifndef ASYNCHRONOUSIOHANDLER_H
#define ASYNCHRONOUSIOHANDLER_H

#include <cstdio>
#include <iosfwd>
#include <queue>

#include "libsakusen-global.h"
#include "converter.h"

namespace tedomari {

/* This class does asyncronous input (not really output) */

void line_callback_handler(char* line);

class AsynchronousIOHandler {
  friend void line_callback_handler(char* line);
  private:
    AsynchronousIOHandler();
    AsynchronousIOHandler(const AsynchronousIOHandler& copy);
  public:
    AsynchronousIOHandler(
        FILE* in,
        std::ostream& out,
        String historyFile,
        int historyLength
      );
      /* We are indeed using a horrific mixture of types here.  It gets worse
       * in the implementation. */
    ~AsynchronousIOHandler();
  private:
    int infd;
    std::ostream& out;
    String historyFile;
    int historyLength;
    std::queue<String> commandBuffer;
    String lastLine;
    bool eof;
    void updateBuffer(const struct timespec* timeout);
      /* read as many chars as are available */
    Converter converter;
  public:
    inline bool getEof() const { return eof; }
    void prompt(); /* Display prompt */
    bool getCommand(String& command, const struct timespec* timeout);
      /* If a command is ready, put it into the string and return true, else
       * return false.  Wait until a period of length timeout occurs *between*
       * characters being input. */
    void message(const String& message);
      /* Display given message, and follow it with the prompt and any partial
       * mesage which has been input so far */
};

}

#endif // ASYNCHRONOUSIOHANDLER_H

