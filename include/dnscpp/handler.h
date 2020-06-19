/**
 *  Handler.h
 * 
 *  In user space programs you should implement the handler interface
 *  that receives the responses to your queries.
 * 
 *  @author Emiel Bruijntjes <emiel.bruijntjes@copernica.com>
 *  @copyright 2020 Copernica BV
 */
 
/**
 *  Include guard
 */
#pragma once

/**
 *  Includes
 */
#include "records.h"
#include "error.h"

/**
 *  Begin of namespace
 */
namespace DNS {

/**
 *  Forward declarations
 */
class Response;
class A;
class AAAA;
class MX;
class CNAME;
class Request;

/**
 *  Class definition
 */
class Handler
{
public:
    /**
     *  Method that is called when an operation has failed
     *  This normally happens when the nameserver could not be reached, or 
     *  when it sent back a response that could not be parsed.
     * 
     *  @param  request         the request that failed without an error
     *  @param  error           the error type that occurred
     */
    virtual void onFailure(const Request *request, DNS::Error error) {}

    /**
     *  Method that is called when a raw response is received
     *  The default implementation of this method reads out the properties
     *  of the response, and passes it on to one of the other methods.
     *  However, if you want to read out the original response yourself,
     *  you can override this method.
     * 
     *  @param  request         the request 
     *  @param  response        the received response
     */
    virtual void onReceived(const Request *request, const Response &response);

    /**
     *  When you made a call for specific records, you can implement
     *  one or more of the following methods to get exactly the information
     *  that you were looking for.
     *
     *  @param  request         the request done to resolve the records (contains hostname)
     *  @param  records         the received records
     */
    virtual void onSuccess(const Request *request, const Records<A> &records) {}
    virtual void onSuccess(const Request *request, const Records<AAAA> &records) {}
    virtual void onSuccess(const Request *request, const Records<MX> &records) {}
    virtual void onSuccess(const Request *request, const Records<CNAME> &records) {}
};

/**
 *  End of namespace
 */
}

