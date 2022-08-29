#include "bluetooth.hpp"

#include <Arduino.h>
#include <stdint.h>
#include <string.h>

#if __has_include(<SoftwareSerial.h>)
#include <SoftwareSerial.h>
#endif

#define OK_RESPONSE     "+OK"
#define DEFAULT_TIMEOUT 5000
#define BUFFER_SIZE     128

char BUFFER[BUFFER_SIZE + 1];

/**
 * hex is an uppercase character in the range 0..F
 */
uint8_t parse_hex_nibble(char hex)
{
    /**
     * '0' is 0x30
     * 'A' is 0x41
     *
     * Substract 0x30 to hex to get 0..9 if hex is a number.
     * If its greater than 9, substract the distance of 0x30 to 0x41 (0x11)
     * and add 10 to it.
     *
     * 0x11 - 10 = 0x7
     */

    uint8_t byte = (uint8_t)hex - 0x30;
    if (byte < 10)
        return byte;

    return byte - 0x07;
}

/**
 *  - power_pin: ID of the pin used to control if the hc05 module gets any power.
 *               The implementation of this pin is most probably not built in.
 */
#ifdef SoftwareSerial_h
Bluetooth::Bluetooth(int rx, int tx, int cmd_pin, int state_pin, int power_pin, bool inverted_power_pin)
    : SoftwareSerial(rx, tx)
{
    this->cmd_pin            = cmd_pin;
    this->state_pin          = state_pin;
    this->power_pin          = power_pin;
    this->inverted_power_pin = inverted_power_pin;


    pinMode(cmd_pin, OUTPUT);
    pinMode(state_pin, INPUT_PULLUP);

    if (power_pin >= 0)
        pinMode(power_pin, OUTPUT);
};
#endif

#ifndef SoftwareSerial_h
Bluetooth::Bluetooth(Uart* serial,
                     int cmd_pin,
                     int state_pin,
                     int power_pin,
                     bool inverted_power_pin,
                     bool listen_for_urc)
    : serial(serial), cmd_pin(cmd_pin), state_pin(state_pin), power_pin(power_pin),
      inverted_power_pin(inverted_power_pin), listen_for_urc(listen_for_urc), Stream()
{
    pinMode(cmd_pin, OUTPUT);
    pinMode(state_pin, INPUT_PULLUP);

    if (power_pin >= 0)
        pinMode(power_pin, OUTPUT);
};
#endif


/**
 *
 * Write HIGH on power_pin and wait 500ms for
 * the HC05 module to initialize
 */
void Bluetooth::powerOn()
{
    if (this->power_pin >= 0) {
        digitalWrite(this->power_pin, inverted_power_pin ? LOW : HIGH);
        delay(500);
    }
}

/**
 * Write LOW on power_pin
 */
void Bluetooth::powerOff()
{
    if (this->power_pin >= 0)
        digitalWrite(this->power_pin, inverted_power_pin ? HIGH : LOW);
}

void Bluetooth::setBaud(long baud, uint32_t stop_bits, uint32_t parity)
{
    static const long rates[] = { 9600, 19200, 38400, 57600, 115200, 128000 };

    size_t baud_index = -1;

    for (size_t i = 0; i < (sizeof(rates) / sizeof(long)); i++) {
        if (rates[i] == baud) {
            baud_index = i;
            break;
        }
    }

    // Baud rate not found
    if (baud_index == -1)
        return;

    this->setCmdPin(HIGH);

    this->write("AT+BAUD");
    this->print(baud_index + 4);
    this->write("\r\n");
    delay(100);

    size_t recvd  = this->readBytes(BUFFER, BUFFER_SIZE);
    BUFFER[recvd] = 0;

    this->setCmdPin(LOW);

    // Failed to set baud rate
    if (recvd > 0 && strcmp(BUFFER, OK_RESPONSE) == 0)
        return;


    this->reset();
    this->end();
    this->begin(baud);
    delay(1000);
};

void Bluetooth::setBaud(long baud)
{
    this->setBaud(baud, 0, 0);
};

unsigned long Bluetooth::findBaud()
{

    static const long rates[] = { 9600, 19200, 38400, 57600, 115200, 128000 };

    int numRates = sizeof(rates) / sizeof(long);
    int response = false;
    int recvd    = 0;

    this->setCmdPin(HIGH);

    for (int rn = numRates - 1; rn >= 0; rn--) {

        const long baud = rates[rn];
        this->begin(baud);
        this->setTimeout(100);

        /**
         * From the specs, the jdy-31 has no "AT" command.
         * Use "AT+VERSION" as a replacement
         */
        this->write("AT+VERSION\r\n");
        delay(10);

        recvd = this->readBytes(BUFFER, BUFFER_SIZE);

        if (recvd > 0) {
            this->setCmdPin(LOW);
            return baud;
        }
    }

    this->setCmdPin(LOW);
    return 0;
}

bool Bluetooth::isConnected()
{
    return this->_is_connected || (digitalRead(this->state_pin) ? true : false);
}

void Bluetooth::handleConnectionURC(const char* str)
{
    if (strstr(str, "+CONNECTING") != NULL) {
        this->_is_connecting = true;
    } else if (strstr(str, "CONNECTED") != NULL && this->_is_connecting) {
        this->_is_connecting = false;
        this->_is_connected  = true;
    } else if (strstr(str, "+DISC:SUCCESS") != NULL) {
        this->_is_connecting = false;
        this->_is_connected  = false;
    }
}

