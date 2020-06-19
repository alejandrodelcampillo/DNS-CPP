/**
 *  Request.h
 * 
 *  Base class for the request
 * 
 *  @author Michael van der Werve <michael.vanderwerve@mailerq.com>
 *  @copyright 2020 Copernica BV
 */

/**
 *  Include guard
 */
#pragma once

/**
 *  Dependencies
 */
#include <string>
#include <arpa/nameser.h>

/**
 *  Begin of namespace
 */
namespace DNS {

/**
 *  Forward declarations
 */
class Handler;

/**
 *  Class definition
 */
class Request
{
protected:    
    /**
     *  Domain that is being resolved
     *  @var std::string
     */
    std::string _domain;

    /**
     *  Type of the records
     *  @var ns_type
     */
    ns_type _type;

    /**
     *  User space object that handles this request
     *  @var Handler
     */
    DNS::Handler *_handler;

    /**
     *  Private destructor, the class is self-destructing
     */
    virtual ~Request() = default;

public:
    /**
     *  Constructor
     *  @param  domain      the domain of the lookup
     *  @param  type        type of records to look for
     *  @param  handler     user space object interested in the result
     */
    Request(const char *domain, ns_type type, DNS::Handler *handler) :
        _domain(domain), _type(type), _handler(handler) {}
    
    /**
     *  No copying
     *  @param  that
     */
    Request(const Request &request) = delete;

    /**
     *  Get access to the domain
     *  @return const char *
     */
    const char *domain() const { return _domain.c_str(); }

    /**
     *  The type of records that is being requested
     *  @return ns_type
     */
    ns_type type() const { return _type; }

    /**
     *  Handler for the request
     *  @return DNS::Handler *
     */
    DNS::Handler *handler() const { return _handler; }

    /**
     *  Method that can be called to cancel the request.
     */
    virtual void cancel() = 0;
};

/**
 *  End of namespace
 */
}
