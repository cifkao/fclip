#ifndef DIRECTORY_H
#define	DIRECTORY_H

#include <memory>
#include <unordered_map>
#include <string>
#include "TreeNode.h"

class Directory : public TreeNode {  
public:
  typedef std::unordered_map<std::string,std::unique_ptr<TreeNode>> node_map;
  typedef node_map::iterator iterator;
  typedef node_map::const_iterator const_iterator;
  typedef node_map::value_type value_type;
  typedef node_map::size_type size_type;
  typedef node_map::difference_type difference_type;
  
  explicit Directory(const std::string &name) : TreeNode(name), recursive_(false), permanent_(false) {}
  Directory() : Directory("") {}
  virtual ~Directory() {}
  virtual bool directory() const { return true; }
  bool recursive() const { return recursive_; }
  void recursive(bool recursive){
    if(recursive && !recursive_) children_.clear();
    recursive_ = recursive;
  }
  bool permanent() const { return permanent_; }
  void permanent(bool permanent){ permanent_ = permanent; }
  iterator add(TreeNode *node);
  iterator remove(iterator it) { return children_.erase(it); }
  void remove(const std::string &name) { children_.erase(name); }
  iterator find(const std::string &name) { return children_.find(name); }
  const_iterator find(const std::string &name) const { return children_.find(name); }
  iterator begin() { return children_.begin(); }
  const_iterator begin() const { return children_.begin(); }
  const_iterator cbegin() const { return children_.begin(); }
  iterator end() { return children_.end(); }
  const_iterator end() const { return children_.end(); }
  const_iterator cend() const { return children_.end(); }
  size_type size() const { return children_.size(); }
  bool empty() const { return children_.size()==0; }
  void clear() { children_.clear(); }
  
private:
  node_map children_;
  bool recursive_;
  bool permanent_;
};

#endif	/* DIRECTORY_H */

