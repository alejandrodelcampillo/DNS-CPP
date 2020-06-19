/**
 *  Context.cpp
 * 
 *  Implementation file for the Context class
 * 
 *  @author Emiel Bruijntjes <emiel.bruijntjes@copernica.com>
 *  @copyright 2020 Copernica BV
 */

/**
 *  Dependencies
 */
#include "../include/dnscpp/context.h"
#include "extendedrequest.h"

/**
 *  Begin of namespace
 */
namespace DNS {

/**
 *  Do a dns lookup
 *  @param  name        the record name to look for
 *  @param  type        type of record (normally you ask for an 'a' record)
 *  @param  handler     object that will be notified when the query is ready
 */
Request *Context::query(const char *domain, ns_type type, Handler *handler)
{
    // we are going to create a self-destructing request
    return new ExtendedRequest(this, domain, type, handler);
}

/**
 *  End of namespace
 */
}

