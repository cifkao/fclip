#include <string>
#include <vector>
#include <tuple>
#include <cstddef>
#include <boost/filesystem.hpp>
#include <boost/system/error_code.hpp>
#include "file_functions.h"
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
    messages.push_back("cannot access " + filename + ": " + ec.message());
    return false;
  }
  
  fs::path currentPath("");
  TreeNode *current = tree_.get();
  
  auto targetIt = --path.end(); // the iterator pointing to the filename
  for(auto it = path.begin(); it != path.end(); ++it){
    const fs::path &p = *it;
    currentPath /= p;
    
    if(current->recursive())
      break;
    
    // get the next node on the path
    TreeNode::iterator childIt = current->find(p.string());
    
    if(childIt == current->end()){ // child not found => create it
      childIt = current->add(new TreeNode(p.string(), false));
    }
    
    current = childIt->second.get();
    
    if(it == targetIt){
      current->recursive(recursive);
      current->inClipboard(true);
      break;
    }
    
  }
  
  return true;
}

bool Clipboard::remove(const vector<string> &files, bool recursive,
        vector<string> &messages){
  for(auto const file : files){
    remove(file, recursive, messages);
  }
  return true;
}

bool Clipboard::remove(const string &filename, bool recursive, vector<string> &messages){
  boost::system::error_code ec;
  fs::path path = file_functions::getCanonicalPathToSymlink(filename, ec);
  if(ec.value() != boost::system::errc::success){
    path = file_functions::normalizePath(filename);
    messages.push_back("cannot access " + filename + ": " + ec.message() +
      "; using " + path.string());
  }
  
  fs::path currentPath("");
  TreeNode *current = tree_.get();
  
  auto targetIt = --path.end(); // the iterator pointing to the filename
  for(auto it = path.begin(); it != path.end(); ++it){
    const fs::path &p = *it;
    
    if(current->recursive()){
      messages.push_back(filename + " cannot be removed, because " +
        currentPath.string() + " is marked as recursive");
      return false;
    }
    
    TreeNode::iterator childIt = current->find(p.string());
    if(childIt == current->end()) break;
    
    if(it != targetIt){
      current = childIt->second.get();
    }else{
      // we found the file we were looking for!
      if(!childIt->second->empty() && !childIt->second->recursive() && !recursive){
        // it has children, so we have to let it live
        childIt->second->inClipboard(false);
      }else{
        current->remove(childIt);
      }
    }
    
    currentPath /= p;
  }
  
  // clear parent directories that are not in the clipboard
  while(current != nullptr && current->parent() != nullptr &&
          current->empty() && !current->inClipboard()){
    TreeNode *parent = current->parent();
    parent->remove(current->name());
    current = parent;
  }
  
  return true;
}

bool Clipboard::directoryListing(const fs::path &_path,
        directoryListing_t &files, bool &recursive, vector<string> &messages){
  boost::system::error_code ec;
  fs::path path = fs::canonical(_path, ec);
  if(ec.value() != boost::system::errc::success){
    messages.push_back("cannot access " + _path.string() + ": " + ec.message());
    return false;
  }
  
  fs::path currentPath("");
  TreeNode *currentDir = tree_.get();
  for(auto it = path.begin(); it != path.end(); ++it){
    const fs::path &p = *it;
    TreeNode::iterator childIt = currentDir->find(p.string());
    if(childIt==currentDir->end())
      return true;
    
    currentPath /= p;
    currentDir = childIt->second.get();
    
    if(currentDir->recursive()){
      recursive = true;
      return true;
    }
  }
  
  recursive = false;
  for(auto const &f : *currentDir){
    files.push_back(make_tuple(f.first, f.second->inClipboard(), f.second->recursive()));
  }
  
  return true;
}

bool Clipboard::stash(std::vector<std::string> &messages){
  if(tree_->empty()){
    messages.push_back("nothing to stash");
    return false;
  }
  stash_.push_front(move(tree_));
  tree_.reset(new TreeNode());
  return true;
}
  
bool Clipboard::unstash(vector<string> &messages){
  if(!tree_->empty()){
    messages.push_back("cannot unstash: clipboard is not empty");
    return false;
  }
  if(stash_.empty()){
    messages.push_back("cannot unstash: no stash found");
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
    messages.push_back("cannot unstash: clipboard is not empty");
    return false;
  }
  if(stash_.size()<stashId+1){
    messages.push_back("cannot unstash: no stash #" + to_string(stashId));
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
    messages.push_back("cannot drop stash: no stash found");
    return false;
  }
  stash_.pop_front();
  return true;
}
  
bool Clipboard::dropStash(size_t stashId, vector<string> &messages){
  if(stash_.size()<stashId+1){
    messages.push_back("cannot drop stash #" + to_string(stashId) + ": no such stash");
    return false;
  }
  stash_.erase(stash_.begin() + stashId);
  return true;
}

/* PRIVATE FUNCTIONS */

fs::path Clipboard::lowestCommonAncestor(const TreeNode &tree){
  fs::path path(tree.name());
  const TreeNode *currentDir = &tree;
  while(currentDir->size()==1 && !currentDir->inClipboard()){
    const TreeNode *f = currentDir->begin()->second.get();
    path /= f->name();
    currentDir = f;
  }
  
  return path;
}
