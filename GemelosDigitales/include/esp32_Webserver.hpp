#pragma once
#include "ESPAsyncWebServer.h"
#include <Update.h>

AsyncWebServer server(80);          // inicializar servidor en el puerto 80
AsyncWebSocket ws("/ws");           // incializando web socket
AsyncEventSource events("/events"); // eventos para web socket

size_t content_len;
#define U_PART U_SPIFFS
#include "esp32_api.hpp"
// desarrollo de api
void initserver()
{
    //-------------Metodos GET-----------------//
    // server.on(URL,HTTP_GET, MANAGER);
    server.on("/api/index", HTTP_GET, handleapiIndex); // direccion del index, metodo get
    server.on("/api/conection/wifi", HTTP_GET, handleapiWifi);
    server.on("/api/conection/mqtt", HTTP_GET, handleapiMqtt);      // direccion del index, metodo get
    server.on("/api/device/download", HTTP_GET, handleapidownload); // URL para descargar archivo settings, metodo get
    server.on("/api/Information/StatusLC", HTTP_GET, handleLcStatus); // URL para descargar archivo settings, metodo get

    //------------Metodos POST-------------//
    // server.on(URL,HTPP_POST,,, MANAGER);
    server.on( "/api/conection/wifi", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, handleapiPostWifi);// actualizar wifi
    server.on("/api/conection/mqtt", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, handleapiPostMqtt); // actualizar mqtt
    server.on("/api/device/upload",HTTP_POST,[](AsyncWebServerRequest *request) 
    {//subir archivos de configuracion
            opened = false;// verifica si el archivo esta abierto o no
    },
    // solicitud, nombre archivo,index, data que llega, final indica cuando termino de subir el archivo
    [](AsyncWebServerRequest *request, const String &filename,size_t index,uint8_t *data,size_t len, bool final)
    {
        handleapiUpload(request,filename,index,data,len,final);
    }
        );
server.on("/api/devicen/firmware",HTTP_POST,[](AsyncWebServerRequest *request)
    {
            opened = false;// verifica si el archivo esta abierto o no
    },
    // solicitud, nombre archivo,index, data que llega, final indica cuando termino de subir el archivo
    [](AsyncWebServerRequest *request, const String &filename,size_t index,uint8_t *data,size_t len, bool final)
    {
        handleapiFirmware(request,filename,index,data,len,final);
    }
        );
server.on("/api/perfil/user", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, handleApiPostUser);

DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");// AÑADIENDO CORS AL SERVER
server.begin();
log("API SERVER INFO", "Servidor HTTP inicializado");
}