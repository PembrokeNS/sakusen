#include "libsakusen-global.h"
#include "tedomari-global.h"
#include "stringutils.h"
#include "partialworld.h"
#include "libsakusen-comms-global.h"
#include "timeutils.h"
#include "errorutils.h"
#include "unixdatagramconnectingsocket.h"
#include "socketexn.h"
#include "libsakusen-resources-global.h"
#include "fileutils.h"
#include "fileresourceinterface.h"

#include "asynchronousiohandler.h"
#include "revision.h"
#include "serverinterface.h"

#include "game/game.h"

#include "ui/ui.h"

#ifndef DISABLE_SDL
  #include "ui/sdl/sdlui.h"
  #ifndef DISABLE_CAIRO
    #include "ui/sdl/cairo/cairoui.h"
  #endif
#endif

#ifdef WIN32
  #include <locale.h>
#endif

#include <sys/stat.h>

#include <iostream>
#include <fstream>
#include <list>
#include <optionsparser.h>

using namespace std;
using namespace __gnu_cxx;
using namespace optimal;

using namespace sakusen;
using namespace sakusen::comms;
using namespace sakusen::resources;
using namespace sakusen::client;
using namespace tedomari;
using namespace tedomari::game;
using namespace tedomari::ui;

#ifndef DISABLE_SDL
  using namespace tedomari::ui::sdl;
  #ifndef DISABLE_CAIRO
    using namespace tedomari::ui::sdl::cairo;
  #endif
#endif

namespace tedomari {

/** struct to store options processed from the configuration
 * file and command line */
struct Options {
  /** Default constructor sets all options to their default values */
  Options() :
#ifndef DISABLE_UNIX_SOCKETS
    abstract(true),
    unixSockets(true),
#endif
    evil(false),
    historyLength(100),
    test(false),
    joinAddress(),
    autoJoin(true),
    help(false),
    version(false)
  {}
#ifndef DISABLE_UNIX_SOCKETS
  /** Whether to use the abstract unix socket namespace where possible */
  bool abstract;
  /** Whether to use unix sockets where possible */
  bool unixSockets;
#endif
  /** When set, tedomari will sleep less */
  bool evil;
  /** Number of commands to save between executions from the tedomari terminal
   * prompt */
  int historyLength;
  /** When set, tedomari does not attempt to connect to a server, but rather
   * runs the GUI in a test mode */
  bool test;
#ifndef DISABLE_SDL
  /** Has various suboptions related to the SDL GUI.  See
   * tedomari::ui:sdl::SDLUI::Options */
  SDLUI::Options sdlOptions;
#endif
  /** Address to use to join a server */
  String joinAddress;
  /** Whether to join automatically on startup */
  bool autoJoin;
  /** Commands to be executed automatically upon joining */
  list<String> autoCommands;
  /** When set, tedomari prints a help message and exits */
  bool help;
  /** When set, tedomari prints version information and exits */
  bool version;
};

} /* end namespace tedomari */

/* enumeration of commands that can be entered at the tedomari prompt */

enum Command {
  command_join,
  command_leave,
  command_set,
  command_get,
  command_clientSet,
  command_clientGet,
  command_reconnect,
  command_talk,
  command_resetUI,
  command_sleep,
  command_quit,
  command_help
};

/* Forward declarations */

void runTest(
    const Options& options,
    const String& homePath,
    const String& configPath
  );

void runClient(
    const Options& options,
    const String& homePath,
    const String& configPath
  );

Options getOptions(String optionsFile, int argc, char const* const* argv);

UI::Ptr newUI(const Options& o, const String& uiConfFilename, Game* game);

void usage();

