#include "vfs/directorybranch.h"

#include "fileutils.h"
#include "filereader.h"
#include "filewriter.h"

#include <boost/filesystem/operations.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <pcrecpp.h>

using namespace std;

namespace sakusen {
namespace resources {
namespace vfs {

DirectoryBranch::DirectoryBranch(
    const String& name,
    const boost::filesystem::path& d,
    const Branch::Ptr& parent
  ) :
  Branch(name, parent),
  directory(d)
{
}

Branch::Ptr DirectoryBranch::getSubBranch(const String& subBranchName)
{
  /* for security reasons, it's crucial that the name contain no characters
   * that might be directory separators; for now we insist more
   * strongly that it match the FILENAME_REGEX. */
  if (!pcrecpp::RE(FILENAME_REGEX).FullMatch(subBranchName)) {
    Fatal("invalid name");
  }
  assert(!subBranchName.empty());
  if (subBranchName == "..") {
    return parent.lock();
  }
  if (subBranchName == ".") {
    return ptrToThis.lock();
  }
  
  boost::filesystem::path potential = directory / subBranchName;
  if (!boost::filesystem::exists(potential)) {
    return Branch::Ptr();
  } else if (boost::filesystem::is_directory(potential)) {
    return create(subBranchName, potential, ptrToThis.lock());
  } else {
    /** \todo Support archives, etc. */
    return Branch::Ptr();
  }
}

Branch::Ptr DirectoryBranch::createSubBranch(const String& subBranchName)
{
  /* for security reasons, it's crucial that the name contain no characters
   * that might be directory separators; for now we insist more
   * strongly that it match the FILENAME_REGEX. */
  if (!pcrecpp::RE(FILENAME_REGEX).FullMatch(subBranchName)) {
    Fatal("invalid name");
  }
  assert(!subBranchName.empty());
  /** \todo memoize? */
  if (subBranchName == "..") {
    return parent.lock();
  }
  if (subBranchName == ".") {
    return ptrToThis.lock();
  }
  
  boost::filesystem::path potential = directory / subBranchName;
  boost::filesystem::create_directory(potential);
  return create(subBranchName, potential, ptrToThis.lock());
}

boost::tuple<Resource, ResourceSearchResult> DirectoryBranch::getResource(
    const String& resourceName,
    const String& extension
  )
{
  if (!boost::filesystem::exists(directory)) {
    return Resource();
  }
  /* This hash map will contain as keys the filenames, and as values the paths
   * to those files.  We store the names also so that we don't consider it an
   * ambiguous result if there are two files of the same name in different
   * places; we rather assume that they are identical. */
  hash_map_string<boost::filesystem::path>::type matchingFiles(10);

  /*QDebug("Searching "<<directory.native_directory_string());*/
  list<boost::filesystem::path> newMatches =
    fileUtils_findMatches(directory, resourceName);
  while (!newMatches.empty()) {
    boost::filesystem::path path = newMatches.front();
    newMatches.pop_front();
    /*QDebug("Found "<<path.string());*/
    String fileName = path.leaf();
    /* check that the file has the correct extension */
    if (!boost::algorithm::ends_with(fileName, extension)) {
        /*QDebug("Looking for extension to "<<name<<" "<<extension);*/
      continue;
    }
    if (0 == matchingFiles.count(fileName)) {
      /*QDebug("Adding " << fileName << " at " << path);*/
      matchingFiles[fileName] = path;
    }
  }

  /* Check how many results we found */
  switch (matchingFiles.size()) {
    case 0:
      return boost::make_tuple(Resource(), resourceSearchResult_notFound);
    case 1:
      break;
    default:
      return boost::make_tuple(Resource(), resourceSearchResult_ambiguous);
  }

  /* We have found exactly one matching file, so we try to open it */
  assert(!matchingFiles.begin()->second.empty());
  /*QDebug("found it right");*/
  pair<String, boost::filesystem::path> matchingFile = *matchingFiles.begin();
  Reader::Ptr reader(new FileReader(matchingFile.second));
  return boost::make_tuple(
      Resource(
        getSakusenPath()+"/"+matchingFile.first, reader, matchingFile.second
      ),
      resourceSearchResult_success
    );
}

Writer::Ptr DirectoryBranch::getWriter(const String& writerName)
{
  /* for security reasons, it's crucial that the name contain no characters
   * that might be directory separators and isn't ..; for now we insist more
   * strongly that it match the FILENAME_REGEX. */
  if (!pcrecpp::RE(FILENAME_REGEX).FullMatch(writerName)) {
    Fatal("invalid name");
  }
  boost::filesystem::path filePath = directory / writerName;
  if (boost::filesystem::exists(filePath) &&
      boost::filesystem::is_directory(filePath)) {
    return Writer::Ptr();
  }
  return Writer::Ptr(new FileWriter(filePath));
}

}}}

