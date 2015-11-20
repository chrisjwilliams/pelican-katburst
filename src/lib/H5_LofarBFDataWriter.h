#ifndef H5_LOFARBFDATAWRITER_H
#define H5_LOFARBFDATAWRITER_H

#ifdef LOFAR_DAL_FOUND

/**
 * @file H5_LofarBFDataWriter.h
 */

#include "pelican/output/AbstractOutputStream.h"
#include "pelican/utility/ConfigNode.h"
#include "pelican/data/DataBlob.h"

#include <QtCore/QDataStream>
#include <QtCore/QFile>
#include <QtCore/QVector>
#include <QtCore/QList>
#include <fstream>
#include "FileWriter.h"
#include "timer.h"

namespace dal {
  class BF_File;
}
namespace pelican {
namespace ampp {

class SpectrumDataSetBase;

/**
 * @class H5_LofarBFDataWriter
 *
 * @brief
 *    OutputStreamer that genererates Lofar Beamformed Format
 *    H5 files
 * @details
 *    common configuration elements for inheriting classes
 *        <file filepath="dir/to/save/output/files" label="">
 *             filepath=directory in which to store files
 *             label=additional string to add to file name
 *        </file>
 *        <observation id="MyArtemisObservation" />
 *        <LBA_0_or_HBA_1 value="1">
 *            Define which array this refers to
 *        </LBA_0_or_HBA_1>
 *        <clock value="200">
 *            could be 160
 *        </clock>
 *        <fch1 value="150" />
 *        <topSubbandIndex value="150" >
 *            Used to calculate the frequency of the first channel
 *            fch1 - only used if fch1 is not set
 *        </topSubbandIndex>
 */

class H5_LofarBFDataWriter : public AbstractOutputStream
{
    protected:
        // lifted from lofar RTCP/Interface/include/Interface/Parset.h
        enum StokesType { STOKES_I = 0, STOKES_IQUV, STOKES_XXYY, INVALID_STOKES = -1 };

    public:
        H5_LofarBFDataWriter( const ConfigNode& config );
        ~H5_LofarBFDataWriter();

        /// return the file path
        QString filepath() { return _filePath; }

        /// the name of the currently opened raw data file
        const QString rawFilename( int polarisation ) const { 
                if( polarisation > (int)_nPols ) return "";
                return _rawFilename[polarisation]; 
        }
        const QString metaFilename( int polarisation ) const { 
                if( polarisation > (int)_nPols ) return "";
                return _h5Filename[polarisation]; 
        }
        // ensure all data is flushed to disk
        void flush();

    protected:
        void _writeHeader(SpectrumDataSetBase* stokes);
        virtual void sendStream(const QString& streamName, const DataBlob* dataBlob);
        QString _clean( const QString& string );

    private:
        // Header helpers
        void _updateHeader( int polarisation );
        void _updateHeaders();
        virtual void _writeData( const SpectrumDataSetBase* ) = 0;
        unsigned int  _count, _checkPoint; // mark number of iterations for checkpointing

    protected:
        inline void _float2int(const float *f, int *i);
        void _setChannels( unsigned n );
        void _setPolsToWrite(unsigned p);
        inline unsigned polsToWrite() const { return _nPols; }

    protected:
        //typedef   std::ofstream fileType;
        typedef   FileWriter fileType;
        bool          _complexVoltages;
        unsigned int  _nBits;
        QVector<fileType*>    _file;
        bool    _separateFiles; 
        StokesType        _stokesType;

    private:
        QString           _filePath;
        QString           _label;
        QString           _observationID;
        QVector<long>              _fileBegin;
        QVector<dal::BF_File*>     _bfFiles;
        QVector<QString>           _rawFilename;
        QVector<QString>           _h5Filename;
        int _beamNr;
        int _sapNr;
        std::vector<ssize_t> _maxdims;
        QString       _sourceName, _raString, _decString, _telescope;
        float         _fch1, _foff, _tsamp, _refdm, _clock, _ra, _dec;
        float         _cropMin, _cropMax, _scaleDelta, _nRange; // for scaling floats to lesser values
        int           _nchans, _nTotalSubbands;
        unsigned int  _nRawPols, _nChannels, _nSubbands, _integration, _nPols;
        unsigned int  _nSubbandsToStore, _topsubband, _lbahba, _site, _machine;
        DEFINE_TIMER( _writeTimer )
        DEFINE_TIMER( _sendStreamTimer)
};

void H5_LofarBFDataWriter::_float2int(const float *f, int *i)
{
    float ftmp;
    ftmp = (*f>_cropMax)? (_cropMax) : *f;
    *i = (ftmp<_cropMin) ? 0 : (int)rint((ftmp-_cropMin)*_nRange/_scaleDelta);
}


} // namespace ampp
} // namespace pelican

#endif // LOFAR_DAL_FOUND
#endif // H5_LOFARBFDATAWRITER_H
