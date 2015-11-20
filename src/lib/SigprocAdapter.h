#ifndef SIGPROC_ADAPTER_H_
#define SIGPROC_ADAPTER_H_

#include "pelican/core/AbstractStreamAdapter.h"
#include "SpectrumDataSet.h"
#include "file_handler.h" // FIXME what is this header?! (where from?)
#include <complex>

namespace pelican {
namespace ampp {

class SigprocAdapter: public AbstractStreamAdapter
{
    public:
        /// Constructs a new SigprocAdapter.
        SigprocAdapter(const ConfigNode& config);

        /// Destroys the SigprocAdapter.
        ~SigprocAdapter() {}

    protected:
        /// Method to deserialise a sigproc file
        void deserialise(QIODevice* in);

    private:
        /// Updates and checks the size of the time stream data.
        void _checkData();

    private:
        SpectrumDataSetStokes* _stokesData;
        FILE_HEADER* _header;
        FILE *_fp;

        unsigned _nSamples;
        unsigned _nChannels;
        unsigned _nPolarisations;
        unsigned _nBits;
        double   _tSamp;
        double   _tStart;
        unsigned long int _iteration;
};

PELICAN_DECLARE_ADAPTER(SigprocAdapter)

} // ampp
} // namespace

#endif // SIGPROC_ADAPTER_H_
