# ESP32_HTTP_Switch

A simple web server for the ESP-S3 Dev Board that allows toggling a physical GPIO pin

## Features
* mDNS - simple local DNS
* TODO: time toggle

## Hardware
* Board: ESP32-S3 or any ESP32 that supports WiFi
* Physical pin: GPIO 15 (or any other given pin) connected directly to a LED or a relay module

## Usage
* Open your web browser an navigate to: 'http://svetla.local' or use the ip address fetched by WiFi.localIP()
