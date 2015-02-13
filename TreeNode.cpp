#include <utility>
#include <memory>
#include "TreeNode.h"

using namespace std;

TreeNode::iterator TreeNode::add(TreeNode *node){
  if(!recursive_){
    node->parent(this);
    return children_.insert(make_pair(node->name(), unique_ptr<TreeNode>(node))).first;
  }else{
    return children_.end();
  }
}