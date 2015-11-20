#ifndef PPF_CHANNELISER_TEST_H_
#define PPF_CHANNELISER_TEST_H_

/**
 * @file PPFChanneliserTest.h
 */

#include <cppunit/extensions/HelperMacros.h>

#include <QtCore/QString>

namespace pelican {
namespace ampp {

/**
 * @class PPFChanneliserTest
 *
 * @brief
 * CppUnit testing for the PPF channeliser module.
 */

class PPFChanneliserTest : public CppUnit::TestFixture
{
    public:
        PPFChanneliserTest() : CppUnit::TestFixture() {}
        virtual ~PPFChanneliserTest() {}

    public:
        /// Register test methods.
        CPPUNIT_TEST_SUITE(PPFChanneliserTest);
        CPPUNIT_TEST(test_run);
        CPPUNIT_TEST(test_channelProfile);
        CPPUNIT_TEST(test_makeSpectrum);
        CPPUNIT_TEST(test_configuration);
        CPPUNIT_TEST(test_threadAssign);
        CPPUNIT_TEST(test_updateBuffer);
        CPPUNIT_TEST(test_filter);
        CPPUNIT_TEST(test_fft);
        CPPUNIT_TEST_SUITE_END();

    public:
        void setUp();

        /// Test module configuration.
        void test_configuration();

        /// Test various thread assignment schemes.
        void test_threadAssign();

        /// Test updating the delay buffer.
        void test_updateBuffer();

        /// Test the FIR filter stage.
        void test_filter();

        /// Test the FFT stage.
        void test_fft();

        /// Test the modules public run method.
        void test_run();

        /// Test the constructing a spectrum given a set of weights.
        void test_makeSpectrum();

        /// Test the channel profile for a given set of weights.
        void test_channelProfile();

    private:
        /// Generate configuration XML.
        QString _configXml(unsigned nChannels, unsigned nThreads,
                unsigned nTaps, const QString& windowType = "kaiser");

    private:
        bool     _verbose;
        unsigned _nBlocks;
        unsigned _nSubbands;
        unsigned _nPols;
        unsigned _nChannels;
        unsigned _nTaps;
};

} // namespace ampp
} // namespace pelican

#endif // PPF_CHANNELISER_TEST_H_

