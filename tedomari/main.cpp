#include "libsakusen-global.h"
#include "stringutils.h"
#include "libsakusen-comms-global.h"
#include "unixdatagramconnectingsocket.h"
#include "errorutils.h"
#include "fileutils.h"
#include "serverinterface.h"
#include "asynchronousiohandler.h"

/* TODO: This include will need to be guarded by whatever symbol is defined by
 * the --enable-sdl configure switch once such a thing exists */
#include "ui/sdl/sdlui.h"

#include <sys/stat.h>
#include <getopt.h>

#include <iostream>
#include <list>

#define NANO 1000000000

using namespace std;
using namespace __gnu_cxx;

using namespace tedomari;
using namespace tedomari::ui;
using namespace tedomari::ui::sdl;

/* struct to store options processed from the command line */

struct Options {
  Options() : noAbstract(false), historyLength(100), nonOptionIndex(0) {}
  ~Options() {}
  bool noAbstract;
  int historyLength;
  int nonOptionIndex;
};

/* enumeration of commands that can be entered at the tedomari prompt */

enum Command {
  command_join,
  command_leave,
  command_set,
  command_get,
  command_reconnect,
  command_quit,
  command_help
};

/* Forward declarations */

Options parseCommandLine(int argc, char * const * argv);

int main(int argc, char * const * argv)
{
  cout << "*******************************\n"
          "* tedomari (a Sakusen client) *\n"
          "*  Similus est circo mortis!  *\n"
          "*******************************" << endl;

  if (NULL == setlocale(LC_CTYPE, "")) {
    Fatal("error setting locale");
  }

  /* For the moment we simply attempt to connect to a socket where fuseki puts
   * it.
   * TODO: allow for connecting elsewhere, or not connecting at all at once */

  /* At present most of this code looks much like the fuseki main function, but
   * I think that will change in the long run so am not concerned overmuch with
   * the code duplication */
  
  Options options = parseCommandLine(argc, argv);
  
  /* Seek out the home directory */
  char* homePathPtr = getenv("HOME");

  if (homePathPtr == NULL) {
    Fatal("no home directory found");
  }

  String homePath(homePathPtr);
  
  /* Construct the path to the socket */
  String socketPath =
    homePath + CONFIG_SUBDIR SOCKET_SUBDIR FILE_SEP "fuseki-socket";

  /* Construct the path to the tedomari config directory */
  String configPath = homePath + CONFIG_SUBDIR FILE_SEP "tedomari";
  cout << "Using directory " << configPath << " for tedomari configuration\n";
  struct stat tmpStat;

  if (-1 == stat(configPath.c_str(), &tmpStat)) {
    switch (errno) {
      case ENOENT:
        cout << "Directory not found, trying to create it" << endl;
        if (-1 == fileUtils_mkdirRecursive(configPath, 0777)) {
          Fatal("error " << errorUtils_parseErrno(errno) <<
              " creating directory");
        }
        break;
      default:
        Fatal("could not stat directory");
    }
  }
  
  /* Construct the path to the history file */
  String historyPath = configPath + FILE_SEP "history";
  cout << "Using history file at " << historyPath << "\n";
  
  bool reconnect;

  do {
    reconnect = false;
    
    /* Connect to the socket */
    cout << "Trying to connect to socket at " << socketPath << endl;

    if (stat(socketPath.c_str(), &tmpStat)) {
      cout << "Socket not found.  Could not connect." << endl;
      exit(EXIT_FAILURE);
    }
    
    Socket* socket = new UnixDatagramConnectingSocket(socketPath, false);

    cout << "Connected to socket." << endl;
    
    ServerInterface serverInterface(socket, !options.noAbstract);

    cout << "Getting advertisement." << endl;
    
    AdvertiseMessageData advertisement;

    if (serverInterface.getAdvertisement(&advertisement)) {
      cout << "Failed to get advertisement.  Giving up." << endl;
      exit(1);
    }

    cout << "Advertisement:\n"
      "Server name: " << advertisement.getServerName() << "\n"
      "Game name:   " << advertisement.getGameName() << "\n"
      "\n"
      "Type 'help' for a list of commands" << endl;

    /* Create the asynchronous input handler.  Hereafter nothing else should
     * mess with stdin, because that is likely to cause problems. */
    AsynchronousIOHandler ioHandler(
        stdin, cout, historyPath, options.historyLength
      );
    UI* ui = NULL;

    bool finished = false;
    String whitespace = " \t\r\n"; /* TODO: obtain whitespace in some more
                                      general fashion.  Maybe use regexes and
                                      their associated cunningness */

    struct timespec sleepTime = {0, NANO/25};
    struct timespec commandSleepTime = {0, 0};

    /* Construct our commands */
    hash_map<String, Command, StringHash> commands;
    commands["j"] = commands["join"] = command_join;
    commands["l"] = commands["leave"] = command_leave;
    commands["g"] = commands["get"] = command_get;
    commands["s"] = commands["set"] = command_set;
    commands["q"] = commands["quit"] = command_quit;
    commands["r"] = commands["reconnect"] = command_reconnect;
    commands["h"] = commands["?"] = commands["help"] = command_help;
    
    String helpMessage =
      "Available commands:\n"
      "  join (j)              Connect to server\n"
      "  leave (l)             Leave server\n"
      "  get (g) SETTING       Get the value of SETTING from the server\n"
      "  set (s) SETTING VALUE Submit request to change SETTING to VALUE\n"
      "  reconnect (r)         Close this connection to the server and start "
        "over\n"
      "                        from scratch\n"
      "  quit (q)              Quit tedomari (closing any connection to "
        "server)\n"
      "  help (h, ?)           Display this help\n";
    
    while (!finished) {
      /* Process commands enterred through stdin */
      String command;
      String message;
      if (ioHandler.getCommand(command, &commandSleepTime)) {
        list<String> words = stringUtils_split(command, whitespace);
        if (!words.empty()) {
          String commandName = words.front();
          words.pop_front();
          if (commands.count(commandName)) {
            switch (commands[commandName]) {
              case command_quit:
                finished = true;
                break;
              case command_reconnect:
                finished = true;
                reconnect = true;
                break;
              case command_join:
                if (serverInterface.isJoined()) {
                  ioHandler.message("Already joined.\n");
                } else if ("" != (message = serverInterface.join())) {
                  ioHandler.message(message);
                } else {
                  ioHandler.message(
                      String("Joined.  Assigned client ID ") +
                        clientID_toString(serverInterface.getID()) + "\n");
                }
                break;
              case command_leave:
                if (!serverInterface.isJoined()) {
                  ioHandler.message("Not joined.\n");
                } else if (serverInterface.leave(true)) {
                  ioHandler.message("Error leaving server.\n");
                } else {
                  ioHandler.message("Left.\n");
                }
                break;
              case command_get:
                if (!serverInterface.isJoined()) {
                  ioHandler.message("Not joined.\n");
                } else if (words.size() != 1) {
                  ioHandler.message("Usage: get SETTING\n");
                } else {
                  String setting = words.front();
                  words.pop_front();
                  if (serverInterface.getSetting(setting)) {
                    ioHandler.message("Error setting setting\n");
                  }
                }
                break;
              case command_set:
                if (!serverInterface.isJoined()) {
                  ioHandler.message("Not joined.\n");
                } else if (words.size() != 2) {
                  ioHandler.message(
                      "Usage: set SETTING VALUE\n"
                      "(Values containing whitespace are not supported yet - "
                        "sorry!)\n"
                    );
                } else {
                  String setting = words.front();
                  words.pop_front();
                  String value = words.front();
                  words.pop_front();
                  if (serverInterface.setSetting(setting, value)) {
                    ioHandler.message("Error setting setting\n");
                  }
                }
                break;
              case command_help:
                ioHandler.message(helpMessage);
                break;
              default:
                Fatal("failed to handle command " << commands[commandName]);
            }
          } else {
            ioHandler.message(
                String("Unknown command '")+commandName+
                  "'.  Type 'help' for a list of commands.\n"
              );
          }
        }
      }
      /* If we've reached end of input then stop */
      if (ioHandler.getEof()) {
        finished = true;
      }
      /* Process messages from server */
      if ("" != (message = serverInterface.flushIncoming())) {
        ioHandler.message(message);
      }
      if (serverInterface.isGameStarted() && ui == NULL) {
        /* Hopefully this should be the only mention anywhere outside of the
         * tedomari::ui::sdl code of SDL.
         * TODO: support alternate UIs (OpenGL, DirectX) */
        ui = new SDLUI();
      }
      nanosleep(&sleepTime, NULL);
    }
    
    cout << endl;
    delete ui;
    delete socket;
  } while (reconnect);
  
  return EXIT_SUCCESS;
}

/* Function to parse the command line */

Options parseCommandLine(int argc, char * const * argv) {
  Options results = Options();
  
  struct option options[] = {
    /* name             args               flag  val (=short version) */
    { "no-abstract",    no_argument,       NULL, 'a' },
    { "history-length", required_argument, NULL, 'h' },
    { 0,                0,                 0,    0   }
  };
  
  bool done = false;
  
  do {
    int opt = getopt_long(argc, argv, "ah", options, NULL);

    switch(opt) {
      case -1:
        results.nonOptionIndex = optind;
        done = true;
        break;
      case '?':
        exit(EXIT_FAILURE);
        break;
      case 'a':
        results.noAbstract = true;
        break;
      case 'h':
        results.historyLength = numFromString<int>(optarg);
        break;
      default:
        Fatal("unexpected option");
    }
  } while (!done);

  return results;
}

