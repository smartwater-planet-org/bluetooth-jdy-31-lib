#include "bluetooth.hpp"

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <stdint.h>
#include <string.h>


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
Bluetooth::Bluetooth(int rx, int tx, int cmd_pin, int state_pin, int power_pin = -1) : SoftwareSerial(rx, tx)
{
    this->rx        = rx;
    this->tx        = tx;
    this->cmd_pin   = cmd_pin;
    this->state_pin = state_pin;
    this->power_pin = power_pin;


    pinMode(cmd_pin, OUTPUT);
    pinMode(state_pin, INPUT_PULLUP);

    if (power_pin >= 0)
        pinMode(power_pin, OUTPUT);
};

/**
 *
 * Write HIGH on power_pin and wait 500ms for
 * the HC05 module to initialize
 */
void Bluetooth::powerOn()
{
    if (this->power_pin >= 0) {
        digitalWrite(this->power_pin, HIGH);
        delay(500);
    }
}

/**
 * Write LOW on power_pin
 */
void Bluetooth::powerOff()
{
    if (this->power_pin >= 0)
        digitalWrite(this->power_pin, LOW);
}

void Bluetooth::setBaud(long baud, uint32_t stop_bits, uint32_t parity)
{
    this->setCmdPin(HIGH);

    this->write("AT+UART=");
    this->print(baud);
    this->print(",");
    this->print(stop_bits);
    this->print(",");
    this->print(parity);

    this->write("\r\n");

    size_t recvd  = this->readBytes(BUFFER, BUFFER_SIZE);
    BUFFER[recvd] = 0;
    Serial.print("Set baud response: ");
    Serial.println(BUFFER);


    digitalWrite(this->cmd_pin, LOW);
    this->begin(baud);
    delay(1000);
};

void Bluetooth::setBaud(long baud)
{
    this->setBaud(baud, 0, 0);
};

unsigned long Bluetooth::findBaud()
{

    static const long rates[] = { 4800, 9600, 19200, 38400, 57600, 115200, 128000 };

    int numRates = sizeof(rates) / sizeof(long);
    int response = false;
    int recvd    = 0;

    this->setCmdPin(HIGH);

    for (int rn = numRates - 1; rn >= 0; rn--) {

        const long baud = rates[rn];

        this->begin(baud);
        this->setTimeout(100);
        this->flush();

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
    return (digitalRead(this->state_pin) ? true : false);
};

void Bluetooth::printClientMAC(bool new_line = false)
{
    for (int i = 0; i < 5; i++) {
        Serial.print(this->client_mac[i], HEX);
        Serial.print(":");
    }
    Serial.print(this->client_mac[5], HEX);

    if (new_line)
        Serial.print('\n');
}

bool Bluetooth::handlNewConnection()
{
    size_t recvd = 0;

    recvd = this->readLine(BUFFER, BUFFER_SIZE);

    if (recvd < 30)
        return false;

    /**
     * Expects "+CONNECTING<<xx:xx:xx:xx:xx:xx"
     *          ^            ^
     *          0            13
     */
    uint8_t byte_index = 0;
    for (int i = 13; i < 30; i += 3) {
        this->client_mac[byte_index] = parse_hex_nibble(BUFFER[i]) << 4;
        this->client_mac[byte_index] |= parse_hex_nibble(BUFFER[i + 1]);

        byte_index++;
    }

    // Flush "CONNECTED" response
    recvd = this->readLine(BUFFER, BUFFER_SIZE);

    return true;
};

int Bluetooth::readLine(char* buffer, int length)
{
    int recvd = this->readBytesUntil('\n', buffer, length);
    if (recvd < length)
        buffer[recvd] = 0;

    return recvd;
};

/**
 * Sets command pin state and waits to enter/exit command mode
 */
void Bluetooth::setCmdPin(int state)
{
    digitalWrite(this->cmd_pin, state);

    /**
     * Wait an arbitrary time to entry and exit command mode.
     * There is no specs on this time,
     * but it appears to depend on baud rate, with/ >100ms required at 9600 baud.
     */
    delay(150);
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
    this->readLine(BUFFER, BUFFER_SIZE);
}
