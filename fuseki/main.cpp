#include "libsakusen-global.h"
#include "socket.h"
#include "errorutils.h"
#include "libsakusen-resources-global.h"
#include "fileresourceinterface.h"
#include "fileutils.h"
#include "fuseki-global.h"
#include "server.h"

#include <sys/stat.h>

#include <cerrno>
#include <iostream>
#include <optionsparser.h>

using namespace std;
using namespace optimal;

using namespace sakusen;
using namespace sakusen::comms;
using namespace sakusen::resources;
using namespace fuseki;

namespace fuseki {

/** Stores command-line options.
 * \todo More command line arguments (changing game name, etc.).
 *
 * Made complicated by UNIX having UNIX sockets and Windows not. 
 * Use of a UNIX socket in Windows results in crash, and Complications(R)
 * So under windows solicit==udp
 * and joining==tcp */
struct Options {
  /** \brief Default constructor
   *
   * Initializes all options to their default values */
  Options() :
#ifndef DISABLE_UNIX_SOCKETS
    abstract(true),
    forceSocket(false),
    unixAddress(),
#endif
    udpAddress("localhost"),
    tcpAddress("localhost"),
    dots(true),
    help(false),
    version(false) 
  {}
  
#ifndef DISABLE_UNIX_SOCKETS
  /** Whether to use abstract unix socket namespace where possible */
  bool abstract;
  /** Whether to delete an existing unix socket to replace it with a new
   * solicitation / joining one */
  bool forceSocket;
  /** What address to use for unix socket solicitation / joining socket */
  String unixAddress;
#endif
  /** What address to use for UDP solicitation socket */
  String udpAddress;
  /** What address to use for TCP joining socket */
  String tcpAddress;
  /** When set, fuseki will print dots all the time so that you know it is
   * alive */
  bool dots;
  /** When set, fuseki will print a help message and exit */
  bool help;
  /** When set, fuseki will print version information and exit */
  bool version;
};

}

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
  String homePath = fileUtils_getHome();
  
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

/** Prints usage information */

void usage()
{
  cout << "fuseki is a server for the game sakusen, accepting connections\n"
          "from clients through a unix(7) socket.\n"
          "\n"
          "Usage: fuseki [OPTIONS]\n"
          "\n"
          " -d-, --no-dots         do not print dots while server running\n"
          " -h,  --help            display help and exit\n"
          " -V,  --version         display version information and exit\n"
#ifndef DISABLE_UNIX_SOCKETS
          " -a-, --no-abstract     do not use the abstract unix socket namespace\n" 
          " -f,  --force-socket    overwrite any existing socket file\n"
          " -x,  --unix ADDRESS    bind the unix solicitation socket at sakusen-style address \n"
          "                        ADDRESS (e.g. concrete|/var/fuskeki/socket)\n"
          "                        (default is a UNIX socket in ~/.sakusen/fuseki)\n"
#endif
          " -u,  --udp ADDRESS     Bind udp socket at ADDRESS.\n"
          "                        Default: localhost, specify a null ADDRESS to disable\n."
          " -t,  --tcp ADDRESS     Bind tcp socket at ADDRESS.\n"
          "                        Default: localhost, specift a null ADDRESS to disable\n."
          "\n"
          "Port for UDP and TCP sockets defaults to "<<DEFAULT_PORT<<" if not specified.\n"
          "Some examples: \n"
          "fuseki -u localhost \n"
          "fuseki -u 192.168.1.1:1776 -t 192.168.1.1:1775 \n"
          "\n"
          "This is an alpha version of fuseki.  Functionality is extremely "
            "limited." << endl;
}

