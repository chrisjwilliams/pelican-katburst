#include "DedispersionDataAnalysis.h"
#include <QPair>


namespace pelican {

namespace ampp {


/**
 *@details DedispersionDataAnalysis 
 */
DedispersionDataAnalysis::DedispersionDataAnalysis()
    : DataBlob("DedispersionDataAnalysis"), _data(0)
{
}

/**
 *@details
 */
DedispersionDataAnalysis::~DedispersionDataAnalysis() {
}

void DedispersionDataAnalysis::reset( const DedispersionSpectra* data ) {
    _data = data;
    _eventIndex.clear();
}

int DedispersionDataAnalysis::eventsFound() const {
    return _eventIndex.size();
}

const QList<DedispersionEvent>& DedispersionDataAnalysis::events() const {
    return _eventIndex;
}

  void DedispersionDataAnalysis::addEvent( unsigned dmIndex, unsigned timeIndex, float mfBinFactor, float mfBinValue ) {
    _eventIndex.append( DedispersionEvent(dmIndex, timeIndex, _data, mfBinFactor, mfBinValue ) ); //mf=matched filtering
}

} // namespace ampp
} // namespace pelican
