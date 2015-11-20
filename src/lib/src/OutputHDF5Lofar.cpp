#include "OutputHDF5Lofar.h"
#include "LofarData.h"
#include "LofarStationConfiguration.h"
#include "pelican/data/DataBlob.h"
#include "pelican/utility/ConfigNode.h"


namespace pelican {

namespace ampp {


/**
 *@details OutputHDF5Lofar
 */
OutputHDF5Lofar::OutputHDF5Lofar(  const ConfigNode& configNode )
    : AbstractOutputStream( configNode )
{
}

/**
 *@details
 */
OutputHDF5Lofar::~OutputHDF5Lofar()
{
}

void OutputHDF5Lofar::sendStream(const QString& /*streamName*/, const DataBlob* dataBlob)
{
    if (dataBlob->type() == "LofarData")
    {
        //const LofarStationConfiguration& config = static_cast<const LofarData*>(dataBlob)->configuration();
        // TODO
    }
}

} // namespace ampp
} // namespace pelican
