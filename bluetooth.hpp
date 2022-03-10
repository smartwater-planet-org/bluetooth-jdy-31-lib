#ifndef BT_JDY_31_LIB
#define BT_JDY_31_LIB

#include <Arduino.h>
#include <stdint.h>

#if __has_include(<SoftwareSerial.h>)
#include <SoftwareSerial.h>
#endif


class BT_Base : public Print
{
    public:
        virtual ~BT_Base() {}

        virtual void begin(unsigned long baudRate) = 0;

        virtual void begin(unsigned long baudrate, uint16_t config) = 0;

        virtual void end() = 0;

        virtual int available() = 0;

        virtual int availableForWrite() = 0;

        virtual int peek() = 0;

        virtual int read() = 0;

        virtual size_t write(uint8_t) = 0;

        virtual void flush() = 0;

        virtual size_t readBytesUntil(char terminator, char* buffer, size_t length) = 0;

        virtual size_t readBytes(char* buffer, size_t length) = 0;

        virtual void setTimeout(long timeout) = 0;
};

template <class C>
class BT_Wrapper : public BT_Base
{
    private:
        C* m_impl;

    public:
        BT_Wrapper(C* impl) : m_impl(impl) {}

        ~BT_Wrapper() {}

        virtual void begin(unsigned long baudRate)
        {
            m_impl->begin(baudRate);
        };

        virtual void begin(unsigned long baudrate, uint16_t config)
        {
            m_impl->begin(baudrate, config);
        };

        virtual void end()
        {
            m_impl->end();
        };

        virtual int available()
        {
            return m_impl->available();
        };

        virtual int availableForWrite()
        {
            return m_impl->availableForWrite();
        };

        virtual int peek()
        {
            return m_impl->peek();
        };

        virtual int read()
        {
            return m_impl->read();
        };

        virtual size_t write(uint8_t v)
        {
            return m_impl->write(v);
        };

        virtual void flush()
        {
            m_impl->flush();
        };

        virtual size_t readBytesUntil(char terminator, char* buffer, size_t length)
        {
            return m_impl->readBytesUntil(terminator, buffer, length);
        };

        virtual size_t readBytes(char* buffer, size_t length)
        {
            return m_impl->readBytes(buffer, length);
        };

        virtual void setTimeout(long timeout)
        {
            m_impl->setTimeout(timeout);
        };

        using Print::print;
        using Print::println;
        using Print::write;
};

#ifdef SoftwareSerial_h
class Bluetooth : public SoftwareSerial
{
    public:
        int rx;
        int tx;
        int cmd_pin;
        int state_pin;
        int power_pin;

        uint8_t client_mac[6] = { 0 };

        Bluetooth(int rx, int tx, int cmd_pin, int state_pin, int power_pin = -1);

        void powerOn();
        void powerOff();

        void setBaud(long baud, uint32_t stop_bits, uint32_t parity);
        void setBaud(long baud);
        unsigned long findBaud();

        bool isConnected();
        void printClientMAC(bool new_line = false);
        bool handlNewConnection();

        int readLine(char* buffer, int length);
        void sendCommand(char* cmd, uint32_t timeout);

        // void getName();
        void getVersion(char* buffer, int length);
        void getBauds(char* buffer, int length);
        void getName(char* buffer, int length);
        void getPin(char* buffer, int length);

        bool setName(char* name);
        bool setPin(char* pin);

        void reset();
        void resetFactory();
        void disconnect();

    private:
        void setCmdPin(int state);
};
#endif

#ifndef SoftwareSerial_h
class Bluetooth : public Stream
{
    public:
        Uart* serial;
        int rx;
        int tx;
        int cmd_pin;
        int state_pin;
        int power_pin;

        uint8_t client_mac[6] = { 0 };

        Bluetooth(Uart* serial, int cmd_pin, int state_pin, int power_pin = -1);

        void powerOn();
        void powerOff();

        void setBaud(long baud, uint32_t stop_bits, uint32_t parity);
        void setBaud(long baud);
        unsigned long findBaud();

        bool isConnected();
        void printClientMAC(bool new_line = false);
        bool handlNewConnection();

        int readLine(char* buffer, int length);
        void sendCommand(char* cmd, uint32_t timeout);

        // void getName();
        void getVersion(char* buffer, int length);
        void getBauds(char* buffer, int length);
        void getName(char* buffer, int length);
        void getPin(char* buffer, int length);

        bool setName(char* name);
        bool setPin(char* pin);

        void reset();
        void resetFactory();
        void disconnect();

        void begin(unsigned long baudRate);
        void begin(unsigned long baudrate, uint16_t config);
        void end();
        int available();
        int availableForWrite();
        int peek();
        int read();
        void flush();
        size_t readBytesUntil(char terminator, char* buffer, size_t length);
        size_t readBytes(char* buffer, size_t length);
        void setTimeout(long timeout);

        size_t write(uint8_t value)
        {
            return this->serial->write(value);
        }

        size_t write(char* value, size_t length)
        {
            return this->serial->write(value, length);
        }

        using Print::print;
        using Print::println;
        using Print::write;


    private:
        void setCmdPin(int state);
};
#endif


#endif
