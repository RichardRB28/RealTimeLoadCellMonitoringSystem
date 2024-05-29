/*En este archivo agrega la funcionalidad de leer, resetear y guardar configuraciones en la memoria SPIFFS*/
// Declaracion de funciones
bool settings_read();
void settings_reset();
bool settings_save();

//\\-----Definicion de funciones------//
bool settings_read()
{
    DynamicJsonDocument jsonSettings(cap_settings);
    File file = SPIFFS.open("/settings.json", "r"); // abrir archivo en modo de lectura "r"
    //log("INFO", "Ingresando al settings");
    DeserializationError error = deserializeJson(jsonSettings, file);
    if(deserializeJson(jsonSettings, file)){
        settings_reset();      // llamar la función que formatea a fabrica                                                                  // Tomar los valores de Fábrica
        DeserializationError error = deserializeJson(jsonSettings, file);
        //log("ERROR","Falló la lectura de las configuraciones, tomando valores por defecto");
        if(error){
            //log("ERROR", String(error.c_str()));
        }
        return false;
    }
    else
    { // Dispositivo settings
        //log("INFO", "deserializacion correcta");
        // strlcpy(variable, dato leido desde json setting, tamaño));
        strlcpy(id_device, jsonSettings["device_id"], sizeof(id_device));
        strlcpy(nm_device, jsonSettings["device_name"], sizeof(nm_device));
        strlcpy(usr_device, jsonSettings["device_user"], sizeof(usr_device));
        strlcpy(psw_device, jsonSettings["device_password"], sizeof(psw_device));
        // wifi cliente settings.json
        /* Aqui se toma de la sesion wifi del json y se almacenan en las variables asignadas*/
        wifi_mode = jsonSettings["wifi"]["wifi_mode"];
        strlcpy(wifi_ssid, jsonSettings["wifi"]["wifi_ssid"], sizeof(wifi_ssid));
        strlcpy(wifi_password, jsonSettings["wifi"]["wifi_password"], sizeof(wifi_password));
        wifi_ip_static = jsonSettings["wifi"]["wifi_ip_status"];
        strlcpy(wifi_ipv4, jsonSettings["wifi"]["wifi_ipv4"], sizeof(wifi_ipv4));
        strlcpy(wifi_subnet, jsonSettings["wifi"]["wifi_subnet"], sizeof(wifi_subnet));
        strlcpy(wifi_gateway, jsonSettings["wifi"]["wifi_gateway"], sizeof(wifi_gateway));
        strlcpy(wifi_dns_primary, jsonSettings["wifi"]["wifi_dns_primary"], sizeof(wifi_dns_primary));
        strlcpy(wifi_dns_secondary, jsonSettings["wifi"]["wifi_dns_secondary"], sizeof(wifi_dns_secondary));
        // wifi AP settings.json
        strlcpy(ap_ssid, jsonSettings["wifi"]["ap_ssid"], sizeof(ap_ssid));
        strlcpy(ap_password, jsonSettings["wifi"]["ap_password"], sizeof(ap_password));
        ap_chanel = jsonSettings["wifi"]["ap_chanel"];
        ap_visibility = jsonSettings["wifi"]["ap_visibility"];
        ap_connect = jsonSettings["wifi"]["ap_connect"];
       
        mqtt_enable = jsonSettings["mqtt"]["mqtt_enable"];
        strlcpy(mqtt_server, jsonSettings["mqtt"]["mqtt_server"], sizeof(mqtt_server));
        mqtt_port = jsonSettings["mqtt"]["mqtt_port"];
        mqtt_retain = jsonSettings["mqtt"]["mqtt_retain"];
        mqtt_qos = jsonSettings["mqtt"]["mqtt_qos"];
        strlcpy(mqtt_id, jsonSettings["mqtt"]["mqtt_id"], sizeof(mqtt_id));
        strlcpy(mqtt_usr, jsonSettings["mqtt"]["mqtt_user"], sizeof(mqtt_usr));
        strlcpy(mqtt_password, jsonSettings["mqtt"]["mqtt_password"], sizeof(mqtt_password));
        mqtt_clean_session = jsonSettings["mqtt"]["mqtt_clean_session"];
        //------------------------------revisado
        strlcpy(mqtt_willTopic, jsonSettings["mqtt"]["mqtt_willTopic"], sizeof(mqtt_willTopic));
        strlcpy(mqtt_willmessage, jsonSettings["mqtt"]["mqtt_willmessage"], sizeof(mqtt_willmessage));
        mqtt_willQos = jsonSettings["mqtt"]["mqtt_willQos"];
        mqtt_willRetain = jsonSettings["mqtt"]["mqtt_willRetain"];
        mqtt_time_send = jsonSettings["mqtt"]["mqtt_time_send"];
        mqtt_time_interval = jsonSettings["mqtt"]["mqtt_time_interval"];
        mqtt_status_send = jsonSettings["mqtt"]["mqtt_status_send"];

        /* // resto de variables
        strlcpy(id_lc, jsonSettings["loadcell"]["id_loadcell"], sizeof(id_lc));
        strlcpy(peso, jsonSettings["loadcell"]["peso"], sizeof(peso));
        strlcpy(milivolt, jsonSettings["loadcell"]["milivoltaje"], sizeof(milivolt));
        strlcpy(volt_bt, jsonSettings["loadcell"]["voltaje_bateria"], sizeof(volt_bt));
        strlcpy(resistencia, jsonSettings["loadcell"]["resistencia"], sizeof(resistencia));
        strlcpy(lc_temp, jsonSettings["loadcell"]["lc_temperatura"], sizeof(lc_temp)); */
        file.close();
        //log("INFO", "lectura de las configuraciones correctamente");
        return true;
    }
}
//----------------------Valores por defecto---------//
void settings_reset()
{
    //log("INFO", "Ingresando a valores por defecto");
    strlcpy(id_device, deviceId().c_str(), sizeof(id_device));
    strlcpy(nm_device, extractNumbers(deviceId()).c_str(), sizeof(nm_device)); // extrae numero y solo deja los caracteres
    strlcpy(usr_device, "admin", sizeof(usr_device));
    strlcpy(psw_device, "admin", sizeof(psw_device));

    //log("INFO", "Ingresando a wifi set");
    // wifi client default
    wifi_mode = true; // inicia en AP o en STA (true STATION )
    strlcpy(wifi_ssid, "BASCOSTALTDA", sizeof(wifi_ssid));
    strlcpy(wifi_password, "Bascosta1608Ag", sizeof(wifi_password));
    wifi_ip_static = true; // dhcp activo y true, fijo
    strlcpy(wifi_ipv4, "192.168.10.62", sizeof(wifi_ipv4));
    strlcpy(wifi_subnet, "255.255.255.0", sizeof(wifi_subnet));
    strlcpy(wifi_gateway, "192.168.10.1", sizeof(wifi_gateway));
    strlcpy(wifi_dns_primary, "192.168.10.1", sizeof(wifi_dns_primary));
    strlcpy(wifi_dns_secondary, "192.168.10.1", sizeof(wifi_dns_secondary));

    // WIFI AP Settings
    strlcpy(ap_ssid, deviceId().c_str(), sizeof(ap_ssid));
    strlcpy(ap_password, "admin1028", sizeof(ap_password));
    ap_chanel = 9;         // MAYOR QUE 8 sino empieza modo abierto
    ap_visibility = false; // false para visible y true oculta (red)
    ap_connect = 4;        // conexiones maximas en la red (8 max)

    // MQTT
    mqtt_enable = true;
    strlcpy(mqtt_server, "192.168.10.21", sizeof(mqtt_server)); // direcion ip local del dispositivo
    mqtt_port = 1883;
    mqtt_retain = false;
    mqtt_qos = 0;
    strlcpy(mqtt_id, deviceId().c_str(), sizeof(mqtt_id));
    strlcpy(mqtt_usr, "emqx", sizeof(mqtt_usr));
    strlcpy(mqtt_password, "public", sizeof(mqtt_password));
    mqtt_clean_session = true;
    strlcpy(mqtt_willTopic, pathMqttTopic("lc_status").c_str(), sizeof(mqtt_willTopic));
    strlcpy(mqtt_willmessage, "{\"connected\"}", sizeof(mqtt_willmessage));
    mqtt_willQos = 0;
    mqtt_willRetain = false;
    mqtt_time_send = true;
    mqtt_time_interval = 0;
    mqtt_status_send = true;

   /*  strlcpy(id_lc, "00", sizeof(id_lc));
    strlcpy(lc_temp, "00", sizeof(lc_temp));
    strlcpy(peso, "00", sizeof(peso));
    strlcpy(milivolt, "0.00", sizeof(milivolt));
    strlcpy(volt_bt, "00", sizeof(volt_bt));
    strlcpy(resistencia, "00", sizeof(resistencia)); */

    // variables bool por defecto
    // temp_status=false;
    //log("INFO", "Se reiniciaron todos los valores por defecto");
}

