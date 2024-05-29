File file;
bool opened=false;
// Manejo de peticiones del servidor
bool security = false;                     // incio con credenciales (true)
const char *dataType = "application/json"; // se puede usar en cualquier otro lado al instanciar en dataType

//-------------Manejador para INDEX-------------------//
void handleapiIndex(AsyncWebServerRequest *request)
{
    if (security)
    {
        if (!request->authenticate(usr_device, psw_device)) // si no se puede autenticar
        {
            return request->requestAuthentication();
        }
    }
    String json = "";
    json += "{";
    json += "\"serial\": \"" + deviceId() + "\"";
    json += ",\"device\":\"" + platform() + "\"";
    // si esta conectado al wifi devuelve
    WiFi.status() == WL_CONNECTED ? json += ",\"wifiQuality\":" + String(getRSSIasQuality(WiFi.RSSI())) : json += ",\"wifiQuality\": 0";
    WiFi.status() == WL_CONNECTED ? json += ",\"wifiStatus\": true" : json += ",\"wifiStatus\": false";
    WiFi.status() == WL_CONNECTED ? json += ",\"rssiStatus\":" + String(WiFi.RSSI()) : json += ",\"rssiStatus\": 0";
    mqttclient.connected() ? json += ",\"mqttStatus\": true" : json += ",\"mqttStatus\": false";

    json += ",\"wifi\":{";
    WiFi.status() == WL_CONNECTED ? json += "\"Estado_WiFi\": \"ONLINE\" " : json += "\"Estado_WiFi\": \"OFFLINE\"";
    wifi_mode == WIFI_STA ? json += ",\"SSID_WiFi\": \"" + String(wifi_ssid) + "\"" : json += ",\"SSID_WiFi\": \"" + String(ap_ssid) + "\"";
    wifi_mode == WIFI_STA ? json += ",\"IPv4_WiFi\": \"" + ipStr(WiFi.localIP()) + "\"" : json += ",\"IPv4_WiFi\": \"" + ipStr(WiFi.softAPIP()) + "\"";
    json += ",\"MAC_WiFi\": \"" + String(WiFi.macAddress()) + "\"";
    json += "},";

    json += "\"mqtt\":{";
    mqttclient.connected() ? json += "\"Estado_MQTT\": \"ONLINE\" " : json += "\"Estado_MQTT\": \"OFFLINE\"";
    mqttclient.connected() ? json += ",\"Servidor_MQTT\": \"" + String(mqtt_server) + "\"" : json += ",\"Servidor_MQTT\": \"server not connected\"";
    json += ",\"Usuario_MQTT\": \"" + String(mqtt_usr) + "\"";
    json += ",\"Cliente_ID_MQTT\": \"" + String(mqtt_id) + "\"";
    json += "},";

    json += "\"info\":{";
    json += "\"Identificación\": \"" + String(nm_device) + "\"";
    json += ",\"Número_de_Serie\": \"" + deviceId() + "\"";
    json += ",\"mDNS_Url\":  \"" + String("http://" + String(nm_device) + ".local/") + "\"";
    json += ",\"Dirección_IP_del_Cliente\": \"" + ipStr(request->client()->remoteIP()) + "\"";
    json += ",\"Navegador_del_Cliente\": \"" + String(request->getHeader("User-Agent")->value().c_str()) + "\"";
    json += ",\"Versión_del_Firmware\": \"" + String(device_fw_version) + "\"";
    json += ",\"Versión_del_Hardware\": \"" + String(device_hw_version) + "\"";
    json += ",\"CPU_FREQ_MHz\":" + String(getCpuFrequencyMhz());
    json += ",\"RAM_SIZE_KB\":" + String(ESP.getHeapSize() / 1024);
    json += ",\"SPIFFS_SIZE_KB\":" + String(SPIFFS.totalBytes() / 1024);
    json += ",\"FLASH_SIZE_MB\":" + String(ESP.getFlashChipSize() / (1024.0 * 1024), 2);
    json += ",\"Fabricante\": \"" + String(device_manufacturer) + "\"";
    json += "}";

    json += ",\"spiffsUsed\":" + String(SPIFFS.usedBytes() / 1024);
    json += ",\"ramAvailable\":" + String(ESP.getFreeHeap() / 1024);
    // AQUI ANEXAR PARTE DEL JSON PARA CELDAS DE CARGA
    json += "}";
    request->addInterestingHeader("API ESP32 Server");
    request->send(200, dataType, json); // mensaje de estado 200 get
}
//-------------Manejador para get Wifi-------------------//
void handleapiWifi(AsyncWebServerRequest *request)
{
    if (security)
    {
        if (!request->authenticate(usr_device, psw_device)) // si no se puede autenticar
        {
            return request->requestAuthentication();
        }
    }
    String json = "";
    json += "{";
    WiFi.status() == WL_CONNECTED ? json += "\"wifiQuality\":" + String(getRSSIasQuality(WiFi.RSSI())) : json += ",\"wifiQuality\": 0";
    WiFi.status() == WL_CONNECTED ? json += ",\"wifiStatus\": true" : json += ",\"wifiStatus\": false";
    WiFi.status() == WL_CONNECTED ? json += ",\"rssiStatus\":" + String(WiFi.RSSI()) : json += ",\"rssiStatus\": 0";
    mqttclient.connected() ? json += ",\"mqttStatus\": true" : json += ",\"mqttStatus\": false";

    json += ",\"wifi\":";
    json += "{";
    wifi_mode ? json += "\"wifi_mode\": true" : json += "\"wifi_mode\": false";
    json += ",\"wifi_ssid\": \"" + String(wifi_ssid) + "\"";
    json += ",\"wifi_password\": \""
            "\"";
    wifi_ip_static ? json += ",\"wifi_ip_static\": true" : json += ",\"wifi_ip_static\": false";
    json += ",\"wifi_ipv4\": \"" + String(wifi_ipv4) + "\"";
    json += ",\"wifi_gateway\": \"" + String(wifi_gateway) + "\"";
    json += ",\"wifi_subnet\": \"" + String(wifi_subnet) + "\"";
    json += ",\"wifi_dns_primary\": \"" + String(wifi_dns_primary) + "\"";
    json += ",\"wifi_dns_secondary\": \"" + String(wifi_dns_secondary) + "\"";
    json += ",\"ap_ssid\": \"" + String(ap_ssid) + "\"";
    json += ",\"ap_password\": \""
            "\"";
    json += ",\"ap_chanel\":" + String(ap_chanel);
    ap_visibility ? json += ",\"ap_visibility\": false" : json += ",\"ap_visibility\": true";
    json += "},";
    json += "\"ap_connect\":" + String(ap_connect);
    json += "}";
    request->addInterestingHeader("API ESP32 Server");
    request->send(200, dataType, json); // mensaje de estado 200 get, responde codigo 200 con tipo de dato json y adjunta el json
}

