#include "asynchronousiohandler.h"
#include "errorutils.h"

#include <readline/readline.h>
#include <readline/history.h>

#include <fcntl.h>
#include <errno.h>

#include <ostream>

using namespace std;

using namespace tedomari;

/* Static data for the static function below to use */

AsynchronousIOHandler* handler = NULL;

/* Static function because it needs to be used as a C-style function pointer */
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

void AsynchronousIOHandler::updateBuffer(const struct timespec* timeout)
{
  fd_set inSet;
  FD_ZERO(&inSet);
  FD_SET(infd, &inSet);
  
  while(true) {
    switch(pselect(infd+1, &inSet, NULL, NULL, timeout, NULL)) {
      case -1:
        Fatal("select failed , errno=" << errno << " (" <<
          errorUtils_parseErrno(errno) <<")");
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

bool AsynchronousIOHandler::getCommand(
    String& command,
    const struct timespec* timeout)
{
  updateBuffer(timeout);
  if (!commandBuffer.empty()) {
    command = converter.convertNativeToUTF8(commandBuffer.front());
    commandBuffer.pop();
    return true;
  }

  return false;
}

void AsynchronousIOHandler::message(const String& message)
{
  struct timespec timeout = {0, 0};
  updateBuffer(&timeout);
  out << "\n" << converter.convertUTF8ToNative(message);
  rl_on_new_line();
  rl_redisplay();
}

