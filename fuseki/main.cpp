#include "libsakusen-global.h"
#include "unixdatagramlisteningsocket.h"
#include "errorutils.h"
#include "libsakusen-resources-global.h"
#include "fileresourceinterface.h"
#include "fileutils.h"
#include "fuseki-global.h"
#include "server.h"

#include <sys/stat.h>
#include <getopt.h>

#include <cerrno>
#include <iostream>
#include <optionsparser.h>

using namespace std;
using namespace optimal;

using namespace sakusen;
using namespace sakusen::comms;
using namespace sakusen::resources;
using namespace fuseki;

/* struct to store options processed from the command line. */
  
/* TODO: More command line arguments (in particular allow for putting
 * socket elsewhere, changing game name, possibly allow for UDP/TCP socket,
 * etc.). */
struct Options {
  Options() :
    forceSocket(false),
    abstract(true),
    dots(true),
    help(false),
    version(false) 
  {}
  ~Options() {}
  bool forceSocket;
  bool abstract;
  bool dots;
  bool help;
  bool version;
};

/* Forward-declarations */
int startServer(const String& homePath, const Options& options);
Options getOptions(
    const String& optionsFile,
    int argc,
    char const* const* argv
  );
void usage();

/* Main function of the sakusen server */

int main(int argc, char* const* argv)
{
  /* Seek out the home directory */
  char* homePathPtr = getenv("HOME");

  if (homePathPtr == NULL) {
    Fatal("no home directory found.");
  }

  String homePath = String(homePathPtr);
  
  String fusekiConfigFile =
    homePath + CONFIG_SUBDIR FILE_SEP "fuseki" FILE_SEP "config";
  cout << "Looking for fuseki config file at " << fusekiConfigFile << endl;
  
  /* Parse config file and command line arguments */
  Options options = getOptions(fusekiConfigFile, argc, argv);

  if (options.help) {
    usage();
    return EXIT_SUCCESS;
  }

  if (options.version) {
    cout << APPLICATION_NAME << " " << APPLICATION_VERSION << endl;
    return EXIT_SUCCESS;
  }
  
  return startServer(homePath, options);
}

/* Function to print usage information */

void usage()
{
  cout << "fuseki is a server for the game sakusen, accepting connections\n"
          "from clients through a unix(7) socket.\n"
          "\n"
          "Usage: fuseki [OPTIONS]\n"
          "\n"
          " -f,  --force-socket overwrite any existing socket file\n"
          " -a-, --no-abstract  do not use the abstract unix socket namespace\n"
          " -d-, --no-dots      do not print dots while server running\n"
          " -h,  --help         display help and exit\n"
          " -V,  --version      display version information and exit\n"
          "\n"
          "This is an alpha version of fuseki.  Functionality is extremely "
            "limited." << endl;
}

/* Function to parse the command line */

Options getOptions(const String& optionsFile, int argc, char const* const* argv)
{
  OptionsParser parser;
  Options results;

  parser.addOption("force-socket", 'f', &results.forceSocket);
  parser.addOption("abstract",     'a', &results.abstract);
  parser.addOption("dots",         'd', &results.dots);
  parser.addOption("help",         'h', &results.help);
  parser.addOption("version",      'V', &results.version);

  if (parser.Parse(optionsFile, argc, argv)) {
    cout << "Error(s) parsing options:\n";
    cout << stringUtils_join(parser.getErrors(), "\n") << "\n";
    usage();
    exit(EXIT_FAILURE);
  }

  return results;
}

int startServer(const String& homePath, const Options& options)
{
  if (options.version) {
    cout << APPLICATION_NAME " " APPLICATION_VERSION << endl;
    return EXIT_SUCCESS;
  }
  
  cout << "*****************************\n"
          "* fuseki (a Sakusen server) *\n"
          "* Similus est circo mortis! *\n"
          "*****************************" << endl;

  /* This server listens for clients on a unix datagram socket located in
   * ~/CONFIG_SUBDIR/SOCKET_SUBDIR */

  /* Construct the config directory */
  struct stat tmpStat;
  String serverPath = homePath + CONFIG_SUBDIR SOCKET_SUBDIR;

  cout << "Using directory '" << serverPath << "' for socket.\n";

  /* Determine whether the directory exists */
  
  while (-1==stat(serverPath.c_str(), &tmpStat)) {
    switch (errno) {
      case ENOENT:
        /* Try to create the directory if it doesn't */
        cout << "Directory not found, trying to create it." <<
          endl;
        if (-1 == fileUtils_mkdirRecursive(serverPath, 0777)) {
          switch (errno) {
            case EACCES:
              Fatal("permission denied when creating directory");
              break;
            case ENOENT:
              Fatal("no such file or directory when creating directory");
              break;
            default:
              Fatal("error " << errorUtils_parseErrno(errno) <<
                  " creating directory");
              break;
          }
        }
        break;
      default:
        Fatal("could not stat directory");
        break;
    }
  }

  /* Check that config directory is really a directory */
  if (!S_ISDIR(tmpStat.st_mode)) {
    Fatal("path '" << serverPath <<
        "' not a directory");
  }

  /* Create the resource interface */
  /* TODO: make the directories searched configurable */
  ResourceInterface* resourceInterface =
    new FileResourceInterface(homePath + CONFIG_SUBDIR DATA_SUBDIR);

  /* Create the socket */
  String socketPath = serverPath + FILE_SEP "fuseki-socket";
  
  cout << "Binding socket at " << socketPath << endl;

  if (!stat(socketPath.c_str(), &tmpStat)) {
    if (options.forceSocket) {
      cout << "Removing existing socket." << endl;
      unlink(socketPath.c_str());
    } else {
      cout << "Socket already exists.  Another instance of fuseki is already\n"
        "running or has crashed.  Please delete the socket file or execute\n"
        "fuseki with the --force-socket option if no other instance is using\n"
        "it." << endl;
      return EXIT_FAILURE;
    }
  }
  Socket* socket = new UnixDatagramListeningSocket(socketPath, false);

  cout << "Socket created." << endl;
  
  Server server(
      socket, cout, resourceInterface, options.abstract, options.dots
    );
  server.serve();
  
  delete socket;
  delete resourceInterface;
  return EXIT_SUCCESS;
}

