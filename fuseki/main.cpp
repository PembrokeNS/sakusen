#include "global.h"

#include <cerrno>
#include <iostream>

#include <sys/stat.h>

#include <boost/filesystem/operations.hpp>

#include <optimal/optionsparser.h>

#ifdef __GNUC__
  #ifdef ENABLE_LTDL_HACKED
    #include <ltdl_hacked/ltdl_hacked.h>
  #else
    #include <ltdl.h>
  #endif
#endif //Not condoned here.

#include <sakusen/fileutils.h>
#include <sakusen/comms/socket.h>
#include <sakusen/comms/errorutils.h>
#include <sakusen/resources/fileresourceinterface.h>
#include "server.h"

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
 */
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
  /** What address to use for unix joining socket */
  String unixAddress;
#endif
  /** What address to use for TCP joining socket */
  String tcpAddress;
  /** What paths to search for plugins */
  list<String> pluginPaths;
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
int startServer(const Options& options);
Options getOptions(
    const boost::filesystem::path& optionsFile,
    int argc,
    char const* const* argv
  );
void usage();

/* Main function of the sakusen server */

int main(int argc, char* const* argv)
{
  boost::filesystem::path fusekiConfigFile =
    fileUtils_configDirectory() / "fuseki" / "config";

  /* Parse config file and command line arguments */
  Options options = getOptions(fusekiConfigFile, argc, argv);

  if (options.help) {
    usage();
    return EXIT_SUCCESS;
  }

  if (options.version) {
    cout << FUSEKI_APPLICATION_NAME << " " << FUSEKI_APPLICATION_VERSION <<
      endl;
    return EXIT_SUCCESS;
  }

  cout << "Looked for fuseki config file at " <<
    fusekiConfigFile.string() << endl;

  return startServer(options);
}

/** Prints usage information */

void usage()
{
  cout <<
  "fuseki is a server for the game sakusen, accepting connections\n"
  "from clients through unix(7) and tcp(7) sockets.\n"
  "\n"
  "Usage: fuseki [OPTIONS]\n"
  "\n"
  " -d-, --no-dots          do not print dots while server running.\n"
  " -h,  --help             display help and exit.\n"
  " -V,  --version          display version information and exit.\n"
#ifndef DISABLE_UNIX_SOCKETS
  " -a-, --no-abstract      do not use the abstract unix socket namespace.\n"
  " -f,  --force-socket     overwrite any existing socket file.\n"
  " -x,  --unix ADDRESS     bind the unix socket at sakusen-style address\n"
  "                         ADDRESS (e.g. concrete|/var/fuseki/socket)\n"
  "                         (default is a UNIX socket in ~/.sakusen/fuseki).\n"
#endif
  " -t,  --tcp ADDRESS      Bind tcp socket at ADDRESS.\n"
  "                         Default: localhost, specify a null ADDRESS to disable.\n"
  " -p,  --plugins PATH:... Search for plugins in each given PATH, in given order.\n"
  "                         A plugin foo is expected at PATH/foo/foo.la or\n"
  "                         PATH/foo/foo.dll according to platform.\n"
  "\n"
  "Port for TCP socket defaults to "<<SAKUSEN_COMMS_DEFAULT_PORT<<
    " if not specified.\n"
  "Some examples: \n"
  "fuseki -t \"\" \n"
  "fuseki -t 192.168.1.1:1775 \n"
  "\n"
  "This is an alpha version of fuseki.  Functionality is extremely limited."
  << endl;
}

