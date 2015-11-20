#include "GPU_TestCard.h"
#include <QMutexLocker>


namespace pelican {

namespace ampp {


/**
 *@details GPU_TestCard 
 */
GPU_TestCard::GPU_TestCard()
    : _current(0), _doThrow(false)
{
}

/**
 *@details
 */
GPU_TestCard::~GPU_TestCard()
{
    completeJob();
}

void GPU_TestCard::run( GPU_Job* job ) {
    QMutexLocker loc(&_mutex);
   _current = job;
   _waitCondition.wait(&_mutex);
   if( _doThrow ) {
       _object();
   }
}

void GPU_TestCard::completeJob() {
    _mutex.lock();
    if( _current ) {
       _current = 0;
       _mutex.unlock();
       _waitCondition.wakeAll();
    }
    else {
        _mutex.unlock();
    }
}

GPU_Job* GPU_TestCard::currentJob() const {
    return _current;
}

} // namespace ampp
} // namespace pelican
