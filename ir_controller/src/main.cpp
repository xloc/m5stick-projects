#include <M5StickCPlus.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>

#include "irtx.h"

/// this contains `const char*` variables [ssid] and [password] for wifi
#include "secret.h"

auto &lcd = M5.Lcd;
AsyncWebServer server(80);

enum RCCode {
    RC_CODE_ON = 0, 
    RC_CODE_OFF, 
    RC_CODE_DIM, 
    RC_CODE_BRIGHTEN, 
    RC_CODE_WARM, 
    RC_CODE_NATURAL, 
    RC_CODE_MAX
};

struct irtx_code_item
{
    const uint8_t code;
    char const *name;
};

const struct irtx_code_item rc_codes[] = {
    [RC_CODE_ON] = {.code = 0x03, .name = "on"},
    [RC_CODE_OFF] = {.code = 0x02, .name = "off"},
    [RC_CODE_DIM] = {.code = 0x01, .name = "dim"},
    [RC_CODE_BRIGHTEN] = {.code = 0x00, .name = "brighten"},
    [RC_CODE_WARM] = {.code = 0x16, .name = "warm"},
    [RC_CODE_NATURAL] = {.code = 0x10, .name = "natural"},
};


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
        request->send(200, "text/plain", "ESP32 server for controlling LED stripe via IR");
    });

    server.onNotFound([](Req *request){
        request->send(404, "text/plain", "Not found");
    });

    server.on("/on", HTTP_GET, [](Req *request){
        irtx_transmit(rc_codes[RC_CODE_ON].code);
        request->send(200, "text/plain", "OK");
    });

    server.on("/off", HTTP_GET, [](Req *request){
        irtx_transmit(rc_codes[RC_CODE_OFF].code);
        request->send(200, "text/plain", "OK");
    });

    server.on("/dim", HTTP_GET, [](Req *request){
        irtx_transmit(rc_codes[RC_CODE_DIM].code);
        request->send(200, "text/plain", "OK");
    });    

    server.on("/brighten", HTTP_GET, [](Req *request){
        irtx_transmit(rc_codes[RC_CODE_BRIGHTEN].code);
        request->send(200, "text/plain", "OK");
    });

    server.on("/warm", HTTP_GET, [](Req *request){
        irtx_transmit(rc_codes[RC_CODE_WARM].code);
        request->send(200, "text/plain", "OK");
    });

    server.on("/natural", HTTP_GET, [](Req *request){
        irtx_transmit(rc_codes[RC_CODE_NATURAL].code);
        request->send(200, "text/plain", "OK");
    });  


    server.on("/battery", HTTP_GET, [](Req *request){
        auto &axp = M5.Axp;
        float bat_v = axp.GetBatVoltage();
        float bat_i = axp.GetBatCurrent();
        float bat_p = axp.GetBatPower();
        
        static char buffer[100];
        sprintf(buffer, "battery info\n"
            "voltage: %7.2fV\ncurrent: %7.2fmA\npower  : %7.2fmW\n", 
            bat_v, bat_i, bat_p);
        request->send(200, "text/plain", buffer);
    });  

    server.on("/coulomb", HTTP_GET, [](Req *request){
        auto &axp = M5.Axp;
        uint32_t coulomb_charge = axp.GetCoulombchargeData();
        uint32_t coulomb_discharge = axp.GetCoulombdischargeData();
        uint32_t coulomb_data = axp.GetCoulombData();
        uint32_t coulomb_input = axp.GetBatCoulombInput();
        uint32_t coulomb_output = axp.GetBatCoulombOut();

        static char buffer[120];
        sprintf(buffer, "Coulomb info:\n" 
            "charge=%d\ndischarge=%d\ndata:%d\ninput=%d\noutput=%d\n", 
            coulomb_charge, coulomb_discharge, coulomb_data, coulomb_input, coulomb_output);
        request->send(200, "text/plain", buffer);
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

    irtx_init();

    network_init();
}

void loop()
{
    delay(1000);
}
