#ifndef SPECTRUM_DATA_SET_TEST_H_
#define SPECTRUM_DATA_SET_TEST_H_

#include <cppunit/extensions/HelperMacros.h>

/**
 * @file SpectrumDataSetTest.h
 */

/**
 * @class SpectrumDataSetTest
 *
 * @brief
 * Unit tests of the spectrum data set blob.
 */

namespace pelican {
namespace ampp {

class SpectrumDataSetTest : public CppUnit::TestFixture
{
    public:
        SpectrumDataSetTest() : CppUnit::TestFixture() {}
        ~SpectrumDataSetTest() {}

    public:
        void setUp() {}
        void tearDown() {}

        /// Test accessor methods.
        void test_accessorMethods();
        void test_serialise_deserialise();
        void test_access_performance();

        CPPUNIT_TEST_SUITE(SpectrumDataSetTest);
        CPPUNIT_TEST(test_accessorMethods);
        CPPUNIT_TEST(test_serialise_deserialise);
        CPPUNIT_TEST(test_access_performance);
        CPPUNIT_TEST_SUITE_END();
};


} // namespace ampp
} // namespace pelican
#endif // SPECTRUM_DATA_SET_TEST_H_
