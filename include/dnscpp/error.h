/**
 *  Error.h
 * 
 *  Enumeration of the error types.
 * 
 *  @author Michael van der Werve <michael.vanderwerve@mailerq.com>
 *  @copyright 2020 Copernica BV
 */

/**
 *  Include guard
 */
#pragma once

/**
 *  Includes
 */
#include <cstdint>

/**
 *  Begin of namespace
 */
namespace DNS {

/**
 *  This is an enumeration type
 */
enum class Error : uint8_t
{
    network,      // connection failure, expired, unavailable
    temporary,    // temporary error
    nxdomain,     // domain does not exist
    malformed,    // received malformed example
    nodata,       // no records, which is also a failure (empty result)
    other
};

// @todo change these error types
    
/**
 *  End of namespace
 */
}

