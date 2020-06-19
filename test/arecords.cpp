/**
 *  A.cpp
 * 
 *  Simple test program to retrieve an A record
 * 
 *  @author Emiel Bruijntjes <emiel.bruijntjes@copernica.com>
 *  @copyright 2020 Copernica BV
 */

/**
 *  Dependencies
 */
#include <ev.h>
#include <dnscpp.h>
#include <dnscpp/libev.h>
#include <iostream>


#include "../src/name.h"
#include "../src/input.h"

/**
 *  Handler that waits for the response on 
 */
class Receiver : public DNS::Handler
{
private:
    /**
     *  Pointer to the original context
     *  @var DNS::Context
     */
    DNS::Context *_context;

public:
    /**
     *  Constructor
     *  @param  context
     */
    Receiver(DNS::Context *context) : _context(context) {}

    /**
     *  Method that is called when a records are received
     *  @param  response        the received response
     */
    virtual void onSuccess(const DNS::Request *request, const DNS::Records<DNS::A> &records) override
    {
        // report about the A record
        size_t num = records.extract([](const DNS::A &a) {
            std::cout << "- a record " << a.name() << " " << a.ttl() << " " << a.ip() << std::endl;
        });

        std::cout << "total a records for " << request->domain() << ": " << num << std::endl;
    }

    /**
     *  Method that is called when aaaa records are received
     *  @param  response        the received response
     */
    virtual void onSuccess(const DNS::Request *request, const DNS::Records<DNS::AAAA> &records) override
    {
        // report about the A record
        size_t num = records.extract([](const DNS::AAAA &a) {
            std::cout << "- aaaa record " << a.name() << " " << a.ttl() << " " << a.ip() << std::endl;
        });

        std::cout << "total aaaa records for " << request->domain() << ": " << num << std::endl;
    }

    /**
     *  Method that is called when mx records are received
     *  @param  response        the received response
     */
    virtual void onSuccess(const DNS::Request *request, const DNS::Records<DNS::MX> &records) override
    {
        // report about the MX record
        size_t num = records.extract([](const DNS::MX &a) {
            std::cout << "- mx record " << a.name() << " " << a.ttl() << " " << a.priority() << " " << a.hostname() << std::endl;
        });

        std::cout << "total mx records for " << request->domain() << ": " << num << std::endl;
    }

    /**
     *  Method that is called on failure
     *  @param  request
     *  @param  error
     */
    virtual void onFailure(const DNS::Request *request, DNS::Error error)
    {
        std::cout << "REQUEST TO " << request->domain() << " failed with error " << (int)error << std::endl;
    }
};

/**
 *  Main procedure
 *  @return int
 */
int main()
{
    // the event loop
    struct ev_loop *loop = EV_DEFAULT;
    
    // wrap the loop to make it accessible by dns-cpp
    DNS::LibEv myloop(loop);
    
    // create a dns context
    DNS::Context context(&myloop);

    // we need a handler
    Receiver handler(&context);

    // add a nameserver
    context.nameserver(DNS::Ip("8.8.8.8"));
    
    // do a query
    auto request1 = context.query("www.google.com", ns_t_a, &handler);
    //auto request2 = context.query("www.google.com", ns_t_aaaa, &handler);
    //auto request3 = context.query("www.google.com", ns_t_mx, &handler);
    auto request4 = context.query("gmail.com", ns_t_mx, &handler);

    // run the event loop
    ev_run(loop, 0);
    
    // done
    return 0;
}