int main(int argc, char const* const* argv)
{
  if (NULL == setlocale(LC_CTYPE, "")) {
    Fatal("error setting locale");
  }

  /* For the moment we simply attempt to connect to a socket where fuseki puts
   * it. */
  /** \todo Allow for connecting elsewhere, or not connecting at all at once */
  
  /* Seek out the home directory */
  String homePath = fileUtils_getHome();
  
  /* Construct the path to the tedomari config directory */
  String configPath = homePath + CONFIG_SUBDIR FILE_SEP "tedomari";
  cout << "Using directory " << configPath << " for tedomari configuration\n";
  struct stat tmpStat;

  if (-1 == stat(configPath.c_str(), &tmpStat)) {
    switch (errno) {
      case ENOENT:
        cout << "Directory not found, trying to create it" << endl;
        fileUtils_mkdirRecursive(configPath, 0777);
        break;
      default:
        Fatal("could not stat directory");
    }
  }
  
  Options options = getOptions(configPath + FILE_SEP "config", argc, argv);

  if (options.help) {
    usage();
    exit(EXIT_SUCCESS);
  }

  if (options.version) {
    cout << APPLICATION_NAME " " APPLICATION_VERSION;
    cout << " (revision " << REVISION << ")";
    cout << endl;
    exit(EXIT_SUCCESS);
  }
  
  cout << "*******************************\n"
          "* tedomari (a Sakusen client) *\n"
          "*  Similus est circo mortis!  *\n"
          "*******************************" << endl;

  /* Initialize sockets */
  Socket::socketsInit();

  if (options.test) {
    runTest(options, homePath, configPath);
  } else {
    runClient(options, homePath, configPath);
  }
  
  return EXIT_SUCCESS;
}

void runTest(
    const Options& options,
    const String& homePath,
    const String& configPath
  ) {
  String uiConfFilename = configPath + FILE_SEP "ui.conf";
  ResourceInterface::Ptr resourceInterface = FileResourceInterface::create(
      homePath + CONFIG_SUBDIR DATA_SUBDIR, false
    );
  Game* game = new Game(resourceInterface);
  UI::Ptr ui = newUI(options, uiConfFilename, game);

  struct timeval sleepTime = {0, MICRO/25};
  if (options.evil) {
    sleepTime.tv_usec = MICRO/250;
  }

  while (true) {
    ui->update();
    if (ui->isQuit()) {
      break;
    }
    timeUtils_sleep(sleepTime);
  }

  delete game;
  game = NULL;
}

