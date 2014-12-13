#ifndef FILE_H
#define	FILE_H

#include <string>
#include "TreeNode.h"

class File : public TreeNode {
public:
  explicit File(const std::string &name) : TreeNode(name) {}
  virtual ~File() {};
  virtual bool directory() const { return false; }
  
private:

};

#endif	/* FILE_H */

