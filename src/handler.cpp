/**
 *  Handler.cpp
 * 
 *  Implementation file for the handler
 * 
 *  @author Emiel Bruijntjes <emiel.bruijntjes@copernica.com>
 *  @copyright 2020 Copernica BV
 */

/**
 *  Dependencies
 */
#include "../include/dnscpp/handler.h"
#include "../include/dnscpp/error.h"
#include "../include/dnscpp/response.h"
#include "../include/dnscpp/answer.h"
#include "../include/dnscpp/a.h"
#include "../include/dnscpp/aaaa.h"
#include "../include/dnscpp/mx.h"
#include "../include/dnscpp/request.h"
#include "../include/dnscpp/records.h"
#include <arpa/nameser.h>
#include <iostream>

/**
 *  Begin of namespace
 */
namespace DNS {

/**
 *  Function to convert Rcode to DNS::Error
 *  @param  code
 */
static Error error(ns_rcode code) 
{
    // switch over the code
    // @todo convert this to errors that make sense
    switch (code)
    {
    case ns_r_formerr: return Error::malformed;
    case ns_r_servfail: return Error::temporary;
    case ns_r_nxdomain: return Error::nxdomain;
    case ns_r_notimpl: return Error::temporary;
    case ns_r_refused: return Error::temporary;
    default: return Error::other;
    }
}

/**
 *  Method that is called when a raw response is received
 *  @param  response        the received response
 */
void Handler::onReceived(const Request *request, const Response &response)
{
    // if the response is not equal to ns_r_noerror (success), we failed (call the handler)
    if (response.rcode() != ns_r_noerror) return onFailure(request, error(response.rcode()));

    std::cout << "onReceived for " << request << std::endl;

    // create an iterable object for the records
    switch (request->type())
    {
    case ns_t_a:
        // call success
        onSuccess(request, Records<A>(request, response));
    case ns_t_aaaa:
        // call success
        onSuccess(request, Records<AAAA>(request, response));
    case ns_t_mx:
        // call success
        onSuccess(request, Records<MX>(request, response));
    default:
        // @todo handle unknown record types
        return;
    }
}
    
/**
 *  End of namespace
 */
}

