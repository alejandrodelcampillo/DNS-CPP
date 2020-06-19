/**
 *  Records.h
 *
 *  Wrapper around the response that only gives the requested records.
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
#include "request.h"
#include "response.h"
#include "answer.h"
#include <functional>

/**
 *  Begin of namespace
 */
namespace DNS {

/**
 *  Class definition
 */
template <typename T>
class Records
{
private:
    /**
     *  Pointer to the request
     *  @var Request *
     */
    const Request *_request;

    /**
     *  Reference to the resposne
     *  @var const Response&
     */
    const Response &_response;

public:
    /**
     *  The constructor
     *  @param  request
     *  @param  response
     */
    Records(const Request *request, const Response &response) : _request(request), _response(response) {}
    
    /**
     *  Virtual destructor
     */
    virtual ~Records() = default;
    
    /**
     *  Extract all records
     *  @param  callback    function to call for each record
     *  @return size_t      number of records
     */
    size_t extract(const std::function<void(const T &)> &callback) const
    {
        // number of records encountered
        size_t records = 0;

        // read out all the answers
        for (size_t i = 0; i < _response.answers(); ++i)
        {
            // get an answer (DNS::Answer is a derived class from DNS::Record
            // that is optimized for records stored in the Answer section)
            DNS::Answer record(_response, i);

            // if the answer type does not match the requested type, continue
            // @todo maybe there are cases in which this is not valid, CNAME?
            if (record.type() != _request->type()) continue;

            // parse the record
            T t(_response, record);

            // call the callback
            callback(t);

            // one more call done
            ++records;
        }

        // return number of times callback was called
        return records;
    }
};
    
/**
 *  End of namespace
 */
}

