/**
 * @file SocketIO.ino
 * @author Daniel
 * @brief This program recived if the button is pressed or released and sends it to the server.
 * \n The server then sends a message to the client to turn the LED on or off.
 * @version 0.0.1
 * @date 2023-01-15
 * @copyright Copyright (c) 2023
 */

// Include the WiFi library
#include <ESP8266WiFi.h>

// Include the WebSockets library
#include <WebSocketsClient.h>

// Include the WifiConfig library
#include "WifiConfig.h"

// Include the ChangeDetector library
#include "ChangeDetector.h"

// Define the LED pin
#define LED_PIN D5

// Define the button pin
#define BTN_PIN D6

// Create a new WebSocket client
WebSocketsClient webSocket;

// Create a new change detector for the button
ChangeDetector<bool> btnChangeDetector(false);

// The current state of the LED
ChangeDetector<bool> ledState = LOW;

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length) {
	switch (type) {
		case WStype_DISCONNECTED: {
			Serial.printf("[WSc] Disconnected!\n");
			break;
		}
		case WStype_CONNECTED: {
			Serial.printf("[WSc] Connected to url: %s\n", payload);
			if (ledState.getValue()) {
				webSocket.sendTXT("LED on");
			} else {
				webSocket.sendTXT("LED off");
			}
			break;
		}
		case WStype_TEXT: {
			bool ledChanged;
			if (String((char *)payload) == "led on") {
				ledChanged = ledState.changed(HIGH);
			} else if (String((char *)payload) == "led off") {
				ledChanged = ledState.changed(LOW);
			}
			if (ledChanged) {
				bool newStatus = ledState.getValue();
				digitalWrite(LED_PIN, newStatus);
				webSocket.sendTXT(newStatus ? "LED on" : "LED off");
			}
			break;
		}
	}
}

void setup() {
	pinMode(LED_PIN, OUTPUT);
	pinMode(BTN_PIN, INPUT_PULLUP);
	digitalWrite(LED_PIN, LOW);

	Serial.begin(115200);
	if (!Serial) {
		while (true);
	}

	WiFi.begin(WIFI_SSID, WIFI_PASS);

	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}

	Serial.println("\nConnected to WiFi");
	Serial.print("IP address: ");
	Serial.println(WiFi.localIP());

	webSocket.begin(HOST_IP, HOST_PORT, "/esp8266");
	webSocket.onEvent(webSocketEvent);
}

void loop() {
	webSocket.loop();

	bool btnPushed = digitalRead(BTN_PIN) == LOW;
	if (btnChangeDetector.changed(btnPushed)) {
		if (btnChangeDetector.getValue() == true) {
			webSocket.sendTXT("button pressed");
		} else {
			webSocket.sendTXT("button released");
		}
	}
}

