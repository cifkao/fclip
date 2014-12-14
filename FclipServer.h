#ifndef FCLIPSERVER_H
#define	FCLIPSERVER_H

#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include "Clipboard.h"
#include "Fclip_adaptor.h"

class FclipServer : public com::fclip::Fclip_adaptor,
  public DBus::IntrospectableAdaptor,
  public DBus::ObjectAdaptor {
public:
  FclipServer (DBus::Connection &connection) :
    DBus::ObjectAdaptor(connection, "/Fclip") {}
    
  virtual void Add(const std::vector< std::string >& files,
    const bool& recursive,
    std::vector< std::string >& messages,
    bool& success)
  { success = clip_.add(files, recursive, messages); }
  
  virtual void Remove(const std::vector<std::string>& files,
    std::vector< std::string >& messages,
    bool& success)
  { success = clip_.remove(files, messages); }
  
  virtual void Clear() { clip_.clear(); }
  
  virtual void ListFiles(const std::string& directory,
    const bool& absolute,
    std::vector< std::string >& files,
    std::vector< std::string >& messages,
    bool& success)
  { success = clip_.listFiles(directory, absolute, files, messages); }
  
  virtual void DirectoryListing(const std::string& directory,
    std::vector< std::string >& files,
    std::vector< std::string >& messages,
    bool& success)
  { success = clip_.directoryListing(directory, files, messages); }
  
  virtual void Stash(std::vector< std::string >& messages, bool& success)
  { success = clip_.stash(messages); }
  
  virtual void Unstash(const uint32_t& n, std::vector< std::string >& messages, bool& success)
  { success = clip_.unstash(n, messages); }
  
  virtual std::vector<std::string> ListStash() { return clip_.listStash(); }
  
  virtual void DropStash(const uint32_t& n, std::vector< std::string >& messages, bool& success)
  { success = clip_.dropStash(n, messages); }
  
  virtual void ClearStash()
  { clip_.clearStash(); }
	
private:
  Clipboard clip_;
};

#endif
