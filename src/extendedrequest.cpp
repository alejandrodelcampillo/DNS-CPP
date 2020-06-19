/**
 *  ExtendedRequest.cpp
 *  
 *  Implementation file for the Requet class
 * 
 *  @author Emiel Bruijntjes <emiel.bruijntjes@copernica.com>
 *  @copyright 2020 Copernica BV
 */

/**
 *  Dependencies
 */
#include "extendedrequest.h"
#include "connection.h"
#include <iostream>
#include "../include/dnscpp/core.h"
#include "../include/dnscpp/response.h"
#include "../include/dnscpp/answer.h"
#include "../include/dnscpp/handler.h"

/**
 *  Begin of namespace
 */
namespace DNS {

/**
 *  Constructor
 *  @param  core        dns core object
 *  @param  domain      the domain of the lookup
 *  @param  type        the type of the request
 *  @param  handler     user space object
 */
ExtendedRequest::ExtendedRequest(Core *core, const char *domain, ns_type type, DNS::Handler *handler) : 
    Request(domain, type, handler),
    _core(core), 
    _query(ns_o_query, domain, type)
{
    // iterate over the nameservers because we will send a datagram to each one of them
    for (auto &nameserver : core->nameservers())
    {
        // send a datagram, and register ourselves as subscriber
        nameserver.datagram(_query);
        
        // we want to be notified when a response comes in
        nameserver.subscribe(this);
    }
    
    // we set a timer to repeat the call
    _timer = core->loop()->timer(_core->interval(), this);
}

/**
 *  Destructor
 */
ExtendedRequest::~ExtendedRequest()
{
    std::cout << "destructed " << this << std::endl;

    // unsubscribe from the nameservers
    for (auto &nameserver : _core->nameservers()) nameserver.unsubscribe(this);
    
    // stop the timer
    if (_timer) _core->loop()->cancel(_timer, this);
}

/**
 *  When does the ExtendedRequest expire?
 *  @return double
 */
double ExtendedRequest::expires() const
{
    // get the max time
    return _started + _core->expire();
}

/** 
 *  Time out the ExtendedRequest because no appropriate response was received in time
 */
void ExtendedRequest::timeout()
{
    // if we were not cancelled yet, report an error
    if (_handler) _handler->onFailure(this, DNS::Error::network);
    
    // self-destruct
    delete this;
}

/**
 *  Retry / send a new message to the nameservers
 *  @param  now     current timestamp
 */
void ExtendedRequest::retry(double now)
{
    // send a datagram to each nameserver
    for (auto &nameserver : _core->nameservers())
    {
        // send a datagram, and register ourselves as subscriber
        nameserver.datagram(_query);
    }
    
    // when is the next attempt?
    double next = std::min(now + _core->interval(), expires());

    // we set a new timer for when the entire ExtendedRequest times out
    _timer = _core->loop()->timer(next - now, this);
}

/**
 *  When the timer expires
 *  This method is called from the event loop in user space
 */
void ExtendedRequest::expire()
{
    // find the current time
    Now now;
    
    // the timer has expired
    _timer = nullptr;
    
    // did the entire ExtendedRequest expire?
    if (now >= expires()) return timeout();
    
    // if we do not yet have a tcp connection we send out more dgrams
    if (!_connection) return retry(now);
    
    // we set a new timer for when the entire ExtendedRequest times out
    _timer = _core->loop()->timer(expires() - now, this);
}
    
/**
 *  Method that can be called to cancel the request.
 */
void ExtendedRequest::cancel()
{
    // forget the handler
    _handler = nullptr;

    // @todo close connection and unsubscribe early
}

/**
 *  Method that is called when a response is received
 *  @param  nameserver  the reporting nameserver
 *  @param  response    the received response
 */
void ExtendedRequest::onReceived(Nameserver *nameserver, const Response &response)
{
    // ignore responses that do not match with the query
    // @todo should we check for more? like whether the response is indeed a response
    if (!_query.matches(response)) return;
    
    // if we're already busy with a tcp connection we ignore further dgram responses
    if (_connection) return;
    
    // if the response was truncated, we ignore it and start a tcp connection
    if (response.truncated()) return _connection.reset(new Connection(_core->loop(), nameserver->ip(), _query, this));
    
    std::cout << "onReceived 1 handler call" << this << std::endl;

    // we have a response, so we can pass that to user space
    if (_handler) _handler->onReceived(this, response);
    
    std::cout << "onReceived 1 " << this << std::endl;

    // we can self-destruct -- this request has been handled
    delete this;
}

/**
 *  Called when the response has been received
 *  @param  connection
 *  @param  response
 */
void ExtendedRequest::onReceived(Connection *connection, const Response &response)
{
    // ignore responses that do not match with the query
    // @todo should we check for more? like whether the response is indeed a response
    if (!_query.matches(response)) return;
    
    // we have a response, hand it over to user space if this request has not been cancelled
    if (_handler) _handler->onReceived(this, response);
    
    std::cout << "onReceived 2 " << this << std::endl;

    // self-destruct now that the request has been completed
    delete this;
}

/**
 *  Called when the connection could not be used
 *  @param  connection
 */
void ExtendedRequest::onFailure(Connection *connection)
{
    // call failure on the handler, if this object has not been cancelled
    if (_handler) _handler->onFailure(this, DNS::Error::network);

    // self-destruct now that the request has permanently failed
    delete this;
}


    
/**
 *  End of namespace
 */
}

