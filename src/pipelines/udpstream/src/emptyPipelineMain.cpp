#include "pelican/core/PipelineApplication.h"

#include "LofarStreamDataClient.h"
#include "EmptyPipeline.h"
#include "AdapterTimeSeriesDataSet.h"
#include "LofarOutputStreamers.h"

#include <QtCore/QCoreApplication>

#include <iostream>
#include <map>

using namespace pelican;
using namespace pelican::ampp;

int main(int argc, char* argv[])
{
    // Create a QCoreApplication.
    QCoreApplication app(argc, argv);

    try {
        // Create a PipelineApplication.
        PipelineApplication pApp(argc, argv);

        // Register the pipelines that can run.
        pApp.registerPipeline(new EmptyPipeline);

        // Set the data client.
        pApp.setDataClient("LofarStreamDataClient");

        // Start the pipeline driver.
        pApp.start();
    }
    catch (const QString& error) {
        std::cout << "Error: " << error.toStdString() << std::endl;
    }

    return 0;
}
