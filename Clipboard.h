#ifndef CLIPBOARD_H
#define	CLIPBOARD_H

#include <vector>
#include <deque>
#include <string>
#include <memory>
#include <cstddef>
#include <boost/filesystem.hpp>
#include "TreeNode.h"

class Clipboard {
public:
  Clipboard() : tree_(new TreeNode()) {}
  virtual ~Clipboard() {}
  
  /**
   * Add a file to the clipboard.
   * @param file an absolute path to the file
   * @param recursive the value of the @c recursive flag if @c file is a directory
   */
  bool add(const std::string &file, bool recursive, std::vector<std::string> &messages);
  
  /**
   * Add multiple files to the clipboard.
   * @param files a vector of absolute paths
   * @param recursive the value of the @c recursive flag for directories
   */
  bool add(const std::vector<std::string> &files, bool recursive, std::vector<std::string> &messages);
  
  /**
   * Remove a file from the clipboard.
   * @param file an absolute path to the file
   * @param recursive remove directory contents recursively
   * @param removeParent whether empty parent directories should be removed
   */
  bool remove(const std::string &file, bool recursive, std::vector<std::string> &messages);
  
  /**
   * Remove multiple files from the clipboard.
   * @param files a vector of absolute paths
   * @param recursive remove directory contents recursively
   * @param removeParent whether empty parent directories should be removed
   */
  bool remove(const std::vector<std::string> &files, bool recursive, std::vector<std::string> &messages);
  
  /**
   * Removes all files from the clipboard.
   */
  void clear(){ tree_->clear(); }
  
  /**
   * Recursively list all files in the clipboard within a given path.
   * The path itself shall not be included.
   * For each file, the given function shall be called, passing the name
   * of the file and a boolean indicating whether the given file has the
   * recursive flag
   * @param path an absolute path
   * @param absolute if true, absolute paths will be returned;
   *        if false, all paths will be relative with respect to @c path
   * @param fn a binary function that accepts a string and a bool
   */
  template<typename Func>
  bool forEachFile(const boost::filesystem::path &path, bool absolute, Func fn, std::vector<std::string> &messages);
  
  /**
   * Lists all files and directories in a given directory that are in the
   * clipboard.
   * @param path an absolute path to the directory
   * @param[out] files the names of the files in the clipboard
   */
  bool directoryListing(const boost::filesystem::path &path, std::vector<std::string> &files, std::vector<std::string> &messages); 

  /**
   * Returns a path to the lowest common ancestor of all files in the
   * clipboard.
   */
  boost::filesystem::path lowestCommonAncestor()
  { return lowestCommonAncestor(*tree_.get()); }
  
  /**
   * If the clipboard is not empty, save its state in the stash.
   */
  bool stash(std::vector<std::string> &messages);
  
  /**
   * Restore the last stashed state (if any).
   */
  bool unstash(std::vector<std::string> &messages);
  
  /**
   * Restore a stashed state.
   * @param stashId the index of the stashed state
   */
  bool unstash(std::size_t stashId, std::vector<std::string> &messages);
  
  /**
   * List all stashed states on the stack.
   * @return a list containing the path to the lowest common ancestor for each
   *         stashed state
   */
  std::vector<std::string> listStash();
  
  /**
   * Drop the last stashed state (if any).
   */
  bool dropStash(std::vector<std::string> &messages);
  
  /**
   * Drop a stashed state.
   * @param stashId the index of the stashed state
   */
  bool dropStash(std::size_t stashId, std::vector<std::string> &messages);
  
  /**
   * Drop all stashed states.
   */
  void clearStash(){ stash_.clear(); }
  
private:
  std::unique_ptr<TreeNode> tree_;
  std::deque<std::unique_ptr<TreeNode>> stash_;
  
  /**
   * Add all files in a directory with a @c recursive flag and remove the flag.
   * @param directory the node of the tree corresponding to @c path
   * @param path a canonical path to a directory
   */
  bool expandDirectory(TreeNode *directory, const boost::filesystem::path &path, std::vector<std::string> &messages);
  
