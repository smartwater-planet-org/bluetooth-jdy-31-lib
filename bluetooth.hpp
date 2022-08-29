#ifndef BT_JDY_31_LIB
#define BT_JDY_31_LIB

#include <Arduino.h>
#include <stdint.h>

#if __has_include(<SoftwareSerial.h>)
#include <SoftwareSerial.h>
#endif


class BT_Base : public Stream
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

        virtual void flush() = 0;

        virtual size_t readBytesUntil(char terminator, char* buffer, size_t length) = 0;

        virtual size_t readBytes(char* buffer, size_t length) = 0;

        virtual void setTimeout(long timeout) = 0;

        virtual size_t write(uint8_t)                            = 0;
        virtual size_t write(const char* str)                    = 0;
        virtual size_t write(const uint8_t* buffer, size_t size) = 0;
        virtual size_t write(const char* buffer, size_t size)    = 0;

        virtual size_t print(const __FlashStringHelper* helper)    = 0;
        virtual size_t print(const String& str)                    = 0;
        virtual size_t print(const char* str)                      = 0;
        virtual size_t print(char v)                               = 0;
        virtual size_t print(unsigned char v, int base = DEC)      = 0;
        virtual size_t print(int v, int base = DEC)                = 0;
        virtual size_t print(unsigned int v, int base = DEC)       = 0;
        virtual size_t print(long v, int base = DEC)               = 0;
        virtual size_t print(unsigned long v, int base = DEC)      = 0;
        virtual size_t print(long long v, int base = DEC)          = 0;
        virtual size_t print(unsigned long long v, int base = DEC) = 0;
        virtual size_t print(double v, int base = 2)               = 0;
        virtual size_t print(const Printable& printable)           = 0;

        virtual size_t println(void)                                 = 0;
        virtual size_t println(const __FlashStringHelper* helper)    = 0;
        virtual size_t println(const String& str)                    = 0;
        virtual size_t println(const char* str)                      = 0;
        virtual size_t println(char v)                               = 0;
        virtual size_t println(unsigned char v, int base = DEC)      = 0;
        virtual size_t println(int v, int base = DEC)                = 0;
        virtual size_t println(unsigned int v, int base = DEC)       = 0;
        virtual size_t println(long v, int base = DEC)               = 0;
        virtual size_t println(unsigned long v, int base = DEC)      = 0;
        virtual size_t println(long long v, int base = DEC)          = 0;
        virtual size_t println(unsigned long long v, int base = DEC) = 0;
        virtual size_t println(double v, int base = 2)               = 0;
        virtual size_t println(const Printable& printable)           = 0;
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

        virtual size_t write(uint8_t v)
        {
            return m_impl->write(v);
        };

        virtual size_t write(const char* str)
        {
            return this->m_impl->write(str);
        };

        virtual size_t write(const uint8_t* buffer, size_t size)
        {
            return this->m_impl->write(buffer, size);
        };

        virtual size_t write(const char* buffer, size_t size)
        {
            return this->m_impl->write(buffer, size);
        };

        virtual size_t print(const __FlashStringHelper* helper)
        {
            return this->m_impl->print(helper);
        };

        virtual size_t print(const String& str)
        {
            return this->m_impl->print(str);
        };

        virtual size_t print(const char* str)
        {
            return this->m_impl->print(str);
        };

        virtual size_t print(char c)
        {
            return this->m_impl->print(c);
        };

        virtual size_t print(unsigned char v, int base = DEC)
        {
            return this->m_impl->print(v, base);
        };

        virtual size_t print(int v, int base = DEC)
        {
            return this->m_impl->print(v, base);
        };

        virtual size_t print(unsigned int v, int base = DEC)
        {
            return this->m_impl->print(v, base);
        };

        virtual size_t print(long v, int base = DEC)
        {
            return this->m_impl->print(v, base);
        };

        virtual size_t print(unsigned long v, int base = DEC)
        {
            return this->m_impl->print(v, base);
        };

        virtual size_t print(long long v, int base = DEC)
        {
            return this->m_impl->print(v, base);
        };

        virtual size_t print(unsigned long long v, int base = DEC)
        {
            return this->m_impl->print(v, base);
        };

        virtual size_t print(double v, int base = 2)
        {
            return this->m_impl->print(v, base);
        };

        virtual size_t print(const Printable& printable)
        {
            return this->m_impl->print(printable);
        };

        virtual size_t println(void)
        {
            return this->m_impl->println();
        };

        virtual size_t println(const __FlashStringHelper* helper)
        {
            return this->m_impl->println(helper);
        };

        virtual size_t println(const String& str)
        {
            return this->m_impl->println(str);
        };

        virtual size_t println(const char* str)
        {
            return this->m_impl->println(str);
        };

        virtual size_t println(char c)
        {
            return this->m_impl->println(c);
        };

        virtual size_t println(unsigned char v, int base = DEC)
        {
            return this->m_impl->println(v, base);
        };

        virtual size_t println(int v, int base = DEC)
        {
            return this->m_impl->println(v, base);
        };

        virtual size_t println(unsigned int v, int base = DEC)
        {
            return this->m_impl->println(v, base);
        };

        virtual size_t println(long v, int base = DEC)
        {
            return this->m_impl->println(v, base);
        };

        virtual size_t println(unsigned long v, int base = DEC)
        {
            return this->m_impl->println(v, base);
        };

        virtual size_t println(long long v, int base = DEC)
        {
            return this->m_impl->println(v, base);
        };

        virtual size_t println(unsigned long long v, int base = DEC)
        {
            return this->m_impl->println(v, base);
        };

        virtual size_t println(double v, int base = 2)
        {
            return this->m_impl->println(v, base);
        };

        virtual size_t println(const Printable& printable)
        {
            return this->m_impl->println(printable);
        };
};

#ifdef SoftwareSerial_h
class Bluetooth : public SoftwareSerial
{
    public:
        int cmd_pin;
        int state_pin;
        int power_pin;
        bool inverted_power_pin;

        uint8_t client_mac[6] = { 0 };

        Bluetooth(int rx, int tx, int cmd_pin, int state_pin, int power_pin = -1, bool invert_power_pin = false);

        void powerOn();
        void powerOff();

        void setBaud(long baud, uint32_t stop_bits, uint32_t parity);
        void setBaud(long baud);
        unsigned long findBaud();

        size_t poll();
        bool isConnected();
        bool waitForConnection(unsigned long timeout);

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
    private:
        bool _is_connected  = false;
        bool _is_connecting = false;

        void setCmdPin(int state);
        void handleConnectionURC(const char* str);

    public:
        Uart* serial;
        int cmd_pin;
        int state_pin;
        int power_pin;
        bool inverted_power_pin;
        bool listen_for_urc;

        uint8_t client_mac[6] = { 0 };

        Bluetooth(Uart* serial,
                  int cmd_pin,
                  int state_pin,
                  int power_pin         = -1,
                  bool invert_power_pin = false,
                  bool listen_for_urc   = false);


        void powerOn();
        void powerOff();

        void setBaud(long baud, uint32_t stop_bits, uint32_t parity);
        void setBaud(long baud);
        unsigned long findBaud();

        size_t poll();
        bool isConnected();
        bool waitForConnection(unsigned long timeout);

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
};
#endif


#endif
