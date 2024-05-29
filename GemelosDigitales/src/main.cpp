#include <Arduino.h>
#include <HX711_ADC.h>
#include <SPIFFS.h> // Almacenamiento de archivo de configuracion
#include <WiFi.h>
#include <DNSServer.h>
#include <ESPmDns.h>
#include <ArduinoJson.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <Preferences.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <INA226_WE.h>
#include <freertos/semphr.h>
#include <vector>
std::vector<int> vecstCal;
std::vector<long> vecfacCal; // vector da factores de calibracion
/*
Archivos de definiciones
*/
#include "esp32_def_var.hpp"
#include "esp32funtions.hpp"
#include "esp32_settings.hpp"
#include "esp32_hx_v5.hpp"
#include "esp32_wifi.hpp"
#include "esp32_mqtt.hpp"
#include "esp32_Webserver.hpp"
#include "esp32_task.hpp"
// put function declarations here:

void setup()
{
  // vecfacCal.resize(lc_num, 0);
  // vecstCal.resize(lc_num, 0);

  pinMode(mqttState, OUTPUT);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  Serial.begin(115200);

  log("INFO", "iniciado el setup");
  xTaskCreate(TskAdmin, "Admintsk", 1024 * 5, NULL, 5, &Taskadmin);
  // conexion mqtt
  // Tarea para lectura de celdas
  // xTaskCreate(mqttreconnect,"mqtttskrn",1024*5,NULL, 2,NULL);

  // Crear el semáforo binario
  if (!SPIFFS.begin(true))
  {
    // log("ERROR SPIFFS", "fallo la inicializacion");
  }
  else
  {
    //readFile("/hx711.json");
    //listSPIFFSFiles();
    // log("INFO SPIFFS", "Iniciada correctamente memoria ");
    if (!hxV_read())
    {
      // log("INFO SETTINGS", "false return");
      hxV_save();
    }
  }
}
void loop()
{
}