  /**
   * Same as forEachFile, but with additional parameters to allow for recursion.
   * @param directory the node of the tree corresponding to @c path
   * @param path a canonical path
   * @param base the path to prepend to all the returned paths
   * @param fn a binary function that accepts a string and a bool
   */
  template<typename Func>
  bool forEachFile(const TreeNode &directory, const boost::filesystem::path &path, const boost::filesystem::path &base, Func fn, std::vector<std::string> &messages);
  
  /**
   * Same as forEachFile, but only lists files in one directory (non-recursively),
   * marking all subdirectories as recursive.
   * @param directory the node of the tree corresponding to @c path
   * @param path a canonical path
   * @param absolute if true, absolute paths will be returned;
   *        if false, all paths will be relative with respect to @c path
   * @param fn a binary function that accepts a string and a bool
   */
  template<typename Func>
  bool forEachFileInRecursiveDirectory(const TreeNode &directory, const boost::filesystem::path &path, bool absolute, Func fn, std::vector<std::string> &messages);
  
  /**
   * Returns a path to the lowest common ancestor of all files in the
   * given tree.
   */
  boost::filesystem::path lowestCommonAncestor(const TreeNode &tree);
};


template<typename Func>
bool Clipboard::forEachFile(const boost::filesystem::path &path_, bool absolute,
        Func fn, std::vector<std::string> &messages){
  namespace fs = boost::filesystem;
  boost::system::error_code ec;
  boost::filesystem::path path("");
  
  if(path_.string() != ""){
    path = fs::canonical(path_, ec);
    if(ec.value() != boost::system::errc::success){
      messages.push_back("Cannot access " + path_.string() + ": " + ec.message());
      return false;
    }
  }

  // first we need to find the path in the tree
  fs::path currentPath("");
  TreeNode *currentDir = tree_.get();
  for(auto it = path.begin(); it != path.end(); ++it){
    const fs::path &p = *it;
    TreeNode::iterator childIt = currentDir->find(p.string());
    if(childIt==currentDir->end()){
      if(currentDir->recursive()){
        // the path doesn't exist in the tree, but it is present
        // implicitly (under a directory with the recursive flag)
        return forEachFileInRecursiveDirectory(*currentDir, path, absolute, fn, messages);
      }
      return true;
    }
    
    currentPath /= p;
    currentDir = childIt->second.get();
  }
  
  if(currentDir->recursive())
    return forEachFileInRecursiveDirectory(*currentDir, path, absolute, fn, messages);
  else
    return forEachFile(*currentDir, path, absolute ? path : "", fn, messages);
}

template<typename Func>
bool Clipboard::forEachFileInRecursiveDirectory(const TreeNode &directory,
        const boost::filesystem::path &path, bool absolute,
        Func fn, std::vector<std::string> &messages){
  namespace fs = boost::filesystem;
  boost::system::error_code ec;
  if(fs::is_directory(path, ec) && ec.value() == boost::system::errc::success){
    fs::directory_iterator it(path, ec), eod;
    if(ec.value() != boost::system::errc::success){
      messages.push_back("Cannot access " + path.string() + ": " + ec.message());
      return false;
    }

    fs::path base(absolute ? path : "");
    try{
      for(; it != eod; ++it){
        fn((base / (*it).path().filename()).string(), fs::is_directory((*it).path(), ec));
        if(ec.value() != boost::system::errc::success){
          messages.push_back("Cannot access " + path.string() + ": " + ec.message());
        }
      }
    }catch(const std::exception &ex){
      messages.push_back(ex.what());
      return false;
    }
  }
  return true;
}

template<typename Func>
bool Clipboard::forEachFile(const TreeNode &directory, const boost::filesystem::path &path,
        const boost::filesystem::path &base, Func fn, std::vector<std::string> &messages){
  for(const auto &p : directory){
    TreeNode &f = *p.second.get();
    if(f.inClipboard()){
      fn((base / f.name()).string(), f.recursive());
    }
    if(!f.empty()){
      forEachFile(f, path / f.name(), base / f.name(), fn, messages);
    }
  }
  
  return true;
}

#endif	/* CLIPBOARD_H */

