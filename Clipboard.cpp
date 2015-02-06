#include <string>
#include <vector>
#include <cstddef>
#include <boost/filesystem.hpp>
#include <boost/system/error_code.hpp>
#include "file_functions.h"
#include "Directory.h"
#include "File.h"
#include "Clipboard.h"

using namespace std;
namespace fs = boost::filesystem;

bool Clipboard::add(const vector<string> &files, bool recursive, vector<string> &messages){
  for(auto const file : files){
    add(file, recursive, messages);
  }
  return true;
}

bool Clipboard::add(const string &filename, bool recursive, vector<string> &messages){
  fs::path path(filename);
  
  boost::system::error_code ec;
  fs::file_status fstatus = fs::symlink_status(path, ec);
  if(fs::status_known(fstatus)){
    path = file_functions::getCanonicalPathToSymlink(path, ec);
  }
  
  if(ec.value() != boost::system::errc::success){
    messages.push_back("Cannot access " + filename + ": " + ec.message());
    return false;
  }
  
  fs::path currentPath("");
  TreeNode *current = tree_.get();
  
  auto targetIt = --path.end(); // the iterator pointing to the filename
  for(auto it = path.begin(); it != path.end(); ++it){
    const fs::path &p = *it;
    currentPath /= p;
    Directory *currentDir = static_cast<Directory *>(current);
    
    if(currentDir->recursive())
      break;
    
    // get the next node on the path
    Directory::iterator childIt = currentDir->find(p.string());
    
    if(it != targetIt || fs::is_directory(fstatus)){
      if(childIt == currentDir->end()){
        childIt = currentDir->add(new Directory(p.string()));
      }else if(!childIt->second->directory()){
        currentDir->remove(childIt);
        childIt = currentDir->add(new Directory(p.string()));
      }
      current = childIt->second.get();
    }else{
      if(childIt == currentDir->end()){
        childIt = currentDir->add(new File(p.string()));
      }else if(childIt->second->directory()){
        currentDir->remove(childIt);
        childIt = currentDir->add(new File(p.string()));
      }
      current = childIt->second.get();
    }
    
    if(it == targetIt){
      if(current->directory()){
        currentDir = static_cast<Directory *>(current);
        currentDir->recursive(recursive);
        currentDir->permanent(true);
      }
      break;
    }
  }
  
  return true;
}

bool Clipboard::remove(const vector<string> &files, bool removeParent, vector<string> &messages){
  for(auto const file : files){
    remove(file, removeParent, messages);
  }
  return true;
}

bool Clipboard::remove(const string &filename, bool removeParent, vector<string> &messages){
  boost::system::error_code ec;
  fs::path path = file_functions::getCanonicalPathToSymlink(filename, ec);
  if(ec.value() != boost::system::errc::success){
    messages.push_back("Cannot access " + filename + ": " + ec.message());
    return false;
  }
  
  fs::path currentPath("");
  TreeNode *current = tree_.get();
  
  bool found = false; // if we found the file we wanted to remove
  auto targetIt = --path.end(); // the iterator pointing to the filename
  Directory *currentDir = nullptr;
  for(auto it = path.begin(); it != path.end(); ++it){
    const fs::path &p = *it;
    currentDir = static_cast<Directory *>(current);
    
    if(currentDir->recursive()){
      // we need to manually add all files in the directory so that we can
      // remove the one we want to remove
      fs::directory_iterator dirIt(currentPath, ec);
      if(ec.value() != boost::system::errc::success){
        messages.push_back("Cannot access " + currentPath.string() + ": " + ec.message());
        return false;
      }
      currentDir->recursive(false);
      for(fs::directory_iterator eod; dirIt != eod; ++dirIt){
        fs::file_status s = dirIt->symlink_status(ec);
        if(fs::status_known(s)){
          if(fs::is_directory(s)){
            Directory *newDir = new Directory(dirIt->path().filename().string());
            newDir->recursive(true);
            currentDir->add(newDir);
          }else{
            currentDir->add(new File(dirIt->path().filename().string()));
          }
        }
      }
    }
    
    Directory::iterator childIt = currentDir->find(p.string());
    if(childIt == currentDir->end()) break;
    
    if(it != targetIt){
      current = childIt->second.get();
    }else{
      currentDir->remove(childIt);
      found = true;
    }
    
    currentPath /= p;
  }
  
  // clear empty non-permanent parent directories
  if(found && removeParent){
    while(currentDir != nullptr && currentDir->parent() != nullptr &&
            currentDir->empty() && !currentDir->permanent()){
      Directory *parent = currentDir->parent();
      parent->remove(currentDir->name());
      currentDir = parent;
    }
  }
  
  return true;
}