//---------------------Manejador para post wifi actualizacion de datos del setting --------//
void handleapiPostWifi(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    /*
    Recibe:
    -peticion
    -data
    tamaño
    -index
    -total
    */
    if (security)
    {
        if (!request->authenticate(usr_device, psw_device)) // si no se puede autenticar
        {
            return request->requestAuthentication();
        }
    }
    String bodycontent = Getbodycontent(data, len);
    StaticJsonDocument<768> doc; // dependiendo el tamaño del json
    DeserializationError error = deserializeJson(doc, bodycontent);
    if (error)
    {
        request->send(400, dataType, "{ \"status\": \"Error de JSON enviado\" }"); // code:400 BAD_REQUEST
        return;
    };
    //------- WIFI Cliente settings.json
    String s = "";
    //APP CLIENT
    wifi_mode = doc["wifi"]["wifi_mode"].as<bool>(); // as<bool>() convertie a boolean
    if (doc["wifi"]["wifi_ssid"] != "")
    {
        s = doc["wifi"]["wifi_ssid"].as<String>();
        s.trim();// limpia caracteres al inicio y final
        strlcpy(wifi_ssid, s.c_str(), sizeof(wifi_ssid));
        s = "";
    }
    // Password SSID Client
    if (doc["wifi"]["wifi_password"] != ""){
        s = doc["wifi"]["wifi_password"].as<String>();
        s.trim();
        strlcpy(wifi_password, s.c_str(), sizeof(wifi_password));
        s = "";
    }
    // DHCP
    wifi_ip_static = doc["wifi"]["wifi_ip_static"].as<bool>();
    
    // IPV4
    if (doc["wifi"]["wifi_ipv4"] != ""){
        s = doc["wifi"]["wifi_ipv4"].as<String>();
        s.trim();
        strlcpy(wifi_ipv4, s.c_str(), sizeof(wifi_ipv4));
        s = "";
    }
    // Gateway
    if (doc["wifi"]["wifi_gateway"] != ""){
        s = doc["wifi"]["wifi_gateway"].as<String>();
        s.trim();
        strlcpy(wifi_gateway, s.c_str(), sizeof(wifi_gateway));
        s = "";
    } 
    // Subned
    if (doc["wifi"]["wifi_subnet"] != ""){
        s = doc["wifi"]["wifi_subnet"].as<String>();
        s.trim();
        strlcpy(wifi_subnet, s.c_str(), sizeof(wifi_subnet));
        s = "";
    }      
    // DNS Primary
    if (doc["wifi"]["wifi_dns_primary"] != ""){
        s = doc["wifi"]["wifi_dns_primary"].as<String>();
        s.trim();
        strlcpy(wifi_dns_primary, s.c_str(), sizeof(wifi_dns_primary));
        s = "";
    }
    // DNS Secondary
    if (doc["wifi"]["wifi_dns_secondary"] != ""){
        s = doc["wifi"]["wifi_dns_secondary"].as<String>();
        s.trim();
        strlcpy(wifi_dns_secondary, s.c_str(), sizeof(wifi_dns_secondary));
        s = "";
    }

    //----------WIFI AP settings
    if (doc["wifi"]["ap_ssid"] != ""){
        s = doc["wifi"]["ap_ssid"].as<String>();
        s.trim();
        strlcpy(ap_ssid, s.c_str(), sizeof(ap_ssid));
        s = "";
    }   
    // AP Password
    if (doc["wifi"]["ap_password"] != ""){
        s = doc["wifi"]["ap_password"].as<String>();
        s.trim();
        strlcpy(ap_password, s.c_str(), sizeof(ap_password));
        s = "";
    }
    // AP visibility 0 Visible - 1 Oculto
    ap_visibility = !doc["wifi"]["ap_visibility"].as<int>(); 
    
    // AP Channel
    if (doc["wifi"]["ap_chanel"] != ""){
        ap_chanel = doc["wifi"]["ap_chanel"].as<int>();
    }
     // AP number of Connections
    if (doc["ap_connect"] != ""){
        ap_connect = doc["wifi"]["ap_connect"].as<int>();
    }
    // Save Settings.json
    if (settings_save()){
        request->addInterestingHeader("API ESP32 Server");
        request->send(200, dataType, "{ \"save\": true }");
    }else{
        request->addInterestingHeader("API ESP32 Server");
        request->send(500, dataType, "{ \"save\": false }");
    }
}

