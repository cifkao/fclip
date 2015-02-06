
/*
 *	This file was automatically generated by dbusxx-xml2cpp; DO NOT EDIT!
 */

#ifndef __dbusxx__Fclip_adaptor_h__ADAPTOR_MARSHAL_H
#define __dbusxx__Fclip_adaptor_h__ADAPTOR_MARSHAL_H

#include <dbus-c++/dbus.h>
#include <cassert>

namespace com {
namespace fclip {

class Fclip_adaptor
: public ::DBus::InterfaceAdaptor
{
public:

    Fclip_adaptor()
    : ::DBus::InterfaceAdaptor("com.fclip.Fclip")
    {
        register_method(Fclip_adaptor, Add, _Add_stub);
        register_method(Fclip_adaptor, Remove, _Remove_stub);
        register_method(Fclip_adaptor, Clear, _Clear_stub);
        register_method(Fclip_adaptor, ListFilesToStream, _ListFilesToStream_stub);
        register_method(Fclip_adaptor, DirectoryListing, _DirectoryListing_stub);
        register_method(Fclip_adaptor, LowestCommonAncestor, _LowestCommonAncestor_stub);
        register_method(Fclip_adaptor, Stash, _Stash_stub);
        register_method(Fclip_adaptor, Unstash, _Unstash_stub);
        register_method(Fclip_adaptor, ListStash, _ListStash_stub);
        register_method(Fclip_adaptor, DropStash, _DropStash_stub);
        register_method(Fclip_adaptor, ClearStash, _ClearStash_stub);
    }

    ::DBus::IntrospectedInterface *introspect() const 
    {
        static ::DBus::IntrospectedArgument Add_args[] = 
        {
            { "files", "as", true },
            { "recursive", "b", true },
            { "messages", "as", false },
            { "success", "b", false },
            { 0, 0, 0 }
        };
        static ::DBus::IntrospectedArgument Remove_args[] = 
        {
            { "files", "as", true },
            { "removeParent", "b", true },
            { "messages", "as", false },
            { "success", "b", false },
            { 0, 0, 0 }
        };
        static ::DBus::IntrospectedArgument Clear_args[] = 
        {
            { 0, 0, 0 }
        };
        static ::DBus::IntrospectedArgument ListFilesToStream_args[] = 
        {
            { "directory", "s", true },
            { "absolute", "b", true },
            { "stream", "s", true },
            { "messages", "as", false },
            { "success", "b", false },
            { 0, 0, 0 }
        };
        static ::DBus::IntrospectedArgument DirectoryListing_args[] = 
        {
            { "directory", "s", true },
            { "files", "as", false },
            { "messages", "as", false },
            { "success", "b", false },
            { 0, 0, 0 }
        };
        static ::DBus::IntrospectedArgument LowestCommonAncestor_args[] = 
        {
            { "path", "s", false },
            { 0, 0, 0 }
        };
        static ::DBus::IntrospectedArgument Stash_args[] = 
        {
            { "messages", "as", false },
            { "success", "b", false },
            { 0, 0, 0 }
        };
        static ::DBus::IntrospectedArgument Unstash_args[] = 
        {
            { "n", "u", true },
            { "messages", "as", false },
            { "success", "b", false },
            { 0, 0, 0 }
        };
        static ::DBus::IntrospectedArgument ListStash_args[] = 
        {
            { "stash", "as", false },
            { 0, 0, 0 }
        };
        static ::DBus::IntrospectedArgument DropStash_args[] = 
        {
            { "n", "u", true },
            { "messages", "as", false },
            { "success", "b", false },
            { 0, 0, 0 }
        };
        static ::DBus::IntrospectedArgument ClearStash_args[] = 
        {
            { 0, 0, 0 }
        };
        static ::DBus::IntrospectedMethod Fclip_adaptor_methods[] = 
        {
            { "Add", Add_args },
            { "Remove", Remove_args },
            { "Clear", Clear_args },
            { "ListFilesToStream", ListFilesToStream_args },
            { "DirectoryListing", DirectoryListing_args },
            { "LowestCommonAncestor", LowestCommonAncestor_args },
            { "Stash", Stash_args },
            { "Unstash", Unstash_args },
            { "ListStash", ListStash_args },
            { "DropStash", DropStash_args },
            { "ClearStash", ClearStash_args },
            { 0, 0 }
        };
        static ::DBus::IntrospectedMethod Fclip_adaptor_signals[] = 
        {
            { 0, 0 }
        };
        static ::DBus::IntrospectedProperty Fclip_adaptor_properties[] = 
        {
            { 0, 0, 0, 0 }
        };
        static ::DBus::IntrospectedInterface Fclip_adaptor_interface = 
        {
            "com.fclip.Fclip",
            Fclip_adaptor_methods,
            Fclip_adaptor_signals,
            Fclip_adaptor_properties
        };
        return &Fclip_adaptor_interface;
    }

public:

    /* properties exposed by this interface, use
     * property() and property(value) to get and set a particular property
     */

public:

    /* methods exported by this interface,
     * you will have to implement them in your ObjectAdaptor
     */
    virtual void Add(const std::vector< std::string >& files, const bool& recursive, std::vector< std::string >& messages, bool& success) = 0;
    virtual void Remove(const std::vector< std::string >& files, const bool& removeParent, std::vector< std::string >& messages, bool& success) = 0;
    virtual void Clear() = 0;
    virtual void ListFilesToStream(const std::string& directory, const bool& absolute, const std::string& stream, std::vector< std::string >& messages, bool& success) = 0;
    virtual void DirectoryListing(const std::string& directory, std::vector< std::string >& files, std::vector< std::string >& messages, bool& success) = 0;
    virtual std::string LowestCommonAncestor() = 0;
    virtual void Stash(std::vector< std::string >& messages, bool& success) = 0;
    virtual void Unstash(const uint32_t& n, std::vector< std::string >& messages, bool& success) = 0;
    virtual std::vector< std::string > ListStash() = 0;
    virtual void DropStash(const uint32_t& n, std::vector< std::string >& messages, bool& success) = 0;
    virtual void ClearStash() = 0;

public:

    /* signal emitters for this interface
     */

private:

    /* unmarshalers (to unpack the DBus message before calling the actual interface method)
     */
    ::DBus::Message _Add_stub(const ::DBus::CallMessage &call)
    {
        ::DBus::MessageIter ri = call.reader();

        std::vector< std::string > argin1; ri >> argin1;
        bool argin2; ri >> argin2;
        std::vector< std::string > argout1;
        bool argout2;
        Add(argin1, argin2, argout1, argout2);
        ::DBus::ReturnMessage reply(call);
        ::DBus::MessageIter wi = reply.writer();
        wi << argout1;
        wi << argout2;
        return reply;
    }
    ::DBus::Message _Remove_stub(const ::DBus::CallMessage &call)
    {
        ::DBus::MessageIter ri = call.reader();

        std::vector< std::string > argin1; ri >> argin1;
        bool argin2; ri >> argin2;
        std::vector< std::string > argout1;
        bool argout2;
        Remove(argin1, argin2, argout1, argout2);
        ::DBus::ReturnMessage reply(call);
        ::DBus::MessageIter wi = reply.writer();
        wi << argout1;
        wi << argout2;
        return reply;
    }
    ::DBus::Message _Clear_stub(const ::DBus::CallMessage &call)
    {
        ::DBus::MessageIter ri = call.reader();

        Clear();
        ::DBus::ReturnMessage reply(call);
        return reply;
    }
    ::DBus::Message _ListFilesToStream_stub(const ::DBus::CallMessage &call)
    {
        ::DBus::MessageIter ri = call.reader();

        std::string argin1; ri >> argin1;
        bool argin2; ri >> argin2;
        std::string argin3; ri >> argin3;
        std::vector< std::string > argout1;
        bool argout2;
        ListFilesToStream(argin1, argin2, argin3, argout1, argout2);
        ::DBus::ReturnMessage reply(call);
        ::DBus::MessageIter wi = reply.writer();
        wi << argout1;
        wi << argout2;
        return reply;
    }
    ::DBus::Message _DirectoryListing_stub(const ::DBus::CallMessage &call)
    {
        ::DBus::MessageIter ri = call.reader();

        std::string argin1; ri >> argin1;
        std::vector< std::string > argout1;
        std::vector< std::string > argout2;
        bool argout3;
        DirectoryListing(argin1, argout1, argout2, argout3);
        ::DBus::ReturnMessage reply(call);
        ::DBus::MessageIter wi = reply.writer();
        wi << argout1;
        wi << argout2;
        wi << argout3;
        return reply;
    }
    ::DBus::Message _LowestCommonAncestor_stub(const ::DBus::CallMessage &call)
    {
        ::DBus::MessageIter ri = call.reader();

        std::string argout1 = LowestCommonAncestor();
        ::DBus::ReturnMessage reply(call);
        ::DBus::MessageIter wi = reply.writer();
        wi << argout1;
        return reply;
    }
    ::DBus::Message _Stash_stub(const ::DBus::CallMessage &call)
    {
        ::DBus::MessageIter ri = call.reader();

        std::vector< std::string > argout1;
        bool argout2;
        Stash(argout1, argout2);
        ::DBus::ReturnMessage reply(call);
        ::DBus::MessageIter wi = reply.writer();
        wi << argout1;
        wi << argout2;
        return reply;
    }
    ::DBus::Message _Unstash_stub(const ::DBus::CallMessage &call)
    {
        ::DBus::MessageIter ri = call.reader();

        uint32_t argin1; ri >> argin1;
        std::vector< std::string > argout1;
        bool argout2;
        Unstash(argin1, argout1, argout2);
        ::DBus::ReturnMessage reply(call);
        ::DBus::MessageIter wi = reply.writer();
        wi << argout1;
        wi << argout2;
        return reply;
    }
    ::DBus::Message _ListStash_stub(const ::DBus::CallMessage &call)
    {
        ::DBus::MessageIter ri = call.reader();

        std::vector< std::string > argout1 = ListStash();
        ::DBus::ReturnMessage reply(call);
        ::DBus::MessageIter wi = reply.writer();
        wi << argout1;
        return reply;
    }
    ::DBus::Message _DropStash_stub(const ::DBus::CallMessage &call)
    {
        ::DBus::MessageIter ri = call.reader();

        uint32_t argin1; ri >> argin1;
        std::vector< std::string > argout1;
        bool argout2;
        DropStash(argin1, argout1, argout2);
        ::DBus::ReturnMessage reply(call);
        ::DBus::MessageIter wi = reply.writer();
        wi << argout1;
        wi << argout2;
        return reply;
    }
    ::DBus::Message _ClearStash_stub(const ::DBus::CallMessage &call)
    {
        ::DBus::MessageIter ri = call.reader();

        ClearStash();
        ::DBus::ReturnMessage reply(call);
        return reply;
    }
};

} } 
#endif //__dbusxx__Fclip_adaptor_h__ADAPTOR_MARSHAL_H
