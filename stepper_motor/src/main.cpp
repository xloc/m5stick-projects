#include <M5StickCPlus.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>

#include <TMCStepper.h>

/// this contains `const char*` variables [ssid] and [password] for wifi
#include "secret.h"

class TMC2020_Config{
    const size_t PIN_RX = 63;
    const size_t PIN_TX = 40;
    const uint8_t UART_ADDRESS = 0b00;

    const float R_SENSE = 0.11f; 
        // Match to your driver
        // SilentStepStick series use 0.11
        // UltiMachine Einsy and Archim2 boards use 0.2
        // Panucatt BSD2660 uses 0.1
        // Watterott TMC5160 uses 0.075

    Stream &SERIAL_PORT = Serial1;
};

void motor_init() {
    auto &motor_serial = Serial1;
    motor_serial.begin(115200, SERIAL_8N1, 0, 26); 
    // baud, config, rx_pin, tx_pin

    TMC2209Stepper motor(&Serial1, 0.11f, 0b00); 
    // Stream *, R_sense, address

    motor.begin();
    motor.toff(5);                 // Enables driver in software
    // off time setting controls duration of slow decay phase
    // N_clk = 24 + 32 * TOFF
    motor.rms_current(600);        // Set motor RMS current
    motor.microsteps(16);          // Set microsteps to 1/16th

    //driver.en_pwm_mode(true);       // Toggle stealthChop on TMC2130/2160/5130/5160
    //driver.en_spreadCycle(false);   // Toggle spreadCycle on TMC2208/2209/2224
    motor.pwm_autoscale(true);     // Needed for stealthChop

}

#define SERIAL_PORT Serial1 // TMC2208/TMC2224 HardwareSerial port
#define DRIVER_ADDRESS 0b00 // TMC2209 Driver address according to MS1 and MS2

#define R_SENSE 0.11f // Match to your driver
                      // SilentStepStick series use 0.11
                      // UltiMachine Einsy and Archim2 boards use 0.2
                      // Panucatt BSD2660 uses 0.1
                      // Watterott TMC5160 uses 0.075

auto &lcd = M5.Lcd;
AsyncWebServer server(80);


void network_set_routes();
void network_init()
{
    if (!WiFi.config(ip, gateway, subnet))
    {
        Serial.println("STA Failed to configure");
    }

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nWiFi connected.");
    Serial.printf("IP address: %s", WiFi.localIP().toString().c_str());

    network_set_routes();

    server.begin();
}

void network_set_routes() {
    typedef AsyncWebServerRequest Req;

    server.on("/", HTTP_GET, [](Req *request){
        request->send(200, "text/plain", "Hello");
    });
}

void setup()
{
    M5.begin(false); // do not init LCD
    M5.Axp.ScreenBreath(0);

    lcd.setRotation(3);
    lcd.fillScreen(BLACK);
    lcd.setTextPadding(5);
    lcd.setCursor(5, 2);

    network_init();


}

void loop()
{
    delay(1000);
}