//---------------------Manejador para get mqtt----------------
void handleapiMqtt(AsyncWebServerRequest *request){    
    if (security){
        if (!request->authenticate(usr_device, psw_device))
            return request->requestAuthentication();
    } 
    String json = "";
    json = "{";
    json += "\"serial\": \"" + deviceId() + "\"";
    json += ",\"device\": \"" + platform() + "\"";
    WiFi.status() == WL_CONNECTED ? json += ",\"wifiQuality\":" + String(getRSSIasQuality(WiFi.RSSI())) : json += ",\"wifiQuality\": 0";
    WiFi.status() == WL_CONNECTED ? json += ",\"wifiStatus\": true" : json += ",\"wifiStatus\": false";
    WiFi.status() == WL_CONNECTED ? json += ",\"rssiStatus\":" + String(WiFi.RSSI()) : json += ",\"rssiStatus\": 0";
    mqttclient.connected() ? json += ",\"mqttStatus\": true" : json += ",\"mqttStatus\": false";
    json += ",\"mqtt\":";
    json += "{";
        mqtt_enable ? json += "\"mqtt_enable\": true" : json += "\"mqtt_enable\": false";
        json += ",\"mqtt_server\": \"" + String(mqtt_server) + "\"";
        json += ",\"mqtt_port\":" + String(mqtt_port);
        mqtt_retain ? json += ",\"mqtt_retain\": true" : json += ",\"mqtt_retain\": false";
        json += ",\"mqtt_qos\":" + String(mqtt_qos);
        json += ",\"mqtt_id\": \"" + String(mqtt_id) + "\"";
        json += ",\"mqtt_user\": \"" + String(mqtt_usr) + "\"";
        json += ",\"mqtt_password\": \"""\"";
        mqtt_clean_session ? json += ",\"mqtt_clean_session\": true" : json += ",\"mqtt_clean_session\": false";
        mqtt_time_send ? json += ",\"mqtt_time_send\": true" : json += ",\"mqtt_time_send\": false";
        json += ",\"mqtt_time_interval\":" + String(mqtt_time_interval / 1000); // 60000 / 1000 = 30s
        mqtt_status_send ? json += ",\"mqtt_status_send\": true" : json += ",\"mqtt_status_send\": false";
    json += "},";
    json += "\"code\": 1 ";
    json += "}";
    request->addInterestingHeader("API ESP32 Server");
    request->send(200, dataType, json);
}

