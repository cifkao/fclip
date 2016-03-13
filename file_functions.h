#ifndef FILE_FUNCTIONS_H
#define	FILE_FUNCTIONS_H

#include <string>
#include <set>
#include <algorithm>
#include <iterator>
#include <boost/filesystem.hpp>
#include <boost/system/error_code.hpp>

namespace file_functions {
  /**
   * Convert a path to a canonical path, but if the path refers to a symlink,
   * don't follow it.
   */
  boost::filesystem::path getCanonicalPathToSymlink(const boost::filesystem::path &p, boost::system::error_code &ec);

  /**
   * Normalize a path by removing "." and "..". Not necessarily correct if the
   * path contains symlinks. Should be used only if getCanonicalPathToSymlink
   * fails.
   */
  boost::filesystem::path normalizePath(const boost::filesystem::path &p);

  /**
   * Convert a path to a relative path with respect to its prefix.
   */
  boost::filesystem::path removePathPrefix(const boost::filesystem::path &p, const boost::filesystem::path &prefix);

}

#endif	/* FILE_FUNCTIONS_H */

