#ifndef DEDISPERSIONPIPELINE_H
#define DEDISPERSIONPIPELINE_H

#include <QtCore/QList>
#include "pelican/core/AbstractPipeline.h"
#include "pelican/utility/LockingCircularBuffer.hpp"
#include "LockingPtrContainer.hpp"
#include "PPFChanneliser.h"
#include "StokesGenerator.h"
#include "RFI_Clipper.h"
#include "StokesIntegrator.h"
#include "AdapterTimeSeriesDataSet.h"
#include "TimeSeriesDataSet.h"
#include "SpectrumDataSet.h"
#include "SigprocStokesWriter.h"
#include "TriggerOutput.h"
#include "DedispersionModule.h"
#include "DedispersionSpectra.h"
#include "DedispersionAnalyser.h"
#include "DedispersionDataAnalysisOutput.h"
#include "timer.h"


/**
 * @file DedispersionPipeline.h
 */

namespace pelican {

namespace ampp {

/**
 * @class DedispersionPipeline
 *  
 * @brief
 *     A dedispersion pipeline for streaming TimeSeries bemaformed Data
 * @details
 * 
 */

class DedispersionPipeline : public AbstractPipeline
{
    public:
        DedispersionPipeline( const QString& streamIdentifier );
        ~DedispersionPipeline();

        /// Initialises the pipeline.
        void init();

        /// Runs the pipeline.
        void run(QHash<QString, DataBlob*>& remoteData);

        /// called internally to free up DataBlobs after they are finished with
        void updateBufferLock( const QList<DataBlob*>& );

    protected:
        void dedispersionAnalysis( DataBlob* data );

    private:
        QString _streamIdentifier;

        /// Module pointers
        PPFChanneliser* _ppfChanneliser;
        StokesGenerator* _stokesGenerator;
        StokesIntegrator* _stokesIntegrator;
        RFI_Clipper* _rfiClipper;
        DedispersionModule* _dedispersionModule;
        DedispersionAnalyser* _dedispersionAnalyser;

        /// Local data blobs
	SpectrumDataSetC32* _spectra;
        QList<SpectrumDataSetC32*> _spectraBuffer;
        TimeSeriesDataSetC32* timeSeries;
        // Uncomment the following line for raw data buffer
	//QList<SpectrumDataSetC32*> _spectra;
        QList<SpectrumDataSetStokes*> _stokesData;
        SpectrumDataSetStokes* _intStokes;
        LockingPtrContainer<SpectrumDataSetStokes>* _stokesBuffer;
        LockingPtrContainer<SpectrumDataSetC32>* _rawBuffer;
        WeightedSpectrumDataSet* _weightedIntStokes;

#ifdef TIMING_ENABLED
        // Timers.
        TimerData _ppfTime;
        TimerData _stokesTime;
        TimerData _integratorTime;
        TimerData _dedispersionTime;
        TimerData _totalTime;
        TimerData _rfiClipperTime;
#endif

        unsigned _iteration;
	unsigned int _minEventsFound;
	unsigned int _maxEventsFound;
};

} // namespace ampp
} // namespace pelican
#endif // DEDISPERSIONPIPELINE_H 
