#include <WiFi.h>
#include <ESPmDNS.h>

#define pin15 15
#define timeoutTime 2000

const char* ssid = "PODA_3217";
const char* password = "300148300148";
const char* mdnsName = "svetla";

WiFiServer server(80);
String header;
bool output15State = false;

unsigned long currentTime = millis();
unsigned long previousTime = 0; 


void sendWebPage(WiFiClient& client, bool pinState) {
  // Send the HTTP Headers
  client.print("HTTP/1.1 200 OK\r\n");
  client.print("Content-type:text/html\r\n");
  client.print("Connection: close\r\n\r\n"); // The extra \r\n creates the crucial blank line

  // Send the Page Structure using a Raw String Literal
  client.print(R"=====(<!DOCTYPE html><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <style>
    html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}
    body { background-color: #2c2e35; color: white;}
    .button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px; text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}
    .button2 {background-color: #B22222;}
  </style>
</head>
<body>
  <h1>ESP32 Web Server</h1>
)=====");

  if (pinState == false) {
    client.print("<p>LED svetla - OFF</p>");
    client.print("<p><a href=\"/15/on\"><button class=\"button\">ON</button></a></p>");
  } else {
    client.print("<p>LED svetla - ON</p>");
    client.print("<p><a href=\"/15/off\"><button class=\"button button2\">OFF</button></a></p>");
  }

  // Send the rest of the HTML closing tags
  client.print(R"=====(</body></html>)=====");
}

void setup() {
  Serial.begin(115200);
  pinMode(pin15, OUTPUT);
  digitalWrite(pin15, LOW);
  Serial.println("");
  Serial.print("Connecting to: ");
  Serial.println(ssid);
  Serial.println("");

  WiFi.begin(ssid, password);
  
  while(WiFi.status() != WL_CONNECTED){
    currentTime = millis();
    if(currentTime - previousTime >= 500){
      previousTime = currentTime;
      Serial.print(".");
    }
  }
  Serial.println("");
  Serial.print("Connected! IP Address: ");
  Serial.println(WiFi.localIP());

  while(!MDNS.begin(mdnsName)){
    Serial.println("Error setting up MDNS responder!");
        if(currentTime - previousTime >= 500){
          previousTime = currentTime;
          Serial.print(".");
    }
  }
  MDNS.addService("_http", "_tcp", 80);
  Serial.println("mDNS responder started. Access the server at http://" + String(mdnsName) + ".local");
  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  
  if(client){
    currentTime = millis();
    previousTime = currentTime;
   
    Serial.println("New Client");
    String currentLine = "";

    while(client.connected() && (currentTime - previousTime <= timeoutTime)){
      currentTime = millis();
      
      if(client.available()){
        char c = client.read();
        Serial.write(c);
        header += c;  
        
        if(c == '\n'){
          
          if(currentLine.length() == 0){ // if line is blank, HTTP request ended
            

            if (header.indexOf("GET /15/on") >= 0) {
              Serial.println("GPIO 15 on");
              output15State = true;
              digitalWrite(pin15, HIGH);
            }
            else if (header.indexOf("GET /15/off") >= 0) {
              Serial.println("GPIO 15 off");
              output15State = false;
              digitalWrite(pin15, LOW);
            } 
            
            sendWebPage(client, output15State);
            break;     
          }
          else { 
            currentLine = "";     
          }
        }
        else if (c != '\r') {  
          currentLine += c;      
        }
      }
    }

    header = "";
    client.stop();
    Serial.println("Client Disconnected\n");
  }
}
