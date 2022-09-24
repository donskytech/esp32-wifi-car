#ifndef ESP32_WIFI_CAR_H
#define ESP32_WIFI_CAR_H

#include "Arduino.h"
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "SPIFFS.h"

constexpr size_t MOTOR_PIN_COUNT { 4 };
constexpr size_t SPEED_CONTROL_PIN_COUNT { 2 };

/*
  The resolution of the PWM is 8 bit so the value is between 0-255
  We will set the speed between 100 to 255.
*/
enum speedSettings
{
    SLOW = 100,
    NORMAL = 180,
    FAST = 255
};

class WifiCar
{
private:
    // Motor Pin
    int motorPins[4] = {16, 17, 32, 33};
    int speedControlpins[2] = {21, 22};

    const int freq = 2000;
    // PWM Channel frequency
    const int channel_0 = 1;
    const int channel_1 = 2;
    // 8 Bit resolution for duty cycle so value is between 0 - 255
    const int resolution = 8;

    // holds the current speed settings, see values for SLOW, NORMAL, FAST
    speedSettings currentSpeedSettings;

    // AsyncWebserver runs on port 80 and the asyncwebsocket is initialize at this point also
    AsyncWebServer server{80};
    AsyncWebSocket ws{"/ws"};

public:
    WifiCar();
    // If you want to override a new motor pins
    WifiCar(const int * newMotorPins, const int * newSpeedControlPins);
    ~WifiCar();

    // initializes all pins and PWM pins
    void init();

    // Set motor pin status
    void setMotorPinStatus(uint8_t *pinStatusPointer);

    // Turn the car left
    void turnLeft();

    // Set the motor speed
    void setMotorSpeed();

    // Turn the car right
    void turnRight();

    // Move the car forward
    void moveForward();

    // Move the car backward
    void moveBackward();

    // Stop the car
    void stop();

    // Set the current speed
    void setCurrentSpeed(speedSettings newSpeedSettings);

    // Get the current speed
    speedSettings getCurrentSpeed();

    // send car command
    void sendCarCommand(const char *command);

    void start(std::function<void(AsyncWebSocket *, AsyncWebSocketClient *, AwsEventType,
                                  void *, uint8_t *, size_t)>);

};

// Function called when resource is not found on the server
void notFound(AsyncWebServerRequest *request);

#endif