#ifndef CLIPBOARD_H
#define	CLIPBOARD_H

#include <vector>
#include <deque>
#include <string>
#include <memory>
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
   */
  bool remove(const std::string &file, std::vector<std::string> &messages);
  
  /**
   * Remove multiple files from the clipboard.
   * @param files a vector of absolute paths
   */
  bool remove(const std::vector<std::string> &files, std::vector<std::string> &messages);
  
  /**
   * Removes all files from the clipboard.
   */
  void clear(){ tree_->clear(); }
  
  /**
   * Recursively list all files in the clipboard within a given path.
   * The path itself shall not be included.
   * @param path an absolute path
   * @param absolute if true, absolute paths will be returned;
   *        if false, all paths will be relative with respect to @c path
   * @param[out] files the paths of the files in the clipboard
   */
  bool listFiles(const boost::filesystem::path &path, bool absolute, std::vector<std::string> &files, std::vector<std::string> &messages);
  
  /**
   * Lists all files and directories in a given directory that are in the
   * clipboard.
   * @param path an absolute path to the directory
   * @param[out] files the names of the files in the clipboard
   */
  bool directoryListing(const boost::filesystem::path &path, std::vector<std::string> &files, std::vector<std::string> &messages); 
  
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
  bool unstash(uint32_t stashId, std::vector<std::string> &messages);
  
  /**
   * List all stashed states on the stack.
   * @return a list containing the path to the lowest common ancestor for each
   *         stashed state
   */
  std::vector<std::string> listStash();
  
private:
  std::unique_ptr<Directory> tree_;
  std::deque<std::unique_ptr<Directory>> stash_;
  
  /**
   * Recursively list all files in the clipboard within a given path.
   * @param directory the node of the tree corresponding to @c path
   * @param path a canonical path
   * @param base the path to prepend to all the returned paths
   * @param[out] files the paths of the files in the clipboard
   */
  bool listFiles(const Directory &directory,  const boost::filesystem::path &path, const boost::filesystem::path &base, std::vector<std::string> &files, std::vector<std::string> &messages);
  
  /**
   * Recursively list all files in a given directory (regardless of their
   * presence in the clipboard).
   * @param directory a canonical path to the directory
   * @param base the path to prepend to all of the returned paths
   * @param[out] files the paths of the files
   */
  bool listFiles(const boost::filesystem::path &directory, const boost::filesystem::path &base, std::vector<std::string> &files, std::vector<std::string> &messages);
  
  /**
   * Returns a path to the lowest common ancestor of all files in the
   * given tree.
   */
  boost::filesystem::path lowestCommonAncestor(const Directory &tree);
};

#endif	/* CLIPBOARD_H */

