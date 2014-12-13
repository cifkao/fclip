#ifndef FILE_FUNCTIONS_H
#define	FILE_FUNCTIONS_H

#include <string>
#include <set>
#include <algorithm>
#include <iterator>
#include <boost/filesystem.hpp>

namespace file_functions {
  /**
   * Convert a path to a canonical path, but if the path refers to a symlink,
   * don't follow it.
   */
  boost::filesystem::path getCanonicalPathToSymlink(const boost::filesystem::path &p, boost::system::error_code &ec);
  
}

#endif	/* FILE_FUNCTIONS_H */