bool Clipboard::directoryListing(const fs::path &_path, vector<string> &files,
        vector<string> &messages){
  boost::system::error_code ec;
  fs::path path = fs::canonical(_path, ec);
  if(ec.value() != boost::system::errc::success){
    messages.push_back("Cannot access " + _path.string() + ": " + ec.message());
    return false;
  }
  
  fs::path currentPath("");
  Directory *currentDir = tree_.get();
  for(auto it = path.begin(); it != path.end(); ++it){
    const fs::path &p = *it;
    Directory::iterator childIt = currentDir->find(p.string());
    if(childIt==currentDir->end() || !childIt->second->directory())
      return true;
    
    currentPath /= p;
    currentDir = static_cast<Directory *>(childIt->second.get());
    
    if(currentDir->recursive()){
      if(fs::is_directory(path, ec) && ec.value() == boost::system::errc::success){
        fs::directory_iterator it(path, ec), eod;
        if(ec.value() != boost::system::errc::success){
          messages.push_back("Cannot access " + path.string() + ": " + ec.message());
          return false;
        }
        // push filenames
        transform(it, eod, back_inserter(files),
                [](const fs::directory_entry &e){ return e.path().filename().string(); });
      }
      return true;
    }
  }
  for(auto const &f : *currentDir){
    files.push_back(f.first);
  }
  
  return true;
}

bool Clipboard::stash(std::vector<std::string> &messages){
  if(tree_->empty()){
    messages.push_back("Nothing to stash");
    return false;
  }
  stash_.push_front(move(tree_));
  tree_.reset(new Directory());
  return true;
}
  
bool Clipboard::unstash(vector<string> &messages){
  if(!tree_->empty()){
    messages.push_back("Cannot unstash: clipboard is not empty");
    return false;
  }
  if(stash_.empty()){
    messages.push_back("Cannot unstash: no stash found");
    return false;
  }
  tree_ = move(stash_.front());
  stash_.pop_front();
  return true;
}

bool Clipboard::unstash(size_t stashId, vector<string> &messages){
  if(stashId==0)
    return unstash(messages);
  
  if(!tree_->empty()){
    messages.push_back("Cannot unstash: clipboard is not empty");
    return false;
  }
  if(stash_.size()<stashId+1){
    messages.push_back("Cannot unstash: no stash #" + to_string(stashId));
    return false;
  }
  tree_ = move(stash_[stashId]);
  stash_.erase(stash_.begin() + stashId);
  return true;
}

vector<string> Clipboard::listStash(){
  vector<string> list;
  for(const auto &tree : stash_){
    list.push_back(lowestCommonAncestor(*tree.get()).string());
  }
  return move(list);
}

bool Clipboard::dropStash(vector<string> &messages){
  if(stash_.empty()){
    messages.push_back("Cannot drop stash: no stash found");
    return false;
  }
  stash_.pop_front();
  return true;
}
  
bool Clipboard::dropStash(size_t stashId, vector<string> &messages){
  if(stash_.size()<stashId+1){
    messages.push_back("Cannot drop stash #" + to_string(stashId) + ": no such stash");
    return false;
  }
  stash_.erase(stash_.begin() + stashId);
  return true;
}

/* PRIVATE FUNCTIONS */

fs::path Clipboard::lowestCommonAncestor(const Directory &tree){
  fs::path path(tree.name());
  const Directory *currentDir = &tree;
  while(currentDir->size()==1 && !currentDir->permanent()){
    const TreeNode *f = currentDir->begin()->second.get();
    path /= f->name();
    if(f->directory()){
      currentDir = static_cast<const Directory *>(f);
    }else{
      return path;
    }
  }
  
  return path;
}
