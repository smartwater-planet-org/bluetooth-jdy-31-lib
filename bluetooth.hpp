#ifndef BT_JDY_31_LIB
#define BT_JDY_31_LIB

#include <Arduino.h>
#include <stdint.h>

#if __has_include(<SoftwareSerial.h>)
#include <SoftwareSerial.h>
#endif

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
class Bluetooth : Stream
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
        size_t readBytes(char* buffer, size_t length);
        void setTimeout(long timeout);

        template <typename T>
        size_t write(T value)
        {
            return this->serial->write(value);
        }

        template <typename T>
        size_t print(T value)
        {
            return this->serial->print(value);
        }

        template <typename T>
        size_t println(T value)
        {
            return this->serial->println(value);
        }


    private:
        void setCmdPin(int state);
};
#endif


#endif
