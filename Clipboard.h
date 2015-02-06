#ifndef CLIPBOARD_H
#define	CLIPBOARD_H

#include <vector>
#include <deque>
#include <string>
#include <memory>
#include <cstddef>
#include <boost/filesystem.hpp>
#include "TreeNode.h"
#include "Directory.h"

class Clipboard {
public:
  Clipboard() : tree_(new Directory()) {}
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
   * @param removeParent whether empty parent directories should be removed
   */
  bool remove(const std::string &file, bool removeParent, std::vector<std::string> &messages);
  
  /**
   * Remove multiple files from the clipboard.
   * @param files a vector of absolute paths
   * @param removeParent whether empty parent directories should be removed
   */
  bool remove(const std::vector<std::string> &files, bool removeParent, std::vector<std::string> &messages);
  
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
   * @param f a unary function that accepts a string and a bool
   * @param[out] files the paths of the files in the clipboard
   */
  template<typename Func>
  bool forEachFile(const boost::filesystem::path &path, bool absolute, Func f, std::vector<std::string> &messages);
  
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
  std::unique_ptr<Directory> tree_;
  std::deque<std::unique_ptr<Directory>> stash_;
  
  /**
   * Same as forEachFile, but with additional parameters to allow for recursion.
   * @param directory the node of the tree corresponding to @c path
   * @param path a canonical path
   * @param base the path to prepend to all the returned paths
   */
  template<typename Func>
  bool forEachFile(const Directory &directory,  const boost::filesystem::path &path, const boost::filesystem::path &base, Func fn, std::vector<std::string> &messages);
  
  /**
   * Returns a path to the lowest common ancestor of all files in the
   * given tree.
   */
  boost::filesystem::path lowestCommonAncestor(const Directory &tree);
};


template<typename Func>
bool Clipboard::forEachFile(const boost::filesystem::path &path_, bool absolute,
        Func fn, std::vector<std::string> &messages){
  boost::system::error_code ec;
  boost::filesystem::path path("");
  
  if(path_.string() != ""){
    path = boost::filesystem::canonical(path_, ec);
    if(ec.value() != boost::system::errc::success){
      messages.push_back("Cannot access " + path_.string() + ": " + ec.message());
      return false;
    }
  }

  boost::filesystem::path currentPath("");
  Directory *currentDir = tree_.get();
  for(auto it = path.begin(); it != path.end(); ++it){
    const boost::filesystem::path &p = *it;
    Directory::iterator childIt = currentDir->find(p.string());
    if(childIt==currentDir->end() || !childIt->second->directory())
      return true;
    
    currentPath /= p;
    currentDir = static_cast<Directory *>(childIt->second.get());
  }
  
  return forEachFile(*currentDir, path, absolute ? path : "", fn, messages);
}

template<typename Func>
bool Clipboard::forEachFile(const Directory &directory, const boost::filesystem::path &path,
        const boost::filesystem::path &base, Func fn, std::vector<std::string> &messages){
  for(const auto &p : directory){
    TreeNode &f = *p.second.get();
    Directory &dir = static_cast<Directory &>(f);
    // dir might be garbage! (have to check f.directory())
    if(f.directory() && !dir.empty()){
      forEachFile(dir, path / f.name(), base / f.name(), fn, messages);
    }else{
      fn((base / f.name()).string(), f.directory() ? dir.recursive() : false);
    }
  }
  
  return true;
}

#endif	/* CLIPBOARD_H */