/**
 * Waits for connection for `timeout` ms.
 * Will wait forever if `timeout` is negative.
 *
 * Will return `true` if theres a connection.
 */
bool Bluetooth::waitForConnection(unsigned long timeout)
{
    bool connected                = false;
    const unsigned long init_time = millis();

    while ((timeout < 0 || millis() - init_time <= timeout) && !connected) {
        this->readLine(BUFFER, BUFFER_SIZE);
        connected = this->isConnected();
    }

    return connected;
}

int Bluetooth::readLine(char* buffer, int length)
{
    return this->readBytesUntil('\n', buffer, length);
}

/**
 * Sets command pin state and waits to enter/exit command mode
 */
void Bluetooth::setCmdPin(int state)
{
    if (this->cmd_pin >= 0) {
        digitalWrite(this->cmd_pin, state);

        /**
         * Wait an arbitrary time to entry and exit command mode.
         * There is no specs on this time,
         * but it appears to depend on baud rate, with/ >100ms required at 9600 baud.
         */
        delay(150);
    }
}

/**
 * Send command without waiting for the response.
 * ! User must read or flush the buffer before and after calling this method.
 */
void Bluetooth::sendCommand(char* cmd, uint32_t timeout)
{
    int recvd = 0;
    this->setCmdPin(HIGH);

    this->write(cmd);
    this->write("\r\n");
    this->setTimeout(timeout);

    this->setCmdPin(LOW);
};

void Bluetooth::getVersion(char* buffer, int length)
{
    this->sendCommand("AT+VERSION", DEFAULT_TIMEOUT);
    this->readLine(buffer, length);
}

void Bluetooth::getBauds(char* buffer, int length)
{
    this->sendCommand("AT+BAUD", DEFAULT_TIMEOUT);
    this->readLine(buffer, length);
}

void Bluetooth::getName(char* buffer, int length)
{
    this->sendCommand("AT+NAME", DEFAULT_TIMEOUT);
    this->readLine(buffer, length);
}

void Bluetooth::getPin(char* buffer, int length)
{
    this->sendCommand("AT+PIN", DEFAULT_TIMEOUT);
    this->readLine(buffer, length);
}

bool Bluetooth::setName(char* name)
{
    strncpy(BUFFER, "AT+NAME", BUFFER_SIZE);
    strncat(BUFFER, name, BUFFER_SIZE);

    this->sendCommand(BUFFER, 1000);
    this->readLine(BUFFER, BUFFER_SIZE);

    Serial.print("SetName buffer: ");
    Serial.println(BUFFER);

    if (strcmp(BUFFER, OK_RESPONSE) != 0)
        return false;

    this->reset();
    return true;
}

bool Bluetooth::setPin(char* pin)
{
    strncpy(BUFFER, "AT+PIN", BUFFER_SIZE);
    strncat(BUFFER, pin, BUFFER_SIZE);

    this->sendCommand(BUFFER, DEFAULT_TIMEOUT);
    this->readLine(BUFFER, BUFFER_SIZE);

    Serial.print("SetPin buffer: ");
    Serial.println(BUFFER);

    if (strcmp(BUFFER, OK_RESPONSE) != 0)
        return false;

    this->reset();
    return true;
}

void Bluetooth::reset()
{
    this->sendCommand("AT+RESET", DEFAULT_TIMEOUT);
    this->readLine(BUFFER, BUFFER_SIZE);

    // Wait an arbitraty time
    delay(100);
}

void Bluetooth::resetFactory()
{
    this->sendCommand("AT+DEFAULT", DEFAULT_TIMEOUT);
    this->readLine(BUFFER, BUFFER_SIZE);

    // Wait an arbitraty time
    delay(100);
}

void Bluetooth::disconnect()
{
    this->sendCommand("AT+DISC", DEFAULT_TIMEOUT);
    this->_is_connected  = false;
    this->_is_connecting = false;
    this->readLine(BUFFER, BUFFER_SIZE);
}

/**
 * Define all methods that would be provided by SoftwareSerial,
 * bind methods to this->serial
 */

#ifndef SofwareSerial_H
void Bluetooth::flush()
{
    this->serial->flush();
};

int Bluetooth::read()
{
    return this->serial->read();
};

size_t Bluetooth::readBytesUntil(char terminator, char* buffer, size_t length)
{
    size_t recv = this->serial->readBytesUntil(terminator, buffer, length);
    this->handleConnectionURC(buffer);
    return recv;
};

void Bluetooth::begin(unsigned long baudRate)
{
    this->serial->begin(baudRate);
}

void Bluetooth::begin(unsigned long baudrate, uint16_t config)
{
    this->serial->begin(baudrate, config);
}

void Bluetooth::end()
{
    this->serial->end();
}

int Bluetooth::available()
{
    return this->serial->available();
}

int Bluetooth::availableForWrite()
{
    return this->serial->availableForWrite();
}

int Bluetooth::peek()
{
    return this->serial->peek();
}

void Bluetooth::setTimeout(long timeout)
{
    this->serial->setTimeout(timeout);
}

size_t Bluetooth::readBytes(char* buffer, size_t length)
{
    size_t recv = this->serial->readBytes(buffer, length);
    this->handleConnectionURC(buffer);
    return recv;
}


#endif
