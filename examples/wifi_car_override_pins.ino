#include <Arduino.h>
#include "esp32_wifi_car.h"

// Change this to your network SSID
const char *ssid = "donsky-4thFloor";
const char *password = "donsky982";

const int newMotorPins[4] = {1, 2, 3, 4};
const int newSpeedControlPins[2] = {5, 6};

WifiCar wifiCar(newMotorPins, newSpeedControlPins);

// Callback function that receives messages from websocket client
void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
               void *arg, uint8_t *data, size_t len)
{
  switch (type)
  {
  case WS_EVT_DATA:
  {
    // data packet
    AwsFrameInfo *info = (AwsFrameInfo *)arg;
    if (info->final && info->index == 0 && info->len == len)
    {
      // the whole message is in a single frame and we got all of it's data
      if (info->opcode == WS_TEXT)
      {
        data[len] = 0;
        char *command = (char *)data;
        wifiCar.sendCarCommand(command);
      }
    }
    break;
  }
  default:
    // Serial.println("Received other socket event!");
    break;
  }
}

// Setup function
void setup()
{
  // Initialize the serial monitor baud rate
  Serial.begin(115200);
  Serial.println("Connecting to ");
  Serial.println(ssid);

  // Connect to your wifi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.printf("WiFi Failed!\n");
    return;
  }

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Initialize SPIFFS
  if (!SPIFFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  wifiCar.start(onWsEvent);

}

void loop()
{
  // No code in here.  Server is running in asynchronous mode
}
