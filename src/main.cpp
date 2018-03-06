/*
// File			: main.cpp
// Project		: Page.Me-Client
// By			: Benjamin Schropp (Ben) and Andrei Pana (Andrei)
// Date			: 
// Description	: Creates the WiFi network and handles network requests from Page.Me Clients
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

ESP8266WiFiMulti WiFiMulti;

const char* host = "192.168.4.1";
int param = 0;

String tName = "Jane";
int joined = 0;

void setup() {
    Serial.begin(9600);
    delay(10);
    
    // We start by connecting to a WiFi network
    WiFi.mode(WIFI_STA);
    WiFiMulti.addAP("ESPWebServer", "12345678");

    Serial.println();
    Serial.println();
    Serial.print("Wait for WiFi... ");

    while(WiFiMulti.run() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    delay(500);
}


void loop()
{
    /*if (Serial.available() > 0) {
        String test = Serial.readString();*/
        WiFiClient client;
        
        Serial.printf("\n[Connecting to %s ... ", host);
        if (client.connect(host, 80)) {
            if (joined == 0) {
                Serial.println("connected]");
                String content = "name=Jane&address=";
                String ip = client.localIP().toString();
                content.concat(ip);
                //content.concat(test);
                Serial.println(content);
                client.println("POST /add HTTP/1.1");
                String h = host;
                String hostPort = "Host: " + h + ":" + 80;
                client.println(hostPort);
                client.println("Cache-Control: no-cache");
                client.println("Content-Type: application/x-www-form-urlencoded");
                client.print("Content-Length: ");
                client.println(content.length());
                client.println();
                client.println(content);

                Serial.println("[Response:]");
                while (client.connected()) {
                    if (client.available()) {
                        String line = client.readStringUntil('\n');
                        if (line.compareTo(tName) == 0) {
                            //Update joined status
                            joined = 1;
                            Serial.println("Added to network successfully.");
                        }
                    }
                }
                client.stop();
                Serial.println("\n[Disconnected]");
            }
            else {
                Serial.println("connected]");                
                Serial.println("Already exist in this network.");
            }
        }
        else
        {
            Serial.println("connection failed!]");
            client.stop();
        }
    //}
    delay(5000);
}