//--------------------Manejador para post actualizacion de datos mqtt del settings-------//
void handleapiPostMqtt(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
   if (security)
    {
        if (!request->authenticate(usr_device, psw_device)) // si no se puede autenticar
        {
            return request->requestAuthentication();
        }
    }  
    String bodyContent = Getbodycontent(data, len);
    StaticJsonDocument<768> doc;
    DeserializationError error = deserializeJson(doc, bodyContent);
    if (error){
        request->send(400, dataType, "{ \"status\": \"Error de JSON enviado\" }");
        return;
    };
    //--------MQTT Conexión settings.json------//
    String s = "";
    // MQTT Enable
    mqtt_enable = doc["mqtt"]["mqtt_enable"].as<bool>();
  
    // MQTT Server
    if (doc["mqtt"]["mqtt_server"] != ""){
        s = doc["mqtt"]["mqtt_server"].as<String>();
        s.trim();
        strlcpy(mqtt_server, s.c_str(), sizeof(mqtt_server));
        s = "";
    }
    // MQTT Port
   /*  if (doc["mqtt"]["mqtt_port"] != ""){
        mqtt_port = doc["mqtt_port"].as<int>();
    } */

     mqtt_port=doc["mqtt"]["mqtt_port"].as<int>();
    // MQTT Retain
    mqtt_retain = doc["mqtt"]["mqtt_retain"].as<bool>();
    
    lc_num=doc["mqtt"]["loadcell"].as<int>();
    // MQTT QOS 0-1-2
    mqtt_qos = doc["mqtt"]["mqtt_qos"].as<int>();
    
    // MQTT ID
    if (doc["mqtt"]["mqtt_id"] != ""){
        s = doc["mqtt"]["mqtt_id"].as<String>();
        s.trim();
        strlcpy(mqtt_id, s.c_str(), sizeof(mqtt_id));
        s = "";
    }
    
    // MQTT User
    if (doc["mqtt"]["mqtt_user"] != ""){
        s = doc["mqtt"]["mqtt_user"].as<String>();
        s.trim();
        strlcpy(mqtt_usr, s.c_str(), sizeof(mqtt_usr));
        s = "";
    }
    
    // MQTT Password
    if (doc["mqtt"]["mqtt_password"] != ""){
        s = doc["mqtt"]["mqtt_password"].as<String>();
        s.trim();
        strlcpy(mqtt_password, s.c_str(), sizeof(mqtt_password));
        s = "";
    }

    // mqtt_clean_sessions
    mqtt_clean_session = doc["mqtt_clean_session"].as<bool>(); 
  
    // MQTT Envio de datos settings.json
     // mqtt_time_send
    mqtt_time_send = doc["mqtt"]["mqtt_time_send"].as<bool>();
    // mqtt_time_interval
    mqtt_time_interval = doc["mqtt"]["mqtt_time_interval"].as<int>() * 1000; // 60 * 1000 = 60000 = 60s
    // mqtt_status_send
    mqtt_status_send = doc["mqtt"]["mqtt_status_send"].as<bool>();
    // Save Settings.json
    if (settings_save()){
        request->addInterestingHeader("API ESP32 Server");
        request->send(200, dataType, "{ \"save\": true }");// responder si se //logró hacer o no
    }else{
        request->addInterestingHeader("API ESP32 Server");
        request->send(500, dataType, "{ \"save\": false }");
    }
}

