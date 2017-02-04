#include <iostream>
#include <fstream>
#include <list>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>

#ifndef DISABLE_SDL
  #include "ui/sdl/sdlui.h"
  #ifndef DISABLE_CAIRO
    #include "ui/sdl/cairo/cairoui.h"
  #endif
#endif

#ifdef WIN32
  #include <locale.h>
#endif

#include <optimal/optionsparser.h>

#include "global.h"
#include <sakusen/revision.h>
#include <sakusen/stringutils.h>
#include <sakusen/fileutils.h>
#include <sakusen/client/partialworld.h>
#include <sakusen/comms/timeutils.h>
#include <sakusen/comms/errorutils.h>
#include <sakusen/comms/unixdatagramconnectingsocket.h>
#include <sakusen/comms/socketexn.h>
#include <sakusen/resources/fileresourceinterface.h>
#include <sakusen/heightfield-methods.h>

#include "asynchronousiohandler.h"
#include "serverinterface.h"

#include "game/game.h"

#include "ui/ui.h"

using namespace std;
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
    unixSockets(boost::logic::indeterminate),
#endif
    evil(false),
    historyLength(100),
    test(false),
    joinAddress(),
    autoJoin(true),
    uiConfig(),
    help(false),
    version(false)
  {}
#ifndef DISABLE_UNIX_SOCKETS
  /** Whether to use the abstract unix socket namespace where possible */
  bool abstract;
  /** Whether to use unix sockets where possible */
  boost::logic::tribool unixSockets;
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
  /** File to read for UI config */
  String uiConfig;
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
    const boost::filesystem::path& configPath
  );

void runClient(
    const Options& options,
    const boost::filesystem::path& configPath
  );

Options getOptions(
    const boost::filesystem::path& optionsFile,
    int argc,
    char const* const* argv
  );

UI::Ptr newUI(
    const Options& o,
    const boost::filesystem::path& uiConfFilename,
    Game* game
  );

void usage();
int main(int argc, char* argv[])
{
  if (NULL == setlocale(LC_CTYPE, "")) {
    SAKUSEN_FATAL("error setting locale");
  }

  /* Construct the path to the tedomari config directory */
  boost::filesystem::path configPath =
    fileUtils_configDirectory() / "tedomari";
  cout << "Using directory " << configPath.string() <<
    " for tedomari configuration\n";

  if (!boost::filesystem::exists(configPath)) {
    cout << "Directory not found, trying to create it" << endl;
    fileUtils_mkdirRecursive(configPath);
  }

  Options options = getOptions(configPath / "config", argc, argv);

  if (options.help) {
    usage();
    exit(EXIT_SUCCESS);
  }

  if (options.version) {
    cout << TEDOMARI_APPLICATION_NAME " " TEDOMARI_APPLICATION_VERSION;
    cout << " (revision " << REVISION << ")";
    cout << endl;
    exit(EXIT_SUCCESS);
  }

#ifndef DISABLE_SDL
  if (options.sdlOptions.help) {
    SDLUI::Options::usage(cout);
    exit(EXIT_SUCCESS);
  }
#endif

  cout << "*******************************\n"
          "* tedomari (a Sakusen client) *\n"
          "*  Similus est circo mortis!  *\n"
          "*******************************" << endl;

  /* Initialize sockets */
  Socket::socketsInit();

  if (options.test) {
    runTest(options, configPath);
  } else {
    runClient(options, configPath);
  }

  return EXIT_SUCCESS;
}

