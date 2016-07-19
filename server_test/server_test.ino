/*
  Chat Server

  A simple server that distributes any incoming messages to all
  connected clients.  To use, telnet to your device's IP address and type.
  You can see the client's input in the serial monitor as well.

  created 18 Dec 2009
  by David A. Mellis
  modified 9 Apr 2012
  by Tom Igoe
  modified 15 Jul 2016
  by Chris Shoemaker
*/

#include <SPI.h>
#include <Ethernet.h>

#define BUFF_SIZE 512

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network.
// gateway and subnet are optional:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 0, 86);
//IPAddress myDns(192, 168, 1, 1);
//IPAddress gateway(192, 168, 1, 1);
//IPAddress subnet(255, 255, 0, 0);

// telnet defaults to port 23
EthernetServer server(23);
boolean alreadyConnected = false; // whether or not the client was connected previously

String g_inputStream = ""; // input stream to carry a command given by the client

void setup() {
  // initialize the ethernet device
  Ethernet.begin(mac, ip);
  // start listening for clients
  server.begin();
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.print("Server address is ");
  Serial.println(Ethernet.localIP());
}

void loop() {
  // wait for a new client:
  EthernetClient client = server.available();

  // when the client sends the first byte, say hello:
  if (client) {
    if (!alreadyConnected) {
      // clear out the input buffer:
      client.flush();
      Serial.println("New client is online");
      alreadyConnected = true;
    }

    while (client.available() > 0) {
      char inch = client.read();
      g_inputStream.concat(inch);
      Serial.print(inch);
    }
    char buff[BUFF_SIZE] = "";
    if (g_inputStream[0] == 'C' && g_inputStream[1] == 'S') {
      String response = "Initiate control state display...";
      response.toCharArray(buff, BUFF_SIZE);
      client.write(buff, BUFF_SIZE);
    }
//    if (g_inputStream.startsWith("Say hello")) {
//      String response = "Bonjour, do you speak le francais?  Hon hon hon.";
//      response.toCharArray(buff, BUFF_SIZE);
//      client.write(buff, BUFF_SIZE);
//    } else if (g_inputStream.startsWith("ORVuCtrl")) {
//      g_inputStream.toCharArray(buff, BUFF_SIZE);
//      client.write(buff, BUFF_SIZE);  
//    } else {
//      String response = "Invalid input.  Please enter in [9] and try again.";
//      response.toCharArray(buff, BUFF_SIZE);
//      client.write(buff, BUFF_SIZE);
//    } 
    g_inputStream = "";
    Serial.println("");
    Serial.println(g_inputStream);
  }
}