// -------------------Manejador para get para obtener archivo settings de configuracion---------//
void handleapidownload(AsyncWebServerRequest *request){
     if (security)
    {
        if (!request->authenticate(usr_device, psw_device)) // si no se puede autenticar
        {
            return request->requestAuthentication();
        }
    }
    //log("API SERVER INFO","Descarga de archivo settings.json");
    //realiza un response con, cargar desde SPIFFS el archivo seetings.json con el formato datatype y true es descargable
    AsyncWebServerResponse *response=request->beginResponse(SPIFFS,"/settings.json",dataType,true);
    request->send(response);// responde con el archivo
}
// -------------------Manejador para post para subir archivo settings de configuracion---------//
void handleapiUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){
     if (security){
        if (!request->authenticate(usr_device, psw_device))
            return request->requestAuthentication();
    } 

    if (!index){
        Serial.printf("API SERVER INFO", "Upload Start: %s\n", filename.c_str());
    }
    // Validar el archivo si esta abierto settings.json
    if (opened == false){
        opened = true;
        file = SPIFFS.open(String("/") + filename, FILE_WRITE);
        if (!file){
            //log("API SERVER ERROR", "No se pudo abrir el archivo para escribir");
            request->send(500, dataType, "{ \"save\": false, \"msg\": \"¡Error, No se pudo abrir el archivo para escribir!\"}");
            return;
        }
    }
    // Escribir el archivo en la memoria, Si no //logra escribir en el archivo enton ces ingresa al condicional que retorna error al server
    if (file.write(data, len) != len){
        //log("API SERVER ERROR", "No se pudo escribir el archivo");
        request->send(500, dataType, "{ \"save\": false, \"msg\": \"¡Error, No se pudo escribir el archivo: " + filename + " !\"}");
        return;
    }

    // Finalizada la carga del archivo.
    if (final){
        AsyncWebServerResponse *response = request->beginResponse(201, dataType, "{ \"save\": true, \"msg\": \"¡Carga del archivo: " + filename + " completada!\"}");
        response->addHeader("Cache-Control", "no-cache");
        response->addHeader("Location", "/");
        request->send(response);
        file.close();
        opened = false;
        //log("API SERVER INFO", "Carga del archivo " + filename + " completada");
        // Esperar la transmisión de los datos seriales
        Serial.flush();
        // Reiniciar el ESP32
        ESP.restart();
    }
}