void runClient(
    const Options& options,
    const String& homePath,
    const String& configPath
  ) {
  String uiConfFilename = configPath + FILE_SEP "ui.conf";
  String socketAddress;
  
#ifdef DISABLE_UNIX_SOCKETS
  socketAddress = options.joinAddress;
  if (socketAddress.empty()) {
    cout << "You must provide an address for joining with --join" << endl;
    exit(EXIT_FAILURE);
  }
#else
  /* Use default socket */
  socketAddress = "unix"ADDR_DELIM"concrete"ADDR_DELIM +
    homePath + CONFIG_SUBDIR SOCKET_SUBDIR FILE_SEP "fuseki-socket";
#endif

  /* Construct the path to the history file */
  String historyPath = configPath + FILE_SEP "history";
  cout << "Using history file at " << historyPath << "\n";
  
  bool reconnect;

  do {
    reconnect = false;
    
    /** \todo The ResourceInterface actually needs to be able to access
     * resources over the network from the server as well as from disk */
    vector<String> dataDirs;
    dataDirs.push_back(homePath + CONFIG_SUBDIR DATA_SUBDIR);
    dataDirs.push_back("data");
    dataDirs.push_back(".."FILE_SEP"data");
    dataDirs.push_back(".."FILE_SEP".."FILE_SEP"data");
    dataDirs.push_back(".."FILE_SEP".."FILE_SEP".."FILE_SEP"data");
    
    ResourceInterface::Ptr resourceInterface =
        FileResourceInterface::create(dataDirs, false);
    Game* game = new Game(resourceInterface);
    ServerInterface serverInterface(socketAddress,
#ifndef DISABLE_UNIX_SOCKETS
        options.unixSockets, options.abstract,
#endif
        game
      );

    std::cout << "Type 'help' for a list of commands" << endl;

    /* Create the asynchronous input handler.  Hereafter nothing else should
     * mess with stdin, because that is likely to cause problems. */
    AsynchronousIOHandler ioHandler(
        stdin, cout, historyPath, options.historyLength
      );
    UI::Ptr ui;

    bool finished = false;
    String whitespace = " \t\r\n"; /** \todo obtain whitespace in some more
                                      general fashion.  Maybe use regexes and
                                      their associated cunningness */

    struct timeval sleepTime = {0, MICRO/25};
    if (options.evil) {
      sleepTime.tv_usec = MICRO/250;
    }
    struct timeval commandSleepTime = {0, 0};

    /* Construct our commands */
    hash_map_string< ::Command>::type commands;
    commands["j"] = commands["join"] = command_join;
    commands["l"] = commands["leave"] = command_leave;
    commands["g"] = commands["get"] = command_get;
    commands["s"] = commands["set"] = command_set;
    commands["cg"] = commands["clientget"] = command_clientGet;
    commands["cs"] = commands["clientset"] = command_clientSet;
    commands["q"] = commands["quit"] = command_quit;
    commands["r"] = commands["reconnect"] = command_reconnect;
    commands["t"] = commands["talk"] = command_talk;
    commands["u"] = commands["resetui"] = command_resetUI;
    commands["z"] = commands["sleep"] = command_sleep;
    commands["h"] = commands["?"] = commands["help"] = command_help;
    
    String helpMessage =
      "Available commands:\n"
      "  join (j)                Connect to server\n"
      "  leave (l)               Leave server\n"
      "  get (g) SETTING         Get the value of SETTING from the server\n"
      "  set (s) SETTING VALUE   Submit request to change SETTING to VALUE\n"
      "  clientget (cg) SETTING  Get the value of SETTING, which is a path\n"
      "                          inside this client's settings\n"
      "  clientset (cs) SETTING VALUE  Submit request to change SETTING to VALUE,\n"
      "                          where SETTING is a path inside this client's\n"
      "                          settings\n"
      "  reconnect (r)           Close this connection to the server and start over\n"
      "  talk (t) MESSAGE        Send MESSAGE to other clients\n"
      "  resetui (u)             If there is a game UI open, then close it and reopen it\n"
      "                          from scratch\n"
      "  sleep (z) USEC          Sleep tedomari for USEC microseconds\n"
      "  quit (q)                Quit tedomari (closing any connection to server)\n"
      "  help (h, ?)             Display this help\n";

    /* If asked to autoJoin, then cause it to happen */
    ioHandler.fakeCommand("join");
    
    while (!finished) {
      /* Process commands enterred through stdin */
      String command;
      String message;
      if (ioHandler.getCommand(command, commandSleepTime)) {
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
                  ioHandler.message(message+"\n");
                } else {
                  ioHandler.message(
                      String("Joined.  Assigned client id ") +
                        serverInterface.getId().toString() + "\n");
                  /* Queue all the autocommands that the user asked for */
                  for (list<String>::const_iterator autoCommand =
                      options.autoCommands.begin();
                      autoCommand != options.autoCommands.end();
                      ++autoCommand) {
                    ioHandler.fakeCommand(*autoCommand);
                  }
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
              case command_clientGet:
                if (!serverInterface.isJoined()) {
                  ioHandler.message("Not joined.\n");
                } else if (words.size() != 1) {
                  ioHandler.message("Usage: clientget SETTING\n");
                } else {
                  String setting = words.front();
                  words.pop_front();
                  if (serverInterface.getClientSetting(setting)) {
                    ioHandler.message("Error setting setting\n");
                  }
                }
                break;
              case command_clientSet:
                if (!serverInterface.isJoined()) {
                  ioHandler.message("Not joined.\n");
                } else if (words.size() != 2) {
                  ioHandler.message(
                      "Usage: clientset SETTING VALUE\n"
                      "(Values containing whitespace are not supported yet - "
                        "sorry!)\n"
                    );
                } else {
                  String setting = words.front();
                  words.pop_front();
                  String value = words.front();
                  words.pop_front();
                  if (serverInterface.setClientSetting(setting, value)) {
                    ioHandler.message("Error setting setting\n");
                  }
                }
                break;
              case command_talk:
                if (!serverInterface.isJoined()) {
                  ioHandler.message("Not joined.\n");
                } else {
                  // Locate the start of the actual message
                  String::size_type p = command.find_first_not_of(whitespace);
                  p = command.find_first_of(whitespace, p);
                  p = command.find_first_not_of(whitespace, p);
                  if (p == String::npos) {
                    ioHandler.message(
                        "Usage: talk MESSAGE\n"
                      );
                  } else {
                    String message = command.substr(p);
                    OArchive data;
                    data << message;
                    serverInterface.send(
                        new ExtensionMessageData("talk", 0, data)
                      );
                  }
                }
                break;
              case command_resetUI:
                if (ui != NULL) {
                  ui = newUI(options, uiConfFilename, game);
                }
                break;
              case command_sleep:
                if((words.size()==2) &&(words.front()==String("data"))&&(words.back()==String("sleep")))
                {
                  ioHandler.message("He's exhausted!\n");
                } else if (words.size() != 1) {
                  ioHandler.message("Usage: sleep USEC\n");
                } else if (words.front()==String("data")){
                  ioHandler.message("I don't understand...\n");
                } else {
                  uint32 usec = numFromString<uint32>(words.front());
                  words.pop_front();
                  ioHandler.message("sleeping...\n");
                  timeUtils_sleep(usec);
                  ioHandler.message("awoken\n");
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
      /* Open or close UI appropriately */
      if (game->isStarted() && ui == NULL) {
        ui = newUI(options, uiConfFilename, game);
      }
      if (!game->isStarted() && ui != NULL) {
        ui.reset();
      }
      /* Allow the UI some processor time */
      if (ui != NULL) {
        ui->update();
        if (ui->isQuit()) {
          finished = true;
        }
      }
      timeUtils_sleep(sleepTime);
    }
    
    cout << endl;
    delete game;
    game = NULL;
  } while (reconnect);
}

void usage() {
  cout << "tedomari\n"
          "\n"
          "Usage: tedomari [OPTIONS]\n"
          "\n"
#ifndef DISABLE_UNIX_SOCKETS
          " -a-, --no-abstract,     do not use the abstract unix socket namespace\n"
          " -u-, --no-unix,         do not use any unix sockets\n"
#endif
          " -e,  --evil,            try for a higher framerate\n"
          " -l,  --history-length LENGTH, store LENGTH commands in the command history\n"
          "                         upon exiting\n"
          " -t,  --test,            don't try to connect to a server, just test the UI\n"
#ifndef DISABLE_SDL
          "      --sdlopts OPTIONS, pass OPTIONS to the SDL UI\n"
#endif
          " -j,  --join ADDRESS,    join server at sakusen-style address ADDRESS\n"
          " -o-, --no-autojoin,     do not automatically try to join server\n"
          " -c,  --commands COMMAND;..., execute each COMMAND upon joining\n"
          " -h,  --help,            display help and exit\n"
          " -V,  --version,         display version information and exit\n"
          "" << endl;
}

/* Function to parse the command line */
Options getOptions(String optionsFile, int argc, char const* const* argv) {
  Options results = Options();
  OptionsParser parser;
#ifndef DISABLE_SDL
  OptionsParser sdlOptionsParser = SDLUI::getParser(&results.sdlOptions);
#endif

#ifndef DISABLE_UNIX_SOCKETS
  parser.addOption("abstract",       'a',  &results.abstract);
  parser.addOption("unix",           'u',  &results.unixSockets);
#endif
  parser.addOption("evil",           'e',  &results.evil);
  parser.addOption("history-length", 'l',  &results.historyLength);
  parser.addOption("test",           't',  &results.test);
#ifndef DISABLE_SDL
  parser.addOption("sdlopts",        '\0', &sdlOptionsParser);
#endif
  parser.addOption("join",           'j',  &results.joinAddress);
  parser.addOption("autojoin",       'o',  &results.autoJoin);
  parser.addOption("commands",       'c',  &results.autoCommands, ';');
  parser.addOption("help",           'h',  &results.help);
  parser.addOption("version",        'V',  &results.version);

  if (parser.parse(optionsFile, argc, argv)) {
    /* There was a problem */
    cout << "error(s) processing options:\n" <<
      stringUtils_join(parser.getErrors(), "\n");
    usage();
    exit(EXIT_FAILURE);
  }

  /*Debug("options.unixSockets=" << results.unixSockets);*/

  return results;
}

UI::Ptr newUI(const Options& o, const String& uiConfFilename, Game* game)
{
  /** \todo Support alternate UIs (OpenGL, DirectX) */
  ifstream uiConf(uiConfFilename.c_str());
#ifndef DISABLE_CAIRO
  UI::Ptr ui(new CairoUI(o.sdlOptions, uiConf, game));
#else
  Fatal("No UI enabled at compile time");
  UI::Ptr ui;
#endif
  ui->setTitle("tedomari");
  return ui;
}

