#ifndef DEDISPERSIONSPECTRA_H
#define DEDISPERSIONSPECTRA_H


#include "pelican/data/DataBlob.h"
#include "BinMap.h"
#include <vector>
#include <QList>

/**
 * @file DedispersionSpectra.h
 */

namespace pelican {

namespace ampp {
class SpectrumDataSetStokes;

/**
 * @class DedispersionSpectra
 *  
 * @brief
 *     Stokes x DM x time
 * @details
 *    The dm spectra is a function of DM value vs. the total integral 
 *    of the power output at that dm.
 *    This class reprepesnts a collection of these spectra, one for each time slice
 */

class DedispersionSpectra : public DataBlob
{
    public:
        DedispersionSpectra();
        void resize( unsigned timebins, unsigned dedispersionBins, 
                     float dedispersionBinStart, float dedispersionBinWidth );
        ~DedispersionSpectra();

        /// return the Dedispersion (dm vs. integrated power from freq-time data)
        //const DedispersionData<T>& spectra( unsigned timeBin ) const;

        /// return the entire data chunk as a contiguous block
        //  represented as a series of vectors for each time stamp. The vector 
        //  represents the summed power for
        //  each dedispersion value.
        inline std::vector<float>& data() { return _data; }

        /// the inegrated power for the specified dm and timeslice
        /// range of timeslice is 0->(timeSamples()-1)
        float dmAmplitude( unsigned timeSlice, float dm ) const;
        float dmAmplitude( unsigned timeSlice, int dm ) const;

        /// return the index of the bin for a given dm value
        int dmIndex( float dm ) const;
        float dm( unsigned dm ) const;

        /// return a list of pointers to the objects representing input data
        const QList< SpectrumDataSetStokes* >& inputDataBlobs() const {
                return _inputBlobs;
        }
        void setInputDataBlobs( const QList< SpectrumDataSetStokes* >& );
        void setFirstSample( unsigned int sampleNumber );

        /// return the start of the maximum DM that can be represented in the data
        float dmMax() const { return _dmBin.lastBinValue(); }

        /// return the number of dm bins
        int dmBins() const { return _dmBin.numberBins(); }
        inline int timeSamples() const { return _timeBins; }

        double getTime( unsigned int sampleNumber ) const;
        /// Return the rms of the data if it has been set;
        float getRMS() const { return _rms; }

        /// Set the rms of the data;
        void setRMS(float rms) { _rms = rms; }

        /// Set flag for lost data;

        void setLost(unsigned int lost) { _lost = lost; }
        unsigned int getLost() const { return _lost; }
    private:
        BinMap _dmBin;
        unsigned _timeBins;
        unsigned _dedispersionBins;
        unsigned _firstSampleNumber;
        float _rms;
        unsigned int _lost;
        std::vector<float> _data;
        QList<SpectrumDataSetStokes* > _inputBlobs;
};
PELICAN_DECLARE_DATABLOB( DedispersionSpectra )

} // namespace ampp
} // namespace pelican
#endif // DEDISPERSIONSPECTRA_H
