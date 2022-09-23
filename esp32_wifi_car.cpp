#include "esp32_wifi_car.h"

WifiCar::WifiCar()
{
    init();
}

WifiCar::WifiCar(const int *newMotorPins, const int *newSpeedControlPins)
{
    for (size_t i = 0; i < MOTOR_PIN_COUNT; i++)
    {
        motorPins[i] = *newMotorPins + i;
    }

    for (size_t i = 0; i < SPEED_CONTROL_PIN_COUNT; i++)
    {
        speedControlpins[i] = *newSpeedControlPins + i;
    }
}

WifiCar::~WifiCar()
{
}

void WifiCar::init()
{
    for (size_t iCtr = 0; iCtr < MOTOR_PIN_COUNT; iCtr++)
    {
        // Set all pins to output
        pinMode(motorPins[iCtr], OUTPUT);

        // Set initial motor state to OFF
        digitalWrite(motorPins[iCtr], LOW);
    }

    for (size_t iCtr = 0; iCtr < SPEED_CONTROL_PIN_COUNT; iCtr++)
    {
        // Set all Speed Control pins to output
        pinMode(speedControlpins[iCtr], OUTPUT);
    }

    // Set the PWM Settings
    ledcSetup(channel_0, freq, resolution);
    ledcSetup(channel_1, freq, resolution);

    // Attach Pin to Channel
    ledcAttachPin(speedControlpins[0], channel_0);
    ledcAttachPin(speedControlpins[1], channel_1);

    // initialize default speed to NORMAL
    setCurrentSpeed(speedSettings::NORMAL);
}

// Set the motor speed
void WifiCar::setMotorSpeed()
{
    // change the duty cycle of the speed control pin connected to the motor
    ledcWrite(channel_0, currentSpeedSettings);
    ledcWrite(channel_1, currentSpeedSettings);
}

void WifiCar::setMotorPinStatus(uint8_t *pinStatusPointer)
{
    for (size_t iCtr = 0; iCtr < MOTOR_PIN_COUNT; iCtr++)
    {
        digitalWrite(motorPins[iCtr], *pinStatusPointer + iCtr);
    }
}

void WifiCar::turnLeft()
{
    setMotorSpeed();
    uint8_t status[MOTOR_PIN_COUNT] = {LOW, HIGH, LOW, LOW};
    setMotorPinStatus(status);
}

void WifiCar::turnRight()
{
    setMotorSpeed();
    uint8_t status[MOTOR_PIN_COUNT] = {LOW, LOW, LOW, HIGH};
    setMotorPinStatus(status);
}

void WifiCar::moveForward()
{
    setMotorSpeed();
    uint8_t status[MOTOR_PIN_COUNT] = {LOW, HIGH, LOW, HIGH};
    setMotorPinStatus(status);
}

void WifiCar::moveBackward()
{
    setMotorSpeed();
    uint8_t status[MOTOR_PIN_COUNT] = {HIGH, LOW, HIGH, LOW};
    setMotorPinStatus(status);
}

void WifiCar::stop()
{
    ledcWrite(channel_0, 0);
    ledcWrite(channel_1, 0);

    uint8_t status[MOTOR_PIN_COUNT] = {LOW, LOW, LOW, LOW};
    setMotorPinStatus(status);
}

// Set the current speed
void WifiCar::setCurrentSpeed(speedSettings newSpeedSettings)
{
    currentSpeedSettings = newSpeedSettings;
}

// Get the current speed
speedSettings WifiCar::getCurrentSpeed()
{
    return currentSpeedSettings;
}

void WifiCar::start(std::function<void(AsyncWebSocket *, AsyncWebSocketClient *, AwsEventType,
                                       void *, uint8_t *, size_t)>
                        webSocketCallbackFunc)
{
    // Add callback function to websocket server
    ws.onEvent(webSocketCallbackFunc);
    server.addHandler(&ws);

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              {
              request->send(SPIFFS, "/index.html", "text/html", false); });

    // Route to load entireframework.min.css file
    server.on("/css/entireframework.min.css", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/css/entireframework.min.css", "text/css"); });

    // Route to load custom.css file
    server.on("/css/custom.css", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/css/custom.css", "text/css"); });

    // Route to load custom.js file
    server.on("/js/custom.js", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/js/custom.js", "text/javascript"); });

    // On Not Found
    server.onNotFound(notFound);

    // Start server
    server.begin();
}

void notFound(AsyncWebServerRequest *request)
{
    request->send(404, "text/plain", "Not found");
}

void WifiCar::sendCarCommand(const char *command)
{
    // command could be either "left", "right", "forward" or "reverse" or "stop"
    // or speed settingg "slow-speed", "normal-speed", or "fast-speed"
    if (strcmp(command, "left") == 0)
    {
        this->turnLeft();
    }
    else if (strcmp(command, "right") == 0)
    {
        this->turnRight();
    }
    else if (strcmp(command, "up") == 0)
    {
        this->moveForward();
    }
    else if (strcmp(command, "down") == 0)
    {
        this->moveBackward();
    }
    else if (strcmp(command, "stop") == 0)
    {
        this->stop();
    }
    else if (strcmp(command, "slow-speed") == 0)
    {
        this->setCurrentSpeed(speedSettings::SLOW);
    }
    else if (strcmp(command, "normal-speed") == 0)
    {
        this->setCurrentSpeed(speedSettings::NORMAL);
    }
    else if (strcmp(command, "fast-speed") == 0)
    {
        this->setCurrentSpeed(speedSettings::FAST);
    }
}