bool settings_save()
{
    DynamicJsonDocument jsonSettings(cap_settings);
    File file = SPIFFS.open("/settings.json", "w+"); // abrir el archivo en modo escritura
    /* se crea el json con las respectivas estructuras*/
    if (file)
    {
        // para dispositivo en json
        jsonSettings["device_id"] = id_device;
        jsonSettings["device_name"] = nm_device;
        jsonSettings["device_user"] = usr_device;
        jsonSettings["device_password"] = psw_device;

        JsonObject wifiobj = jsonSettings.createNestedObject("wifi");
        wifiobj["wifi_mode"] = wifi_mode;
        wifiobj["wifi_ssid"] = wifi_ssid;
        wifiobj["wifi_password"] = wifi_password;
        wifiobj["wifi_ip_static"] = wifi_ip_static;
        wifiobj["wifi_ipv4"] = wifi_ipv4;
        wifiobj["wifi_subnet"] = wifi_subnet;
        wifiobj["wifi_gateway"] = wifi_gateway;
        wifiobj["wifi_dns_primary"] = wifi_dns_primary;
        wifiobj["wifi_dns_secondary"] = wifi_dns_secondary;

        // wifi AP seetings
        wifiobj["ap_ssid"] = ap_ssid;
        wifiobj["ap_password"] = ap_password;
        wifiobj["ap_chanel"] = ap_chanel;
        wifiobj["ap_visibility"] = ap_visibility;
        wifiobj["ap_connect"] = ap_connect;

        JsonObject mqttobj = jsonSettings.createNestedObject("mqtt");
        mqttobj["mqtt_enable"] = mqtt_enable;
        mqttobj["mqtt_server"] = mqtt_server;
        mqttobj["mqtt_port"] = mqtt_port;
        mqttobj["mqtt_retain"] = mqtt_retain;
        mqttobj["mqtt_qos"] = mqtt_qos;
        mqttobj["mqtt_id"] = mqtt_id;
        mqttobj["mqtt_user"] = mqtt_usr;
        mqttobj["mqtt_password"] = mqtt_password;
        mqttobj["mqtt_clean_session"] = mqtt_clean_session;
        mqttobj["mqtt_willTopic"] = mqtt_willTopic;
        mqttobj["mqtt_willmessage"] = mqtt_willmessage;
        mqttobj["mqtt_willQos"] = mqtt_willQos;
        mqttobj["mqtt_willRetain"] = mqtt_willRetain;
        mqttobj["mqtt_time_send"] = mqtt_time_send;
        mqttobj["mqtt_time_interval"] = mqtt_time_interval;
        mqttobj["mqtt_status_send"] = mqtt_status_send;

        /* JsonObject loadcellobj = jsonSettings.createNestedObject("loadcell");
        loadcellobj["id_loadcell"] = id_lc;
        loadcellobj["peso"] = peso;
        loadcellobj["milivoltaje"] = milivolt;
        loadcellobj["voltaje_bateria"] = volt_bt;
        loadcellobj["resistencia"] = resistencia;
        loadcellobj["lc_temperatura"] = lc_temp; */

        jsonSettings["file_version"] = "1.0.0";
        serializeJsonPretty(jsonSettings, file);
        file.close();
        //log("INFO", "Configuracion guardada correctamente");
        //serializeJsonPretty(jsonSettings, Serial); // mostrar en el serial
        return true;
    }
    else
    {
        //log("ERROR", "Falló el guardado de la configuracion");
        return false;
    }
}