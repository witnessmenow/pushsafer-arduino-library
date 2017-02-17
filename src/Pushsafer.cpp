/*
Copyright (c) 2015 Giancarlo Bacchio. All right reserved.

TelegramBot - Library to create your own Telegram Bot using
ESP8266 on Arduino IDE.
Ref. Library at https:github/esp8266/Arduino

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/

#include "Pushsafer.h"

Pushsafer::Pushsafer(String key, Client &client)	{
  _key = key;
  this->client = &client;
}

String Pushsafer::sendEvent(PushSaferInput input) {

  String body = "";
  String headers = "";
	long now;
	bool responseReceived;
  String boundary = "------------------------b8f610217e83e29b";
	// Connect with api.telegram.org
  if (client->connect(HOST, SSL_PORT)) {

    String start_request = "";
    String end_request = "";

    start_request = start_request + buildString(boundary, "m", input.message);
    start_request = start_request + buildString(boundary, "d", input.device);
    start_request = start_request + buildString(boundary, "k", _key);
    start_request = start_request + buildString(boundary, "s", input.sound);
    start_request = start_request + buildString(boundary, "v", input.vibration);
    start_request = start_request + buildString(boundary, "i", input.icon);

    // Optional Params
    if(input.title != "")
    {
      start_request = start_request + buildString(boundary, "t", input.title);
    }
    if(input.url != "")
    {
      start_request = start_request + buildString(boundary, "u", input.url);
    }
    if(input.urlTitle != "")
    {
      start_request = start_request + buildString(boundary, "ut", input.urlTitle);
    }

    end_request = end_request + "--" + boundary + "--" + "\r\n";

    client->print("POST /api"); client->println(" HTTP/1.1");
    // Host header
    client->print("Host: "); client->println(HOST);
    client->println("User-Agent: arduino/1.0");
    client->println("Accept: */*");

    int contentLength = start_request.length() + end_request.length();
    if (debug) Serial.println("Content-Length: " + String(contentLength));
    client->print("Content-Length: "); client->println(String(contentLength));
    client->println("Content-Type: multipart/form-data; boundary=" + boundary);
    client->println("");

    client->print(start_request);

    if (debug) Serial.print(start_request);

    client->println(end_request);
    if (debug) Serial.print(end_request);

    char c;
    now=millis();
    bool finishedHeaders = false;
    bool currentLineIsBlank = true;
    while (millis()-now<1500) {
      while (client->available()) {
        Serial.println("response");
        char c = client->read();
        responseReceived=true;


        if(!finishedHeaders){
          if (currentLineIsBlank && c == '\n') {
            finishedHeaders = true;
          }
          else {
            headers = headers + c;
          }
        } else {
          body=body+c;
        }

        if (c == '\n') {
          currentLineIsBlank = true;
        }else if (c != '\r') {
          currentLineIsBlank = false;
        }

      }

      if (responseReceived) {
        if (debug) {
          Serial.println();
          Serial.println(body);
          Serial.println();
        }
        break;
      }
    }
  }

  return body;
}

String Pushsafer::buildString(String boundary, String name, String value) {

  String formData;
  formData = formData + "--" + boundary + "\r\n";
  formData = formData + "content-disposition: form-data; name=\"" + name + "\"" + "\r\n";
  formData = formData + "\r\n";
  formData = formData + value + "\r\n";

  return formData;
}
