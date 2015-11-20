#ifndef UDPBFPIPELINEINTEGRATIONTEST_H
#define UDPBFPIPELINEINTEGRATIONTEST_H

#include <cppunit/extensions/HelperMacros.h>

/**
 * @file UdpBFPipelineIntegrationTest.h
 */
class QProcess;

namespace pelican {

namespace ampp {

/**
 * @class UdpBFPipelineIntegrationTest
 *  
 * @brief
 * 
 * @details
 * 
 */

class UdpBFPipelineIntegrationTest : public CppUnit::TestFixture
{
    public:
        CPPUNIT_TEST_SUITE( UdpBFPipelineIntegrationTest );
        CPPUNIT_TEST( test_topdownInit );
        CPPUNIT_TEST_SUITE_END();

    public:
        void setUp();
        void tearDown();

        // Test Methods
        void test_topdownInit();

    protected:
        void startServer();
        void stopServer();
        QProcess* _server;

    public:
        UdpBFPipelineIntegrationTest(  );
        ~UdpBFPipelineIntegrationTest();

};

} // namespace ampp
} // namespace pelican
#endif // UDPBFPIPELINEINTEGRATIONTEST_H 
