/*
 Name:		sacndmx.ino
 Created:	9/3/2015 10:39:54 AM
 Author:	Jesse Higginson
*/

// the setup function runs once when you press reset or power the board
#include <WiFiUdp.h>
#include <ESP8266WiFi.h>

#define LED_PIN				0
#define SACN_PORT			5568
#define UNIVERSE			1
#define SACN_GARBAGE_BYTES	123
#define DMX_SPEED			250000

const char* ssid = "FlynnEffect";
const char* password = "FlynnEffect";
long lastUpdateTime = -1;

WiFiUDP udp;

void setup() 
{
	WiFi.begin(ssid, password);
	Serial.begin(115200);
	Serial1.begin(DMX_SPEED); // serial1's pins are 14 (GPIO2) and 23 (SD_D1)
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
	}
}

// the loop function runs over and over again until power down or reset
void loop() 
{
	int packetSize = udp.parsePacket();
	if (packetSize > 0)
	{
		for (int i = 0; i < SACN_GARBAGE_BYTES; i++)
		{
			udp.read();
		}
		short size = (udp.read() << 8) + udp.read();
		/*byte* values = new byte[size];
		udp.read(values, size);*/
		sendBreak();
		for (int i = 0; i < size; i++)
		{
			Serial1.write(udp.read());
		}	
	}
}

void sendBreak()
{
	Serial1.end();
	Serial1.begin(57600);
	Serial1.write(0); // BREAK signal
	Serial1.end();
	Serial1.begin(DMX_SPEED);
}

void connect()
{
	IPAddress multicastAddress(239, 255, 0, UNIVERSE);
	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
	}
	lastUpdateTime = -1;
	udp.beginMulticast(WiFi.localIP(), multicastAddress, SACN_PORT);
}