/** Parses the config file and command line */
Options getOptions(
    const boost::filesystem::path& optionsFile,
    int argc,
    char const* const* argv
  )
{
  OptionsParser parser;
  Options results;

  #ifndef DISABLE_UNIX_SOCKETS
    parser.addOption("abstract",     'a', &results.abstract);
    parser.addOption("unix",         'x', &results.unixAddress);
    parser.addOption("force-socket", 'f', &results.forceSocket);
  #endif
  parser.addOption("tcp",          't', &results.tcpAddress);
  parser.addOption("plugins",      'p', &results.pluginPaths, ":");
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

int startServer(const Options& options)
{
  cout << "*****************************\n"
          "* fuseki (a Sakusen server) *\n"
          "* Similus est circo mortis! *\n"
          "*****************************" << endl;

  /* ltdl initialization */
#ifdef __GNUC__
  if (lt_dlinit()) {
    SAKUSEN_FATAL("lt_dlinit() failed");
  }
#endif

  /* Create the resource interface */
  /** \todo make the directories searched configurable */
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
      FileResourceInterface::create(dataDirs, true);

  cout << "Using the following directories for plugins:\n";
  copy(
      options.pluginPaths.begin(), options.pluginPaths.end(),
      ostream_iterator<String>(cout, "\n")
    );
  if (options.pluginPaths.empty()) {
    cout << "(None)\n";
  }

  /* Initialize sockets */
  Socket::socketsInit();

  #ifndef DISABLE_UNIX_SOCKETS
  String unixSocketAddress = options.unixAddress;

  if (unixSocketAddress.empty()) {
    /* By default, when Unix sockets enabled, this server listens for clients
     * on a unix datagram socket located in
     * ~/SAKUSEN_CONFIG_SUBDIR/SAKUSEN_COMMS_SOCKET_SUBDIR */

    /* Construct the server directory */
    boost::filesystem::path serverPath =
      fileUtils_configDirectory() / SAKUSEN_COMMS_SOCKET_SUBDIR;

    cout << "Using directory '" << serverPath.string() <<
      "' for unix socket.\n";

    /* Determine whether the directory exists */

    if (!boost::filesystem::exists(serverPath)) {
      /* Try to create the directory if it doesn't */
      cout << "Directory not found, trying to create it." <<
        endl;
      fileUtils_mkdirRecursive(serverPath);
    }

    /* Check that config directory is really a directory */
    if (!boost::filesystem::is_directory(serverPath)) {
      SAKUSEN_FATAL("path '" << serverPath.string() <<
          "' not a directory");
    }

    boost::filesystem::path unixSocketPath = serverPath / "fuseki-socket";

    if (boost::filesystem::exists(unixSocketPath)) {
      if (options.forceSocket) {
        cout << "Removing existing socket." << endl;
        boost::filesystem::remove(unixSocketPath);
      } else {
        cout << "Socket already exists.  Another instance of fuseki is\n"
          "already running or has crashed.  Please delete the socket file or\n"
          "execute fuseki with the --force-socket option if no other instance\n"
          "is using it." << endl;
        return EXIT_FAILURE;
      }
    }

    /* Use the default socket address */
    unixSocketAddress =
      "concrete"SAKUSEN_COMMS_ADDR_DELIM + unixSocketPath.string();
  }

  cout << "Creating unix socket " << unixSocketAddress << endl;
  Socket::Ptr unixSocket =Socket::newBindingToAddress(
        "unix"SAKUSEN_COMMS_ADDR_DELIM+unixSocketAddress
      );

  if (unixSocket == NULL) {
    cout << "Error creating unix socket.  Check the address and try "
      "again." << endl;
    return EXIT_FAILURE;
  }
  #endif // DISABLE_UNIX_SOCKETS

  String bindAddress = options.tcpAddress;
  Socket::Ptr tcpSocket;

  if (!bindAddress.empty()) {
    cout << "Starting TCP socket at " << bindAddress << endl;
    tcpSocket =
      Socket::newBindingToAddress("tcp"SAKUSEN_COMMS_ADDR_DELIM+bindAddress);

    if (tcpSocket==NULL) {
      cout << "Error creating TCP socket at:" << endl;
      cout << bindAddress << endl;
      cout << "Check the address and try again." << endl;
      return EXIT_FAILURE;
    }
  }

  cout << "Sockets created." << endl;

  if (
#ifndef DISABLE_UNIX_SOCKETS
      unixSocket == NULL &&
#endif
      tcpSocket == NULL) {
    cout << "There is no socket for clients to join.  This server cannot work." <<
      endl;
    return EXIT_FAILURE;
  }

  { /* Braces to ensure that server is destructed before lt_dlexit called */
    vector<boost::filesystem::path> pluginPaths;
    copy(
        options.pluginPaths.begin(), options.pluginPaths.end(),
        back_inserter(pluginPaths)
      );
    Server server(
        cout, resourceInterface,
        pluginPaths,
#ifndef DISABLE_UNIX_SOCKETS
        options.abstract, unixSocket,
#endif
        tcpSocket, options.dots
      );
    server.serve();
  }

#ifdef __GNUC__
  /* ltdl finalization */
  if (lt_dlexit()) {
    SAKUSEN_FATAL("lt_dlexit() failed");
  }
#endif

  return EXIT_SUCCESS;
}

