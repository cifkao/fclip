
/*
 *	This file was automatically generated by dbusxx-xml2cpp; DO NOT EDIT!
 */

#ifndef __dbusxx__Fclip_proxy_h__PROXY_MARSHAL_H
#define __dbusxx__Fclip_proxy_h__PROXY_MARSHAL_H

#include <dbus-c++/dbus.h>
#include <cassert>

namespace com {
namespace fclip {

class Fclip_proxy
: public ::DBus::InterfaceProxy
{
public:

    Fclip_proxy()
    : ::DBus::InterfaceProxy("com.fclip.Fclip")
    {
    }

public:

    /* properties exported by this interface */
public:

    /* methods exported by this interface,
     * this functions will invoke the corresponding methods on the remote objects
     */
    void Add(const std::vector< std::string >& files, const bool& recursive, std::vector< std::string >& messages, bool& success)
    {
        ::DBus::CallMessage call;
        ::DBus::MessageIter wi = call.writer();

        wi << files;
        wi << recursive;
        call.member("Add");
        ::DBus::Message ret = invoke_method (call);
        ::DBus::MessageIter ri = ret.reader();

        ri >> messages;
        ri >> success;
    }

    void Remove(const std::vector< std::string >& files, const bool& removeParent, std::vector< std::string >& messages, bool& success)
    {
        ::DBus::CallMessage call;
        ::DBus::MessageIter wi = call.writer();

        wi << files;
        wi << removeParent;
        call.member("Remove");
        ::DBus::Message ret = invoke_method (call);
        ::DBus::MessageIter ri = ret.reader();

        ri >> messages;
        ri >> success;
    }

    void Clear()
    {
        ::DBus::CallMessage call;
        call.member("Clear");
        ::DBus::Message ret = invoke_method (call);
    }

    void ListFilesToStream(const std::string& directory, const bool& absolute, const std::string& stream, std::vector< std::string >& messages, bool& success)
    {
        ::DBus::CallMessage call;
        ::DBus::MessageIter wi = call.writer();

        wi << directory;
        wi << absolute;
        wi << stream;
        call.member("ListFilesToStream");
        ::DBus::Message ret = invoke_method (call);
        ::DBus::MessageIter ri = ret.reader();

        ri >> messages;
        ri >> success;
    }

    void DirectoryListing(const std::string& directory, std::vector< std::string >& files, std::vector< std::string >& messages, bool& success)
    {
        ::DBus::CallMessage call;
        ::DBus::MessageIter wi = call.writer();

        wi << directory;
        call.member("DirectoryListing");
        ::DBus::Message ret = invoke_method (call);
        ::DBus::MessageIter ri = ret.reader();

        ri >> files;
        ri >> messages;
        ri >> success;
    }

    std::string LowestCommonAncestor()
    {
        ::DBus::CallMessage call;
        call.member("LowestCommonAncestor");
        ::DBus::Message ret = invoke_method (call);
        ::DBus::MessageIter ri = ret.reader();

        std::string argout;
        ri >> argout;
        return argout;
    }

    void Stash(std::vector< std::string >& messages, bool& success)
    {
        ::DBus::CallMessage call;
        call.member("Stash");
        ::DBus::Message ret = invoke_method (call);
        ::DBus::MessageIter ri = ret.reader();

        ri >> messages;
        ri >> success;
    }

    void Unstash(const uint32_t& n, std::vector< std::string >& messages, bool& success)
    {
        ::DBus::CallMessage call;
        ::DBus::MessageIter wi = call.writer();

        wi << n;
        call.member("Unstash");
        ::DBus::Message ret = invoke_method (call);
        ::DBus::MessageIter ri = ret.reader();

        ri >> messages;
        ri >> success;
    }

    std::vector< std::string > ListStash()
    {
        ::DBus::CallMessage call;
        call.member("ListStash");
        ::DBus::Message ret = invoke_method (call);
        ::DBus::MessageIter ri = ret.reader();

        std::vector< std::string > argout;
        ri >> argout;
        return argout;
    }

    void DropStash(const uint32_t& n, std::vector< std::string >& messages, bool& success)
    {
        ::DBus::CallMessage call;
        ::DBus::MessageIter wi = call.writer();

        wi << n;
        call.member("DropStash");
        ::DBus::Message ret = invoke_method (call);
        ::DBus::MessageIter ri = ret.reader();

        ri >> messages;
        ri >> success;
    }

    void ClearStash()
    {
        ::DBus::CallMessage call;
        call.member("ClearStash");
        ::DBus::Message ret = invoke_method (call);
    }


public:

    /* signal handlers for this interface
     */

private:

    /* unmarshalers (to unpack the DBus message before calling the actual signal handler)
     */
};

} } 
#endif //__dbusxx__Fclip_proxy_h__PROXY_MARSHAL_H
