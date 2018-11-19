/*  Andrea Vogrig #2018
 *   
 *  This sketch sends and "receives" random data over OSC->UDP on a ESP32 device
 *  
 *  INSTALL: (software & dependency)
 *  
 *    Arduino core for ESP32 WiFi chip
 *  - https://github.com/espressif/arduino-esp32 
 *  
 *    OSC modified library for ESP32 found on https://0110.be/posts/ESP32_Thing_as_xOSC_alternative
 *  - https://0110.be/files/attachments/449/ESP32-Arduino-OSC-library.zip
 *  
 *  TODO: osc input still does not work properly..
 *  
*/

#include <WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include <OSCBundle.h>
#include <OSCBoards.h>

// WiFi network name and password:
const char * networkName = "***************";
const char * networkPswd = "***************";

//IP address to send UDP data to:
// either use the ip address of the server or 
// a network broadcast address
const char * udpAddress = "***.***.***.***";
const int udpPort = 57120;

//Are we currently connected?
boolean connected = false;

//The udp library class
WiFiUDP udp;

void setup(){
  // Initilize hardware serial:
  Serial.begin(115200);
  
  //Connect to the WiFi network
  connectToWiFi(networkName, networkPswd);

}

void routeIn(OSCMessage &msg, int addrOffset ){
  Serial.println(msg.getInt(0));
}

void loop(){
  //only send data when connected
  if(connected){
    
    //Send a UDP packet
    //udp.beginPacket(udpAddress,udpPort);
    //udp.printf("Seconds since boot: %u", millis()/1000);
    //udp.endPacket();

    //Send a UDP/OSC packet//////////////
    OSCMessage msg("/bno");
    msg.add((int) 1);//millis());
    msg.add((int) 2);
    msg.add((float) 1.0);
    
    udp.beginPacket(udpAddress,udpPort);
      msg.send(udp);
    udp.endPacket();
    
    msg.empty();
    /////////////////////////////////////

   // listen for incoming OSC message.. still does not work properly
   OSCBundle bundleIN;
   int size;
 
   if( (size = udp.parsePacket())>0)
   {
     while(size--)
       bundleIN.fill(udp.read());
        Serial.println("in");
      if(!bundleIN.hasError())
        bundleIN.route("/test", routeIn);
   }

  }
  
  //Wait for 1 second
  delay(1000);
}

void connectToWiFi(const char * ssid, const char * pwd){
  Serial.println("Connecting to WiFi network: " + String(ssid));

  // delete old config
  WiFi.disconnect(true);
  //register event handler
  WiFi.onEvent(WiFiEvent);
  
  //Initiate connection
  WiFi.begin(ssid, pwd);

  Serial.println("Waiting for WIFI connection...");
}

//wifi event handler
void WiFiEvent(WiFiEvent_t event){
    switch(event) {
      case SYSTEM_EVENT_STA_GOT_IP:
          //When connected set 
          Serial.print("WiFi connected! IP address: ");
          Serial.println(WiFi.localIP());  
          //initializes the UDP state
          //This initializes the transfer buffer
          udp.begin(WiFi.localIP(),udpPort);
          connected = true;
          break;
      case SYSTEM_EVENT_STA_DISCONNECTED:
          Serial.println("WiFi lost connection");
          connected = false;
          break;
    }
}
