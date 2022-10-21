#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ctime>
#include <time.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// access credentials for WiFi network.
// const char *ssid = "";
// const char *password = "";


// WiFI server. Listen on port 80, which is the default TCP port for HTTP
WiFiServer server(80);

//
const int oneWireBus = D3;
OneWire onewire(oneWireBus);
DallasTemperature DS18B20(&onewire);

void setup()
{
  Serial.begin(115200);
  Serial.println("\nWeb Server Demo");
  Serial.printf("\nConnecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");
  server.begin();
  Serial.printf("Web server started, open %s in a web browser\n", WiFi.localIP().toString().c_str());
}
void loop()
{
  // get temputre
  DS18B20.requestTemperatures();
  float fTemp = DS18B20.getTempCByIndex(0);

  WiFiClient client = server.available();
  // wait for a client (web browser) to connect
  if (client)
  {
    Serial.println("\n>> Client connected");
    Serial.println(">> Client request:");
    while (client.connected())
    {
      // read line by line what the client (web browser) is requesting
      if (client.available())
      {
        // print information about the request (browser, OS, etc)
        String line = client.readStringUntil('\r');
        Serial.print(line);
        // wait for end of client's request
        if (line.length() == 1 && line[0] == '\n')
        {
          // send some data back to the browser. Note: this is not
          // proper HTML!

          // get runing time
          time_t endTime = time(nullptr);
          char tmp[32] = {NULL};
          strftime(tmp, sizeof(tmp), "%H:%M:%S", localtime(&endTime));
          Serial.println("formated: " + String(tmp));

          client.printf("<h1>Current tempture is %s &#8451; </h1>", String(fTemp));
          client.printf("<p>Hello from %s </p>", WiFi.localIP().toString().c_str());
          client.printf("<p>Server has been running for %s </p>", tmp);
          client.println("<footer>formatted by YongxingZhang <a href='mailto:000766325@mohakcollege.ca'>000766325@mohawkcollege.ca</a></footer>");
          Serial.println(">> Response sent to client");
          break;
        }
      }
    }
    // keep read client request data (if any). After that, we can terminate
    // our client connection
    while (client.available())
    {
      client.read();
    }
    // close the connection:
    client.stop();
    Serial.println(">> Client disconnected");
  }
}

// format time
