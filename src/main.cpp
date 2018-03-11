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
int request = 0;
String response = "";
String nameTest = "";

void setupClientConnection();
void checkContactRequests();
void sendTeacherResponse();

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

    setupClientConnection();
    delay(500);
}

void loop()
{
    if (request == 0) {
        checkContactRequests();
    }    
    else {
        Serial.println("Request in progress.");
        if (Serial.available() > 0) {
            String message = Serial.readStringUntil('\n');
            //if (message.compareTo(":res123") == 0) {
            if (message.indexOf(":res") == 0) {
                message.remove(0, 4);
                response = message;
                Serial.println("hi there");
                Serial.println(response);
                sendTeacherResponse();
                //send response where we update the name on the server that's getting
                //requested
            }
        }
        //Read serial for response, then call method to send response to server, 
        //then set request back to 0 upon delivery confirmation
    }
    delay(5000);
}

void setupClientConnection() {
    int connected = 0;
    while(connected == 0) {
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
                            Serial.println("Added to network successfully.");
                        }
                        else if (line.compareTo("updated") == 0) {
                            Serial.println("Already existed in the database, but address was different so it has been updated.");
                        }
                        //Update joined status                        
                        joined = 1;    
                        connected = 1;                    
                    }
                }
                client.stop();
                Serial.println("\n[Disconnected]");
            }
            else {
                Serial.println("connected]");                
                Serial.println("Already exist in this network.");
                client.stop();
                Serial.println("\n[Disconnected]");
            }
        }
        else
        {
            Serial.println("connection failed!]");
            client.stop();
        }
    }
}

void checkContactRequests() {
    WiFiClient client;
    Serial.printf("\n[Connecting to %s ... ", host);
    if (client.connect(host, 80)) {
        Serial.println("connected]");
        Serial.println("Check contact request.");
        String content = "";
        client.println("POST /check HTTP/1.1");
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
                Serial.println(line);
                if (line.compareTo(tName) == 0) {
                    Serial.println("Request for you.");
                    request = 1;
                    nameTest = line;
                    client.stop();                  
                    Serial.write(":req");  
                    //Serial write to arduino, play notifications
                }      
                //need other if to check length to see if the teacher "none" so we 
                //can differentiate from no requests and request for different teacher
                else {
                    Serial.println("Request for other teacher.");
                    //Serial write to arduino, display on screen, no notifications
                }            
            }
        }
        client.stop();
        Serial.println("\n[Disconnected]");
    }
    else {
        Serial.println("connection failed!]");
        //Serial write unable to get requests
        client.stop();
    }
}

void sendTeacherResponse() {
    int responseCheck = 0;
    while(responseCheck == 0) {
        WiFiClient client;
        Serial.printf("\n[Connecting to %s ... ", host);
        if (client.connect(host, 80)) {
            Serial.println("connected]");
            //response = type of response from teacher (available, away, etc.)
            String content = "response=";
            Serial.print("Message Response: ");
            Serial.println(response);
            content.concat(response);
            client.println("POST /response HTTP/1.1");
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
                    Serial.print("Line: ");
                    Serial.println(line);
                    //Convert to integers to remove garbage characters
                    int a = atoi(line.c_str());
                    int b = atoi(response.c_str());
                    //if (line.compareTo(response) == 0) {
                    if (a == b) {
                        Serial.println("Response received.");
                        //reset checks for new requests
                        response = "";
                        responseCheck = 1;
                        request = 0;
                        client.stop();         
                    }      
                    else {
                        Serial.println("Response not received.");
                    }            
                }
            }
            client.stop();
            Serial.println("\n[Disconnected]");
        }
        else {
            Serial.println("connection failed!]");
            client.stop();
        }
    }
}