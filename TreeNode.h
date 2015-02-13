#ifndef TREENODE_H
#define	TREENODE_H

#include <memory>
#include <unordered_map>
#include <string>

class TreeNode {
public:
  typedef std::unordered_map<std::string,std::unique_ptr<TreeNode> > node_map;
  typedef node_map::iterator iterator;
  typedef node_map::const_iterator const_iterator;
  typedef node_map::value_type value_type;
  typedef node_map::size_type size_type;
  typedef node_map::difference_type difference_type;
  
  TreeNode(const std::string &name, bool inClipboard) : name_(name), parent_(nullptr),
    recursive_(false), inClipboard_(inClipboard){}
  TreeNode() : TreeNode("", false) {}
  virtual ~TreeNode() {}
  virtual std::string name() const { return name_; }
  virtual void parent(TreeNode *parent){ parent_ = parent; }
  virtual TreeNode *parent() const { return parent_; }
  
  bool recursive() const { return recursive_; }
  void recursive(bool recursive){
    if(recursive && !recursive_) children_.clear();
    recursive_ = recursive;
  }
  bool inClipboard() const { return inClipboard_; }
  void inClipboard(bool inClipboard){ inClipboard_ = inClipboard; }
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
  std::string name_;
  TreeNode *parent_;
  node_map children_;
  bool recursive_;
  bool inClipboard_;
};

#endif	/* TREENODE_H */

