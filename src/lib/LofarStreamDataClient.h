#ifndef LOFARSTREAMDATACLIENT_H
#define LOFARSTREAMDATACLIENT_H

#include "pelican/core/DirectStreamDataClient.h"

/**
 * @file LofarStreamDataClient.h
 */

namespace pelican {
namespace ampp {

/**
 * @class LofarStreamDataClient
 *
 * @ingroup pelican_lofar
 *
 * @brief
 *    Lofar data client to connect directly to the LOFAR station
 *    output stream.
 *
 * @details
 *
 */

class LofarStreamDataClient : public DirectStreamDataClient
{
    public:
        LofarStreamDataClient(const ConfigNode& configNode,
                const DataTypes& types, const Config* config);
        ~LofarStreamDataClient();

    private:
};

PELICAN_DECLARE_CLIENT(LofarStreamDataClient)

} // namespace ampp
} // namespace pelican

#endif // LOFARSTREAMDATACLIENT_H
