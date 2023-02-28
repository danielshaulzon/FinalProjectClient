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

// Define the red LED pin
#define RED_LED_PIN D5

// Define the button pin for the red LED
#define RED_BTN_PIN D6

// Define the green LED pin
#define GREEN_LED_PIN D7

// Define the button pin for the green LED
#define GREEN_BTN_PIN D3

// Create a new WebSocket client
WebSocketsClient webSocket;

// Create a new change detector for the red LED button
ChangeDetector<bool> redBtnChangeDetector(false);

// Create a new change detector for the green LED button
ChangeDetector<bool> greenBtnChangeDetector(false);

// The current state of the red LED
ChangeDetector<bool> redLedState = LOW;

// The current state of the green LED
ChangeDetector<bool> greenLedState = LOW;

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length) {
	switch (type) {
		case WStype_DISCONNECTED: {
			Serial.printf("[WSc] Disconnected!\n");
			break;
		}
		case WStype_CONNECTED: {
			Serial.printf("[WSc] Connected to url: %s\n", payload);
			if (redLedState.getValue()) {
				webSocket.sendTXT("red led turned on");
			} else {
				webSocket.sendTXT("red led turned off");
			}
			if (greenLedState.getValue()) {
				webSocket.sendTXT("green led turned on");
			} else {
				webSocket.sendTXT("green led turned off");
			}
			break;
		}
		case WStype_TEXT: {
			bool redLedChanged, greenLedChanged;
			String message = (char *)payload;
			if (message == "red led on") {
				redLedChanged = redLedState.changed(HIGH);
			} else if (message == "red led off") {
				redLedChanged = redLedState.changed(LOW);
			} else if (message == "green led on") {
				greenLedChanged = greenLedState.changed(HIGH);
			} else if (message == "green led off") {
				greenLedChanged = greenLedState.changed(LOW);
			} else if (message == "red led toggle") {
				redLedChanged = redLedState.changed(!redLedState.getValue());
			} else if (message == "green led toggle") {
				greenLedChanged = greenLedState.changed(!greenLedState.getValue());
			}

			if (redLedChanged) {
				bool newStatus = redLedState.getValue();
				digitalWrite(RED_LED_PIN, newStatus);
				if (newStatus) {
					webSocket.sendTXT("red led turned on");
				} else {
					webSocket.sendTXT("red led turned off");
				}
			}
			if (greenLedChanged) {
				bool newStatus = greenLedState.getValue();
				digitalWrite(GREEN_LED_PIN, newStatus);
				if (newStatus) {
					webSocket.sendTXT("green led turned on");
				} else {
					webSocket.sendTXT("green led turned off");
				}
			}
			break;
		}
	}
}

void setup() {
	pinMode(RED_LED_PIN, OUTPUT);
	pinMode(RED_BTN_PIN, INPUT_PULLUP);
	pinMode(GREEN_LED_PIN, OUTPUT);
	pinMode(GREEN_BTN_PIN, INPUT_PULLUP);

	digitalWrite(RED_LED_PIN, LOW);
	digitalWrite(GREEN_LED_PIN, LOW);

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

	bool redBtnPushed = digitalRead(RED_BTN_PIN) == LOW;
	if (redBtnChangeDetector.changed(redBtnPushed)) {
		if (redBtnChangeDetector.getValue() == true) {
			webSocket.sendTXT("red button pressed");
		}
	}

	bool greenBtnPushed = digitalRead(GREEN_BTN_PIN) == LOW;
	if (greenBtnChangeDetector.changed(greenBtnPushed)) {
		if (greenBtnChangeDetector.getValue() == true) {
			webSocket.sendTXT("green button pressed");
		}
	}
}
