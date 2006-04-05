#include "asynchronousiohandler.h"
#include "errorutils.h"

#ifndef DISABLE_READLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif

#include <fcntl.h>

#include <cerrno>
#include <ostream>

using namespace std;

using namespace sakusen::comms;
using namespace tedomari;

#ifdef DISABLE_READLINE

#define INPUT_BUFFER_LEN 512

#ifdef WIN32
#define NativeRead _read
#define NativeReadReturnType int
#include <io.h>
#else
#define NativeRead read
#define NativeReadReturnType ssize_t
#endif

AsynchronousIOHandler::AsynchronousIOHandler(
    FILE* in,
    ostream& o,
    String hf,
    int hl
  ) :
  infd(fileno(in)),
  out(o),
  historyFile(hf),
  historyLength(hl),
  commandBuffer(),
  eof(false),
  inputBuffer()
{
}

AsynchronousIOHandler::~AsynchronousIOHandler()
{
}

void AsynchronousIOHandler::updateBuffer(const struct ::timeval& timeout)
{
  fd_set inSet;
  FD_ZERO(&inSet);
  FD_SET(infd, &inSet);
  
  while(true) {
    switch(select(infd+1, &inSet, NULL, NULL, &timeout)) {
      case -1:
        Fatal("select failed: " << errorUtils_parseErrno(socket_errno));
      case 0:
        /* Nothing further */
        return;
      case 1:
        /* There is something to read */
        {
          char buf[INPUT_BUFFER_LEN];
          /* Read what we can from the input file descriptor */
          NativeReadReturnType bytesRead = NativeRead(infd, buf, INPUT_BUFFER_LEN);
          if (bytesRead < 0)
            Fatal("error reading input: " << errorUtils_errorMessage(errno));
          /* Append what we've read to the input buffer */
          inputBuffer.append(buf, bytesRead);
          /* Strip newline-delimited commands from the input buffer */
          String::iterator nl = find(inputBuffer.begin(), inputBuffer.end(), '\n');
          while (nl != inputBuffer.end()) {
            commandBuffer.push(String(inputBuffer.begin(), nl));
            inputBuffer.erase(inputBuffer.begin(), nl);
            nl = find(inputBuffer.begin(), inputBuffer.end(), '\n');
          }
        }
        break;
      default:
        Fatal("Unexpected return value from select.");
    }
  }
}

void AsynchronousIOHandler::message(const String& message)
{
  struct timeval timeout = {0, 0};
  updateBuffer(timeout);
  out << "\n" << converter.convertUTF8ToNative(message) <<
    "> " << inputBuffer;
}

#else // DISABLE_READLINE

/** \breif Static data for tedomari::line_callback_handler to use
 * 
 * \internal */
AsynchronousIOHandler* handler = NULL;

/** \brief Static function because it needs to be used as a C-style function
 * pointer
 *
 * \internal */
void tedomari::line_callback_handler(char* line)
{
  /* The readline library has passed us this completed line of input - we pass
   * it on to the IO handler */
  assert(handler != NULL);
  if (line == NULL) {
    handler->eof = true;
  } else {
    if (*line != '\0') {
      if (handler->lastLine != line) {
        add_history(line);
        handler->lastLine = line;
      }
    }
    handler->commandBuffer.push(line);
    free(line);
  }
}  

AsynchronousIOHandler::AsynchronousIOHandler(
    FILE* in,
    ostream& o,
    String hf,
    int hl
  ) :
  infd(fileno(in)),
  out(o),
  historyFile(hf),
  historyLength(hl),
  commandBuffer(),
  eof(false)
{
  if (handler != NULL) {
    Fatal("cannot install multiple AsynchronousIOHandlers");
  }
  handler = this;
  rl_callback_handler_install("> ", line_callback_handler);
  struct stat s;
  if (0 == stat(historyFile.c_str(), &s)) {
    if (S_ISREG(s.st_mode)) {
      if (0 == read_history(historyFile.c_str())) {
        if (0 == history_set_pos(history_length)) {
          Debug("error setting history position");
        }
        /* TODO: set the value of lastLine appropriately */
      } else {
        Debug("error reading history");
      }
    }
  }
}

AsynchronousIOHandler::~AsynchronousIOHandler()
{
  if (0 == write_history(historyFile.c_str())) {
    if (0 != history_truncate_file(historyFile.c_str(), historyLength)) {
      Debug("error truncating history");
    }
  } else {
    Debug("error writing history");
  }
  rl_callback_handler_remove();
  handler = NULL;
}

void AsynchronousIOHandler::updateBuffer(const struct timeval& timeout2)
{
  fd_set inSet;
  FD_ZERO(&inSet);
  FD_SET(infd, &inSet);
  struct timeval timeout=timeout2;
  while(true) {
    switch(select(infd+1, &inSet, NULL, NULL, &timeout)) {
      case -1:
        Fatal("select failed , errno=" << errno << " (" <<
          errorUtils_parseErrno(errno) << ")");
      case 0:
        /* Nothing further */
        return;
      case 1:
        /* There is something to read */
        rl_callback_read_char();
        break;
      default:
        Fatal("Unexpected return value from select.");
    }
  }
}

void AsynchronousIOHandler::message(const String& message)
{
  struct timeval timeout = {0, 0};
  updateBuffer(timeout);
  out << "\n" << converter.convertUTF8ToNative(message);
  rl_on_new_line();
  rl_redisplay();
}

#endif // DISABLE_READLINE

bool AsynchronousIOHandler::getCommand(
    String& command,
    const struct timeval& timeout)
{
  updateBuffer(timeout);
  if (!commandBuffer.empty()) {
    command = converter.convertNativeToUTF8(commandBuffer.front());
    commandBuffer.pop();
    return true;
  }

  return false;
}