//------------------Manejador para post subir archivo de actualizacion de firmware y filesystem------------//
void handleapiFirmware(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){
     if (security){
        if (!request->authenticate(usr_device, psw_device))
            return request->requestAuthentication();
    } 
    // Si el nombre de archivo incluye ( spiffs ), actualiza la partición de spiffs
    int cmd = (filename.indexOf("spiffs") > -1) ? U_PART : U_FLASH;
    String updateSystem = cmd == U_PART ? "FileSystem" : "Firmware";// ambas pueden ser subidas a la esp32

    if (!index){
        content_len = request->contentLength();
        //log("API SERVER INFO", "Actualización del " + updateSystem + " iniciada...");
        if (!Update.begin(UPDATE_SIZE_UNKNOWN, cmd)){
            AsyncWebServerResponse *response = request->beginResponse(500, dataType, "{ \"save\": false, \"msg\": \"¡Error, No se pudo actualizar el " + updateSystem + " Index: " + filename + " !\"}");
            request->send(response);
            Update.printError(Serial);
            //log("API SERVER ERROR", "Update del " + updateSystem + " terminado");
        }
    }
    // escribir e firmware o el filesystem
    if (Update.write(data, len) != len){
        Update.printError(Serial);
    }
    // Terminado
    if (final){
        if (!Update.end(true)){
            AsyncWebServerResponse *response = request->beginResponse(500, dataType, "{ \"save\": false, \"msg\": \"¡Error, No se pudo actualizar el " + updateSystem + " Final: " + filename + " !\"}");
            request->send(response);
            Update.printError(Serial);
        }else{
            AsyncWebServerResponse *response = request->beginResponse(201, dataType, "{ \"save\": true, \"type\": \"" + updateSystem + "\"}");
            response->addHeader("Cache-Control", "no-cache");
            response->addHeader("Location", "root@" + updateSystem + "");
            request->send(response);
            //log("API SERVER INFO", "Update del " + updateSystem + " completado");
            // Esperar la Transmisión de los datos seriales
            Serial.flush();
            ESP.restart();
        }
    }
}

//-------------------Manjeador post para configuraciones de acceso al servidor--------------------------------------//
void handleApiPostUser(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
    if (security){
        if (!request->authenticate(usr_device, psw_device))
            return request->requestAuthentication();
    }

    String bodyContent = Getbodycontent(data, len);
    StaticJsonDocument<384> doc;
    DeserializationError error = deserializeJson(doc, bodyContent);
    if (error){
        request->send(400, dataType, "{ \"status\": \"Error de JSON enviado\" }");
        return;
    };

    // -------------------------------------------------------------------
    // Contraseña settings.json
    // -------------------------------------------------------------------
    String p, np, cp;
    // capturamos las variables enviadas en el JSON
    p = doc["device_old_password"].as<String>();    // Contraseña Actual
    np = doc["device_new_password"].as<String>();   // Nueva Contraseña
    cp = doc["device_c_new_password"].as<String>(); // Confirmación de nueva Contraseña
    // Limpiamos de espacios vacios
    p.trim();
    np.trim();
    cp.trim();
    // Validar que los datos de lacontraseña anterior no este en blanco
    if(p != ""){
        // validar que la contraseña coincida con la anterior
        if( p == psw_device ){
            if(np == "" && cp == ""){
                request->send(400, dataType, "{ \"save\": false, \"msg\": \"¡Error, No se permite los datos de la nueva contraseña y confirmación vacíos!\"}");// 400 bad request
                return;
            } else if(np != "" && cp != "" && np == cp){
                if(np == psw_device){
                    request->send(400, dataType, "{ \"save\": false, \"msg\": \"¡Error, La contraseña nueva no puede ser igual a la anterior\"}");
                    return;
                }
                strlcpy(psw_device, np.c_str(), sizeof(psw_device));
                if (settings_save()) {
                    request->send(200, dataType, "{ \"save\": true, \"msg\": \"¡Contraseña actualizada correctamente!\" }");
                } else {
                    request->send(500, dataType, "{ \"save\": false, \"msg\": \"¡ Internar server error !\" }");
                }
            }else if( np !=  cp ){
                request->send(400, dataType, "{ \"save\": false, \"msg\": \"¡Error, La nueva contraseña y confirmación de nueva contraseña no coinciden!\"}");
                return;
            }else{}
        }else{
           AsyncWebServerResponse *response = request->beginResponse(400, dataType, "{ \"save\": false, \"msg\": \"¡Error, No se pudo guardar, la contraseña anterior no coincide\"}");
            request->send(response);
            return;
        }
    }else{
        request->send(400, dataType, "{ \"save\": false, \"msg\": \"¡Error, No se permite contraseña anterior en blanco\"}");
        return;
    }
}

void handleLcStatus(AsyncWebServerRequest *request){
      if (security){
        if (!request->authenticate(usr_device, psw_device))
            return request->requestAuthentication();
    }
         request->addInterestingHeader("API ESP32 Server");
         request->send(200, dataType, jsontt);
}