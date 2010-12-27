#ifndef TEDOMARI__ASYNCHRONOUSIOHANDLER_H
#define TEDOMARI__ASYNCHRONOUSIOHANDLER_H

#include "global.h"

#include <cstdio>
#include <iosfwd>
#include <queue>
#include <boost/filesystem/path.hpp>

#include "converter.h"
#include <sakusen/comms/timeutils.h>

namespace tedomari {

void line_callback_handler(char* line);

/** \brief This class does non-blocking input (not really output)
 *
 * By default, GNU Readline is used to handle the input, which allows for
 * command history, etc.  However, if the preprocessor symbol DISABLE_READLINE
 * is set then a simpler fallback method is used. */
class AsynchronousIOHandler : private boost::noncopyable {
  friend void line_callback_handler(char* line);
  public:
    /** \brief Constructor
     *
     * \param in C-style file stream to use for input
     * \param out C++-style stream to use for output
     * \param historyFile Path to history file (should be accessible for both
     * reading and writing)
     * \param historyLength Maximum number of commands to save from the command
     * history into the history file upon destruction
     *
     * We are mixing C- and C++-style streams here because readline is a C
     * library, and thus cannot handle cin, whereas we can use C++ streams for
     * output, and thus do, because it is more convinient.
     *
     * \note When readline is disabled, historyFile and historyLength are
     * ignored because command history is not supported. */
    AsynchronousIOHandler(
        FILE* in,
        std::ostream& out,
        const boost::filesystem::path& historyFile,
        int historyLength
      );
    /** \brief Destructor
     *
     * Amongst other things, this destructor is responsible for saving some or
     * all of the command history to the history file (as specified to the
     * constructor */
    ~AsynchronousIOHandler();
  private:
    int infd;
    std::ostream& out;
    boost::filesystem::path historyFile;
    int historyLength;
    std::queue<String> commandBuffer;
    String lastLine;
    bool eof;
    Converter converter;
#ifdef DISABLE_READLINE
    /** Chars which have been input but not terminated with a newline */
    String inputBuffer;
    #ifdef _MSC_VER
        HANDLE hStdout, hStdin;
        DWORD cRead, cWritten, fdwMode, fdwOldMode; 
    #endif //_MSC_VER
#endif //DISABLE_READLINE
    
    /** \brief Put all pending commands into commandBuffer
     *
     * This method reads as many chars as are available on the input and puts
     * any complete commands into the buffer */
    void updateBuffer(const struct timeval& timeout);
  public:
    /** \brief Returns true iff input has reached EOF */
    inline bool getEof() const { return eof; }
    /** \brief Get the next pending command
     * 
     * If a command is ready, put it into \a command and return true, else
     * return false.  Wait until a period of length \a timeout occurs *between*
     * characters being input. */
    bool getCommand(String& command, const struct timeval& timeout);
    /** \brief Output a message
     *
     * Display given message, and follow it with the prompt and any partial
     * command which has been input by the user */
    void message(const String& message);
    /** \brief Append a command to the command queue
     *
     * The command passed to this method will by added to the internal command
     * queue, so that it will later be returned through getCommand, as if the
     * user had typed it. */
    void fakeCommand(const String& command) {
      commandBuffer.push(command);
    }
};

}

#endif // TEDOMARI__ASYNCHRONOUSIOHANDLER_H

