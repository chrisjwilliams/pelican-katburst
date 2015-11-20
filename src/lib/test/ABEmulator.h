#ifndef ABEMULATOR_H
#define ABEMULATOR_H

#include "pelican/emulator/AbstractUdpEmulator.h"
#include <QtCore/QByteArray>

namespace pelican {

class ConfigNode;

namespace ampp {

/*
 * This emulator outputs ALFABURST packets using a UDP socket. A sine wave is
 * FFT'd and psuedo-Stokes is put into the data stream.
 * It should be constructed with a configuration node that contains
 * the number of samples in the packet, the interval between packets in
 * microseconds, and the connection settings.
 *
 * The default values are:
 *
 * <ABEmulator>
 *     <packet samples="1024" interval="2560" />
 *     <signal period="20" />
 *     <connection host="127.0.0.1" port="2001" />
 * </ABEmulator>
 */
class ABEmulator : public AbstractUdpEmulator
{
    public:
        // Constructs a new UDP packet emulator.
        ABEmulator(const ConfigNode& configNode);

        // Destroys the UDP packet emulator.
        ~ABEmulator() {}

        // Creates a UDP packet.
        void getPacketData(char*& ptr, unsigned long& size);

        // Returns the interval between packets in microseconds.
        unsigned long interval() {return _interval;}
    private:
        unsigned long _counter;  // (6-Byte) packet counter.
        char _specQuart; // Spectral quarter (0 <= _specQuart < 4)
        char _beam; // Beam number (0 <= _beam < 8)
        unsigned long long _totalSamples; // The total number of samples sent.
        unsigned long _samples; // The number of (frequency) samples in the packet.
        unsigned long _interval; // The interval between packets in microsec.
        unsigned long _period; // The number of samples in one period.
        float _omega; // The angular frequency of the signal (from _period).
        QByteArray _packet; // The data packet.
};

} // namespace ampp
} // namespace pelican

#endif // ABEMULATOR_H

