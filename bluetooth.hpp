#include <SoftwareSerial.h> // Librería para la comunicación Bluetooth

// SoftwareSerial BT(4, 6);    // Pines Tx y Rx del módulo Bluetooth, respectivamente

enum Command {
    NAME,
};

class Bluetooth
{
    public:
        int rx;
        int tx;
        int cmd_pin;
        int state_pin;
        int power_pin;

        uint8_t client_mac[6] = { 0 };

        SoftwareSerial* serial;

        Bluetooth(int rx, int tx, int cmd_pin, int state_pin, int power_pin = -1);

        void powerOn();
        void powerOff();

        void begin(long baud);
        void end();
        void setBaud(long baud, uint32_t stop_bits, uint32_t parity);
        void setBaud(long baud);
        unsigned long findBaud();

        bool isConnected();
        void printClientMAC(bool new_line = false);
        bool handlNewConnection();


        int available();
        char read();
        int readLine(char* buffer, int length);
        void flush();
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
