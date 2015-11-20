#include "DedispersionModuleTest.h"
#include "DedispersionModule.h"
#include "pelican/utility/ConfigNode.h"
#include "SpectrumDataSet.h"
#include "WeightedSpectrumDataSet.h"
#include "DedispersionDataGenerator.h"
#include <boost/bind.hpp>
#include <iostream>
#include "pelican/utility/ConfigNode.h"
#include <QDebug>


namespace pelican {

namespace ampp {

CPPUNIT_TEST_SUITE_REGISTRATION( DedispersionModuleTest );
/**
 *@details DedispersionModuleTest 
 */
DedispersionModuleTest::DedispersionModuleTest()
    : CppUnit::TestFixture()
{
}

/**
 *@details
 */
DedispersionModuleTest::~DedispersionModuleTest()
{
}

void DedispersionModuleTest::setUp()
{
}

void DedispersionModuleTest::tearDown()
{
}

void DedispersionModuleTest::test_multipleBuffersPerBlob()
{
    // Use Case:
    // Blob size is bigger than a single buffer
    // Expect:
    // Launch multiple async tasks
    float dm = 10.0;
    int multiple=4; // factor of blob samples size to buffer size
    unsigned ddSamples = 200;
    unsigned nBlocks = 2;
    unsigned nSamples = 6400;
    CPPUNIT_ASSERT( nSamples%multiple == 0 );
    DedispersionDataGenerator stokesData;
    stokesData.setTimeSamplesPerBlock( nSamples );
    QList<SpectrumDataSetStokes*> spectrumData = stokesData.generate( nBlocks, dm );

    WeightedSpectrumDataSet weightedData(spectrumData[0]);
    WeightedSpectrumDataSet weightedData2(spectrumData[1]);
    ConfigNode config;

    // setup configuration
    QString configString = QString("<DedispersionModule>"
            " <invertedData value=\"0\" />"
            " <sampleNumber value=\"%1\" />"
            " <frequencyChannel1 MHz=\"%2\"/>"
            " <channelBandwidth MHz=\"%3\"/>"
            " <dedispersionSamples value=\"%4\" />"
            " <dedispersionStepSize value=\"0.1\" />"
            " <numberOfBuffers value=\"3\" />"
            "</DedispersionModule>")
        .arg( nSamples / multiple  ) // block size should match the buffer size to ensure we get two calls to the GPU
        .arg( stokesData.startFrequency())
        .arg( stokesData.bandwidthOfSample())
        .arg( ddSamples );
     config.setFromString(configString);

     try {
        DedispersionModule ddm(config);
        ddm.connect( boost::bind( &DedispersionModuleTest::connected, this, _1 ) );
        ddm.onChainCompletion( boost::bind( &DedispersionModuleTest::connectFinished, this ) );
        ddm.unlockCallback( boost::bind( &DedispersionModuleTest::unlockCallback, this, _1 ) );
        _connectData = 0;
        _connectCount = 0;
        _chainFinished = 0;
        _unlocked.clear();
        ddm.dedisperse( &weightedData ); // asynchronous tasks launch
        while( _connectCount < multiple ) { sleep(1); }; // expect more times
                                                         // due to maxshift
        while( _chainFinished != _connectCount ) { sleep(1); };
     }
     catch( const QString& s )
     {
         CPPUNIT_FAIL(s.toStdString());
     }
     stokesData.deleteData(spectrumData);
}

void DedispersionModuleTest::test_multipleBlobsPerBufferUnaligned ()
{
     // Uses Case:
     // buffer size is bigger than a single DataSample
     // and it is not an integer number of the DataSample
     // size so that the Data sample is split over two buffers
     // Maxshift should be small enough to leave plenty of room
     // to allow for unlocked DataBlobs
     // Expect:
     // Datablobs to be locked until they are finished with
     // even across the buffer boundry
     float dm = 10.0;
     float multiple=5.5; // factor of blob samples size to buffer size
     unsigned ddSamples = 200;
     unsigned nBlocks = 6;
     unsigned nSamples = 3200;
     DedispersionDataGenerator stokesData;
     stokesData.setTimeSamplesPerBlock( nSamples );
     QList<SpectrumDataSetStokes*> spectrumData = stokesData.generate( nBlocks, dm );
     QList<SpectrumDataSetStokes*> spectrumDataCopy
                                        = stokesData.deepCopy( spectrumData );
     CPPUNIT_ASSERT_EQUAL( spectrumData.size(), spectrumDataCopy.size() );
     CPPUNIT_ASSERT( stokesData.equal( spectrumData, spectrumDataCopy ));

     WeightedSpectrumDataSet weightedData(spectrumData[0]);
     WeightedSpectrumDataSet weightedData2(spectrumData[1]);
     ConfigNode config;

     // setup configuration
     QString configString = QString("<DedispersionModule>"
            " <invertedData value=\"0\" />"
             " <sampleNumber value=\"%1\" />"
             " <frequencyChannel1 MHz=\"%2\"/>"
             " <channelBandwidth MHz=\"%3\"/>"
            " <dedispersionSamples value=\"%4\" />"
            " <dedispersionStepSize value=\"0.1\" />"
            " <numberOfBuffers value=\"2\" />"
            "</DedispersionModule>")
        .arg( nSamples * multiple  ) // block size should match the buffer size to ensure we get two calls to the GPU
        .arg( stokesData.startFrequency())
        .arg( stokesData.bandwidthOfSample())
        .arg( ddSamples );
     config.setFromString(configString);

     try {
         DedispersionModule ddm(config);
         ddm.connect( boost::bind( &DedispersionModuleTest::connected, this, _1 ) );
         ddm.onChainCompletion( boost::bind( &DedispersionModuleTest::connectFinished, this ) );
         ddm.unlockCallback( boost::bind( &DedispersionModuleTest::unlockCallback, this, _1 ) );
         _connectData = 0;
         _connectCount = 0;
         _chainFinished = 0;
         _unlocked.clear();
         // first dedisperse calls should just buffer
         int i;
         for(i=0; i < (int)multiple; ++i ) {
             WeightedSpectrumDataSet weightedData(spectrumData[i]);
             ddm.dedisperse( &weightedData ); // asynchronous task
             CPPUNIT_ASSERT_EQUAL( 0, _connectCount ); // no data should be processed first time
             CPPUNIT_ASSERT( (int)nSamples > ddm.maxshift() ); // ensures first sample should be released
         }
         stokesData.equal( spectrumData, spectrumDataCopy );
         // next dedisperse call should trigger a process chain
         WeightedSpectrumDataSet weightedData(spectrumData[i]);
         ddm.dedisperse( &weightedData ); // asynchronous task
         CPPUNIT_ASSERT( ddm.lockNumber( spectrumData[i] ) > 1 ); // needs to be reserved for maxshift
         while( _connectCount == 0 ) { sleep(1); };
         CPPUNIT_ASSERT_EQUAL( 1, _connectCount );

         float expectedDMIntentsity = spectrumData[0]->nSubbands() * spectrumData[0]->nChannels();
         CPPUNIT_ASSERT_EQUAL( expectedDMIntentsity , _connectData->dmAmplitude( 0, dm ) );
         while( _chainFinished == 0 ) { sleep(1); }

         CPPUNIT_ASSERT( stokesData.equal( spectrumData, spectrumDataCopy ));
         // check that the datablobs are freed
         for( i=0; i < (int)multiple; ++i ) {
             CPPUNIT_ASSERT_EQUAL( 0, ddm.lockNumber( spectrumData[i] ) );
         }

         // the split blob should still be locked after processing the
         // first buffer as it is partually used in the second
         // it may also be associated with the xshift, in either
         // case it should only be locked once
         CPPUNIT_ASSERT_EQUAL( 1, ddm.lockNumber( spectrumData[i] ));

         // expect unlock trigger
         CPPUNIT_ASSERT_EQUAL( (int)multiple, _unlocked.size() );
         CPPUNIT_ASSERT_EQUAL( (void *)spectrumData[0], (void *)_unlocked[0] );

         // ensure lock clears for the latest spectrumData after next iteration
         for(i=0; i < (int)multiple; ++i ) {
             WeightedSpectrumDataSet weightedData(spectrumData[i]);
             ddm.dedisperse( &weightedData ); // asynchronous task
         }
         while( _connectCount == 1 ) { sleep(1); };
         CPPUNIT_ASSERT_EQUAL( 2, _connectCount );
         CPPUNIT_ASSERT_EQUAL( 0, ddm.lockNumber( spectrumData[i] ));
     }
     catch( const QString& s )
     {
         CPPUNIT_FAIL(s.toStdString());
     }
     stokesData.deleteData(spectrumData);
     stokesData.deleteData(spectrumDataCopy);
}

void DedispersionModuleTest::test_multipleBlobsPerBuffer ()
{
     // Use Case:
     // Buffer size is bigger than a single WeightedData sample set
     // Expect:
     // Asyncronous task to only happen when data buffer is full
     // Data is unlocked
    float dm = 10.0;
    unsigned ddSamples = 200;
    unsigned nBlocks = 2;
    unsigned nSamples = 6400;
    DedispersionDataGenerator stokesData;
    stokesData.setTimeSamplesPerBlock( nSamples );
    QList<SpectrumDataSetStokes*> spectrumData = stokesData.generate( nBlocks, dm );

    WeightedSpectrumDataSet weightedData(spectrumData[0]);
    WeightedSpectrumDataSet weightedData2(spectrumData[1]);
    ConfigNode config;

    // setup configuration
    QString configString = QString("<DedispersionModule>"
            " <invertedData value=\"0\" />"
            " <sampleNumber value=\"%1\" />"
            " <frequencyChannel1 MHz=\"%2\"/>"
            " <channelBandwidth MHz=\"%3\"/>"
            " <dedispersionSamples value=\"%4\" />"
            " <dedispersionStepSize value=\"0.1\" />"
            " <numberOfBuffers value=\"3\" />"
            "</DedispersionModule>")
        .arg( nSamples * 2 ) // block size should match the buffer size to ensure we get two calls to the GPU
        .arg( stokesData.startFrequency())
        .arg( stokesData.bandwidthOfSample())
        .arg( ddSamples );
     config.setFromString(configString);

     try {
        DedispersionModule ddm(config);
        ddm.connect( boost::bind( &DedispersionModuleTest::connected, this, _1 ) );
        ddm.onChainCompletion( boost::bind( &DedispersionModuleTest::connectFinished, this ) );
        ddm.unlockCallback( boost::bind( &DedispersionModuleTest::unlockCallback, this, _1 ) );
        _connectData = 0;
        _connectCount = 0;
        _chainFinished = 0;
        _unlocked.clear();
        // first dedisperse call should just buffer
        ddm.dedisperse( &weightedData ); // asynchronous task
        CPPUNIT_ASSERT_EQUAL( 0, ddm.lockNumber( &weightedData ) );
        CPPUNIT_ASSERT_EQUAL( 0, ddm.lockNumber( &weightedData2 ) );
        CPPUNIT_ASSERT_EQUAL( 0, ddm.lockNumber( spectrumData[1] ) );
        CPPUNIT_ASSERT_EQUAL( 0, _connectCount ); // no data should be processed first time
        CPPUNIT_ASSERT( (int)nSamples > ddm.maxshift() ); // ensures first sample should be released
        // second dedisperse call should trigger a process chain
        ddm.dedisperse( &weightedData2 ); // asynchronous task
        CPPUNIT_ASSERT( ddm.lockNumber( spectrumData[1] ) > 1 ); // needs to be reserved for maxshift
        while( _connectCount == 0 ) { sleep(1); };
        CPPUNIT_ASSERT_EQUAL( 1, _connectCount );

        float expectedDMIntentsity = spectrumData[0]->nSubbands() * spectrumData[0]->nChannels();
        CPPUNIT_ASSERT_EQUAL( expectedDMIntentsity , _connectData->dmAmplitude( 0, dm ) );
        while( _chainFinished == 0 ) { sleep(1); }
        CPPUNIT_ASSERT_EQUAL( 1, ddm.lockNumber( spectrumData[1] ) );
        CPPUNIT_ASSERT_EQUAL( 0, ddm.lockNumber( spectrumData[0] ) );

        // expect unlock trigger
        CPPUNIT_ASSERT_EQUAL( 1, _unlocked.size() );
        CPPUNIT_ASSERT_EQUAL( (void *)spectrumData[0], (void *)_unlocked[0] );
     }
     catch( const QString& s )
     {
         CPPUNIT_FAIL(s.toStdString());
     }
     stokesData.deleteData(spectrumData);
}

void DedispersionModuleTest::unlockCallback( const QList<DataBlob*>& data ) {
     _unlocked=data;
}

void DedispersionModuleTest::test_multipleBlobs ()
{
     // Use Case:
     // Dedispersed signal spread over multiple data blobs
     // Data per blob is the same size as the databuffer
     // Expect:
     // two calls to the gpu.
     // Overlap of data from the first blob inline with the maxshift 
     // parameter
     // In time the blobs will overlap thus:
     // | data1      |
     //          |  data2     |
     // i.e. the tail end of datablob1 will be duplicated to the beginning of
     // the second buffer.
     // As the buffer is of fixed size, all the data in the second blob will
     // not be included, but should be placed in the next awaiting buffer
    float dm = 10.0;
    unsigned ddSamples = 200;
    unsigned nBlocks = 2;
    unsigned nSamples = 3400;
    DedispersionDataGenerator stokesData;
    stokesData.setTimeSamplesPerBlock( nSamples );
    QList<SpectrumDataSetStokes*> spectrumData = stokesData.generate( nBlocks, dm );
    stokesData.writeToFile( "inputStokes.data", spectrumData );

    WeightedSpectrumDataSet weightedData(spectrumData[0]);
    WeightedSpectrumDataSet weightedData2(spectrumData[1]);
    ConfigNode config;
    CPPUNIT_ASSERT_EQUAL( nSamples, spectrumData[0]->nTimeBlocks() );
    // setup configuration
    QString configString = QString("<DedispersionModule>"
            " <invertedData value=\"0\" />"
            " <sampleNumber value=\"%1\" />"
            " <frequencyChannel1 MHz=\"%2\"/>"
            " <channelBandwidth MHz=\"%3\"/>"
            " <dedispersionSamples value=\"%4\" />"
            " <dedispersionStepSize value=\"0.1\" />"
            " <numberOfBuffers value=\"3\" />"
            "</DedispersionModule>")
        .arg( nSamples ) // block size should match the buffer size to ensure we get two calls to the GPU
        .arg( stokesData.startFrequency())
        .arg( stokesData.bandwidthOfSample())
        .arg( ddSamples );
    config.setFromString(configString);

    try {
          DedispersionModule ddm(config);
          ddm.connect( boost::bind( &DedispersionModuleTest::connected, this, _1 ) );
          _connectData = 0;
          _connectCount = 0;
          ddm.dedisperse( &weightedData ); // asynchronous task
          while( _connectCount != 1 ) { sleep(1); };
          float expectedDMIntentsity = spectrumData[0]->nSubbands() * spectrumData[0]->nChannels();
          CPPUNIT_ASSERT_EQUAL( expectedDMIntentsity , _connectData->dmAmplitude( 0, dm ) );
          ddm.dedisperse( &weightedData2 ); // asynchronous task
          while( _connectCount != 2 ) { sleep(1); };
    }
    catch( const QString& s )
    {
        CPPUNIT_FAIL(s.toStdString());
    }
    stokesData.deleteData(spectrumData);
}

void DedispersionModuleTest::test_method()
{
    try {
        float dm = 10.0;
        unsigned ddSamples = 200;
        unsigned nBlocks = 1;
        unsigned nSamples = 3200;
        DedispersionDataGenerator stokesData;
        stokesData.setTimeSamplesPerBlock( nSamples );
        QList<SpectrumDataSetStokes*> spectrumData = stokesData.generate( nBlocks, dm );
        //stokesData.writeToFile( "inputStokes.data", spectrumData );

        WeightedSpectrumDataSet weightedData(spectrumData[0]);
        { // Use Case:
          // Single Data Blob as input, of same size as the buffer
          // Expect:
          // output dataBlob to be filled
          // returned output blob should be the same as that passed
          // to any connected functions
          ConfigNode config;
          CPPUNIT_ASSERT_EQUAL( nSamples, spectrumData[0]->nTimeBlocks() );
          QString configString = QString("<DedispersionModule>"
                                         " <invertedData value=\"0\" />"
                                         " <sampleNumber value=\"%1\" />"
                                         " <frequencyChannel1 MHz=\"%2\"/>"
                                         " <channelBandwidth MHz=\"%3\"/>"
                                         " <dedispersionSamples value=\"%4\" />"
                                         " <dedispersionStepSize value=\"0.1\" />"
                                         "</DedispersionModule>")
                                        .arg( nSamples )
                                        .arg( stokesData.startFrequency())
                                        .arg( stokesData.bandwidthOfSample())
                                        .arg( ddSamples );
          config.setFromString(configString);
          DedispersionModule ddm(config);
          ddm.connect( boost::bind( &DedispersionModuleTest::connected, this, _1 ) );
          _connectData = 0;
          _connectCount = 0;
          ddm.dedisperse( &weightedData ); // asynchronous task
          while( ! _connectCount ) { sleep(1); };
          CPPUNIT_ASSERT_EQUAL( 1, _connectCount );
          int outputSampleSize = (int)(((nSamples - ddm.maxshift() )));
          //CPPUNIT_ASSERT_EQUAL( (int)(outputSampleSize*ddSamples), _connectData->data().size() );
          CPPUNIT_ASSERT_EQUAL( (size_t)(outputSampleSize*ddSamples), _connectData->data().size() );
// Print out the resulting data
//          std::ofstream file("output.data");
//          for( int i=0; i < buffer->current()->data().size(); ++i ) {
//                file << (buffer->current()->data())[i] << std::string(((i+1)%outputSampleSize)?" ":"\n");
//          }
//          std::cout << std::endl;

          float expectedDMIntentsity = spectrumData[0]->nSubbands() * spectrumData[0]->nChannels();
          CPPUNIT_ASSERT_EQUAL( expectedDMIntentsity , _connectData->dmAmplitude( 0, dm ) );
          stokesData.deleteData(spectrumData);
        }
    }
    catch( QString s )
    {
        CPPUNIT_FAIL(s.toStdString());
    }
}

void DedispersionModuleTest::test_dataConsistency() {
    // Use case:
    // Ensure Input DataBlobs do not get corrupted after 
    // passing through the dedisperse algo
    // Ensure each Blob is unique to ensure detection
    unsigned multiple = 5.3;
    unsigned nTimeBlocks = 6400;
    unsigned nSubbands = 64;
    unsigned nChan = 32;
    unsigned nPol = 1;
    QList<SpectrumDataSetStokes*> spectrumData;
    for( int i=0; i < (int)(2.0*multiple); ++i ) { // 2 full buffers worth
       SpectrumDataSetStokes* d = new SpectrumDataSetStokes;
       d->resize( nTimeBlocks, nSubbands, nPol, nChan );
       d->setBlockRate( 0.00032768 );
       // set unique data for each data point
       float* data = d->data();
       for(int j=0; j < d->size(); ++j ) {
           data[j]= j + i;
       }
       spectrumData.append(d);
    }
    QList<SpectrumDataSetStokes*> spectrumDataCopy =
                        DedispersionDataGenerator::deepCopy( spectrumData );
    try {
         DedispersionModule ddm(testConfig( nTimeBlocks * multiple));
         ddm.connect( boost::bind( &DedispersionModuleTest::connected, this, _1 ) );
         ddm.onChainCompletion( boost::bind( &DedispersionModuleTest::connectFinished, this ) );
         _connectData = 0;
         _connectCount = 0;
         _chainFinished = 0;
         int i;
         for(i=0; i < spectrumData.size(); ++i ) {
             WeightedSpectrumDataSet weightedData(spectrumData[i]);
             ddm.dedisperse( &weightedData ); // asynchronous task
         }
         while( _connectCount != 2 ) { sleep(1); };
         while( _chainFinished != _connectCount ) { sleep(1); };
         CPPUNIT_ASSERT( DedispersionDataGenerator::equal(spectrumData, spectrumDataCopy ));
    }
    catch( const QString& s )
    {
        CPPUNIT_FAIL(s.toStdString());
    }
    DedispersionDataGenerator::deleteData(spectrumData);
    DedispersionDataGenerator::deleteData(spectrumDataCopy);
}

void DedispersionModuleTest::connected( DataBlob* dataOut ) {
    ++_connectCount;
    _connectData = 0;
    CPPUNIT_ASSERT( ( _connectData = dynamic_cast<DedispersionSpectra* >(dataOut) ) );
}

void DedispersionModuleTest::connectFinished() {
    ++_chainFinished;
}

ConfigNode DedispersionModuleTest::testConfig(unsigned nSamples) const
{
    ConfigNode node;
    QString xml = QString("<DedispersionModule >\n"
                " <invertedData value=\"0\" />"
                " <sampleNumber value=\"%1\" />"
                " <frequencyChannel1 MHz=\"150.0\"/>"
                " <channelBandwidth MHz=\"-0.0292969\"/>" // -6.0/(nSubbands*nChannels);
                " <dedispersionSamples value=\"200\" />"
                " <dedispersionStepSize value=\"0.1\" />"
              "</DedispersionModule>\n").arg(nSamples);
    node.setFromString( xml );
    return node;
}

} // namespace ampp
} // namespace pelican