/** Parses the config file and command line */
Options getOptions(const String& optionsFile, int argc, char const* const* argv)
{
  OptionsParser parser;
  Options results;

  #ifndef DISABLE_UNIX_SOCKETS
    parser.addOption("abstract",     'a', &results.abstract);
    parser.addOption("unix",         'x', &results.unixAddress);
    parser.addOption("force-socket", 'f', &results.forceSocket);
  #endif
  parser.addOption("udp",          'u', &results.udpAddress);
  parser.addOption("tcp",          't', &results.tcpAddress);
  parser.addOption("dots",         'd', &results.dots);
  parser.addOption("help",         'h', &results.help);
  parser.addOption("version",      'V', &results.version);

  if (parser.parse(optionsFile, argc, argv)) {
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

  /* Create the resource interface */
  /** \todo make the directories searched configurable */
  ResourceInterface* resourceInterface =
    new FileResourceInterface(homePath + CONFIG_SUBDIR DATA_SUBDIR);
  
  /* Initialize sockets */
  Socket::socketsInit();
  
  #ifndef DISABLE_UNIX_SOCKETS
  String unixSocketAddress = options.unixAddress;
  
  if (unixSocketAddress.empty()) {
    /* By default, when Unix sockets enabled, this server listens for clients
     * on a unix datagram socket located in ~/CONFIG_SUBDIR/SOCKET_SUBDIR */

    /* Construct the server directory */
    struct stat tmpStat;
    String serverPath = homePath + CONFIG_SUBDIR SOCKET_SUBDIR;

    cout << "Using directory '" << serverPath << "' for unix socket.\n";
    
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

    String unixSocketPath = serverPath + FILE_SEP "fuseki-socket";

    if (!stat(unixSocketPath.c_str(), &tmpStat)) {
      if (options.forceSocket) {
        cout << "Removing existing socket." << endl;
        NativeUnlink(unixSocketPath.c_str());
      } else {
        cout << "Socket already exists.  Another instance of fuseki is\n"
          "already running or has crashed.  Please delete the socket file or\n"
          "execute fuseki with the --force-socket option if no other instance\n"
          "is using it." << endl;
        return EXIT_FAILURE;
      }
    }

    /* Use the default socket address */
    unixSocketAddress = "concrete"ADDR_DELIM + unixSocketPath;
  }

  cout << "Creating unix socket " << unixSocketAddress << endl;
  Socket* unixSocket =
    Socket::newBindingToAddress("unix"ADDR_DELIM+unixSocketAddress);

  if (unixSocket == NULL) {
    cout << "Error creating unix socket.  Check the address and try "
      "again." << endl;
    return EXIT_FAILURE;
  }
  #endif // DISABLE_UNIX_SOCKETS
  
  String bindAddress = options.udpAddress;
  Socket* udpSocket = NULL;
  
  if (!bindAddress.empty()) {
    cout << "Starting UDP (solicitation) socket at " << bindAddress << endl;
    udpSocket = Socket::newBindingToAddress("udp"ADDR_DELIM+bindAddress);

    if (udpSocket == NULL) {
      cout << "Error creating UDP socket at:"<< endl;
      cout << bindAddress << endl;
      cout << "Check the address and try again." << endl;
      return EXIT_FAILURE;
    }
  }

  bindAddress = options.tcpAddress;
  Socket* tcpSocket = NULL;
  
  if (!bindAddress.empty()) {
    cout << "Starting TCP (joining) socket at " << bindAddress << endl;
    tcpSocket = Socket::newBindingToAddress("tcp"ADDR_DELIM+bindAddress);

    if (tcpSocket==NULL) {
      cout << "Error creating TCP (joining) socket at:" << endl;
      cout << bindAddress << endl;
      cout << "Check the address and try again." << endl;
      return EXIT_FAILURE;
    }
  }

  cout << "Sockets created." << endl;

#ifdef DISABLE_UNIX_SOCKETS
  if (udpSocket == NULL) {
    cout << "There is no solicitation socket.  This server cannot work." <<
      endl;
    return EXIT_FAILURE;
  }
  
  if (tcpSocket == NULL) {
    cout << "There is no join socket.  This server cannot work." << endl;
    return EXIT_FAILURE;
  }
#endif
  
  Server server(
      cout, resourceInterface,
#ifndef DISABLE_UNIX_SOCKETS
      options.abstract, unixSocket,
#endif
      udpSocket, tcpSocket, options.dots
    );
  server.serve();
 
#ifndef DISABLE_UNIX_SOCKETS
  delete unixSocket;
#endif
  delete tcpSocket;
  delete udpSocket;
  delete resourceInterface;
  return EXIT_SUCCESS;
}

