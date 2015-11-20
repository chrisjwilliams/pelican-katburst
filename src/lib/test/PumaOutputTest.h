#ifndef PUMAOUTPUTTEST_H
#define PUMAOUTPUTTEST_H
#include <QtCore/QString>
#include "SpectrumDataSet.h"
class QCoreApplication;

#include <cppunit/extensions/HelperMacros.h>

/**
 * @file PumaOutputTest.h
 */

namespace pelican {

namespace ampp {

/**
 * @class PumaOutputTest
 *
 * @brief
 *
 * @details
 *
 */

class PumaOutputTest : public CppUnit::TestFixture
{
    public:
        CPPUNIT_TEST_SUITE( PumaOutputTest );
        CPPUNIT_TEST( test_configuration );
        CPPUNIT_TEST_SUITE_END();

    public:
        void setUp();
        void tearDown();

        // Test Methods
        void test_configuration();

    public:
        PumaOutputTest();
        virtual ~PumaOutputTest();

    private:
      QString _filename;
      SpectrumDataSetStokes _dummyData;
      QCoreApplication* _app;
};

} // namespace ampp
} // namespace pelican
#endif // PUMAOUTPUTTEST_H
