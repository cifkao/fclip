#ifndef TREENODE_H
#define	TREENODE_H

#include <memory>
#include <string>

class Directory;

class TreeNode {
public:
  explicit TreeNode(const std::string &name) : name_(name), parent_(nullptr) {}
  virtual ~TreeNode() {}
  virtual bool directory() const = 0;
  virtual std::string name() const { return name_; }
  virtual void parent(Directory *parent){ parent_ = parent; }
  virtual Directory *parent() const { return parent_; }
  
private:
  std::string name_;
  Directory *parent_;
};

#endif	/* TREENODE_H */