void runTest(
    const Options& options,
    const boost::filesystem::path& configPath
  ) {
  boost::filesystem::path uiConfFilePath;
  if (options.uiConfig.empty()) {
    uiConfFilePath = configPath / "ui.conf";
  } else {
    uiConfFilePath = options.uiConfig;
  }
  ResourceInterface::Ptr resourceInterface = FileResourceInterface::create(
      fileUtils_configDirectory() / SAKUSEN_RESOURCES_SUBDIR, false
    );
  Game* game = new Game(resourceInterface);
  UI::Ptr ui = newUI(options, uiConfFilePath, game);

  struct timeval sleepTime = {0, SAKUSEN_COMMS_TIMEUTILS_MICRO/25};
  if (options.evil) {
    sleepTime.tv_usec = SAKUSEN_COMMS_TIMEUTILS_MICRO/250;
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
    const boost::filesystem::path& configPath
  ) {
  boost::filesystem::path uiConfFilePath;
  if (options.uiConfig.empty()) {
    uiConfFilePath = configPath / "ui.conf";
  } else {
    uiConfFilePath = options.uiConfig;
  }

  /* For the moment we simply attempt to connect to a socket where fuseki puts
   * it. */
  /** \todo Allow for connecting elsewhere, or not connecting at all at once */
  String socketAddress;

  socketAddress = options.joinAddress;
  if (socketAddress.empty()) {
#ifdef DISABLE_UNIX_SOCKETS
    socketAddress = "tcp"SAKUSEN_COMMS_ADDR_DELIM"localhost";
#else
    /* Use default socket */
    socketAddress =
      "unix" SAKUSEN_COMMS_ADDR_DELIM "concrete" SAKUSEN_COMMS_ADDR_DELIM +
      (fileUtils_configDirectory() / SAKUSEN_COMMS_SOCKET_SUBDIR /
        "fuseki-socket").string();
#endif
  }

#ifndef DISABLE_UNIX_SOCKETS
  boost::logic::tribool unixSockets = options.unixSockets;
  /* If the unix sockets option was not specified then default to true iff
   * a unix join address was specified */
  if (boost::logic::indeterminate(unixSockets)) {
    unixSockets = boost::algorithm::starts_with(socketAddress, "unix");
  }
#endif

  /* Construct the path to the history file */
  boost::filesystem::path historyPath = configPath / "history";
  cout << "Using history file at " << historyPath.string() << "\n";

  bool reconnect;

  do {
    reconnect = false;

    /** \todo The ResourceInterface actually needs to be able to access
     * resources over the network from the server as well as from disk */
    boost::filesystem::path dotDot("..");
    vector<boost::filesystem::path> dataDirs;
    dataDirs.push_back(
        fileUtils_configDirectory() / SAKUSEN_RESOURCES_SUBDIR
      );
    dataDirs.push_back("data");
    dataDirs.push_back(dotDot/"data");
    dataDirs.push_back(dotDot/".."/"data");
    dataDirs.push_back(dotDot/".."/".."/"data");
    dataDirs.push_back(dotDot/".."/".."/".."/"data");

    ResourceInterface::Ptr resourceInterface =
        FileResourceInterface::create(dataDirs, false);
    Game* game = new Game(resourceInterface);
    ServerInterface serverInterface(
        socketAddress,
#ifndef DISABLE_UNIX_SOCKETS
        unixSockets, options.abstract,
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

    struct timeval sleepTime = {0, SAKUSEN_COMMS_TIMEUTILS_MICRO/25};
    if (options.evil) {
      sleepTime.tv_usec = SAKUSEN_COMMS_TIMEUTILS_MICRO/250;
    }
    struct timeval commandSleepTime = {0, 0};

    /* Construct our commands */
    u_map<String, ::Command>::type commands;
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
        list<String> words =
          stringUtils_split<list<String> >(command, whitespace);
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
                    String talkMessage = command.substr(p);
                    OArchive data;
                    data << talkMessage;
                    serverInterface.send(
                        new ExtensionMessageData("talk", 0, data.getBuffer())
                      );
                  }
                }
                break;
              case command_resetUI:
                if (ui != NULL) {
                  ui = newUI(options, uiConfFilePath, game);
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
                SAKUSEN_FATAL("failed to handle command " << commands[commandName]);
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
      if ("" != (message = serverInterface.flushIncoming(resourceInterface))) {
        ioHandler.message(message);
      }
      /* Open or close UI appropriately */
      if (game->isStarted() && ui == NULL) {
        ui = newUI(options, uiConfFilePath, game);
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
    /* Must leave before deleting game object */
    if (serverInterface.isJoined()) {
      serverInterface.leave(true);
    }
    delete game;
    game = NULL;
  } while (reconnect);
}

void usage() {
  cout <<
"tedomari\n"
"\n"
"Usage: tedomari [OPTIONS]\n"
"\n"
#ifndef DISABLE_UNIX_SOCKETS
" -a-, --no-abstract     Do not use the abstract unix socket namespace.\n"
" -x-, --no-unix         Do not use any unix sockets.\n"
#endif
" -e,  --evil            Try for a higher framerate.\n"
" -l,  --history-length LENGTH\n"
"                        Store LENGTH commands in the command history upon\n"
"                        exiting.\n"
" -t,  --test            Don't try to connect to a server, just test the UI.\n"
#ifndef DISABLE_SDL
"      --sdlopts OPTIONS Pass OPTIONS to the SDL UI.\n"
"                        Use --sdlopts help for more info.\n"
#endif
" -j,  --join ADDRESS    Join server at sakusen-style address ADDRESS.\n"
" -o-, --no-autojoin     Do not automatically try to join server.\n"
" -u,  --ui-config FILE  Use FILE for UI configuration, instead of\n"
"                        ~/.sakusen/tedomari/ui.conf\n"
" -c,  --commands COMMAND;...\n"
"                        Execute each COMMAND upon joining.\n"
" -h,  --help            Display help and exit.\n"
" -V,  --version         Display version information and exit.\n"
<< endl;
}

/* Function to parse the command line */
Options getOptions(
    const boost::filesystem::path& optionsFile,
    int argc,
    char const* const* argv
  ) {
  Options results = Options();
  OptionsParser parser;
#ifndef DISABLE_SDL
  OptionsParser sdlOptionsParser = SDLUI::getParser(&results.sdlOptions);
#endif

#ifndef DISABLE_UNIX_SOCKETS
  parser.addOption("abstract",       'a',  &results.abstract);
  parser.addOption("unix",           'x',  &results.unixSockets);
#endif
  parser.addOption("evil",           'e',  &results.evil);
  parser.addOption("history-length", 'l',  &results.historyLength);
  parser.addOption("test",           't',  &results.test);
#ifndef DISABLE_SDL
  parser.addOption("sdlopts",        '\0', &sdlOptionsParser);
#endif
  parser.addOption("join",           'j',  &results.joinAddress);
  parser.addOption("autojoin",       'o',  &results.autoJoin);
  parser.addOption("ui-config",      'u',  &results.uiConfig);
  parser.addOption("commands",       'c',  &results.autoCommands, ";");
  parser.addOption("help",           'h',  &results.help);
  parser.addOption("version",        'V',  &results.version);

  if (parser.parse(optionsFile, argc, argv)) {
    /* There was a problem */
    cout << "error(s) processing options:\n" <<
      stringUtils_join(parser.getErrors(), "\n") << '\n' << endl;
    usage();
    exit(EXIT_FAILURE);
  }

  /*SAKUSEN_DEBUG("options.unixSockets=" << results.unixSockets);*/

  return results;
}

UI::Ptr newUI(
    const Options& o
#ifdef DISABLE_CAIRO
__attribute__((unused))
#endif
,
    const boost::filesystem::path& uiConfFilePath
#ifdef DISABLE_CAIRO
__attribute__((unused))
#endif
,
    Game* game
#ifdef DISABLE_CAIRO
__attribute__((unused))
#endif
  )
{
  /** \todo Support alternate UIs (OpenGL, DirectX) */
  boost::filesystem::ifstream uiConf(uiConfFilePath);
#ifndef DISABLE_CAIRO
  UI::Ptr ui(new CairoUI(o.sdlOptions, uiConf, game));
#else
  SAKUSEN_FATAL("No UI enabled at compile time");
  UI::Ptr ui;
#endif
  ui->setTitle("tedomari");
  return ui;
}

