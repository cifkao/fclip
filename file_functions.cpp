#include <string>
#include <cstring>
#include <vector>
#include <algorithm>
#include <memory>
#include <cstdlib>
#include <unistd.h>
#include <libgen.h>
#include <boost/filesystem.hpp>
#include <boost/system/error_code.hpp>
#include "file_functions.h"

using namespace std;
namespace fs = boost::filesystem;

namespace file_functions {

  fs::path getCanonicalPathToSymlink(const fs::path &p, boost::system::error_code &ec){
    if(fs::is_symlink(p, ec) && fs::absolute(p).has_parent_path())
      return fs::canonical(fs::absolute(p).parent_path(), ec) / p.filename();
    else
      return fs::canonical(p, ec);
  }
  
}