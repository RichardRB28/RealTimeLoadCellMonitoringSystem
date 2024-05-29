// DEFINE CLIENTE MQTT SOBRE CONEXION WIFI

#include <PubSubClient.h>
// https://pubsubclient.knolleary.net/api.html
// https://www.amebaiot.com/zh/rtl8722dm-arduino-api-pubsubclient/

// boolean connect (clientID)
// boolean connect (clientID, willTopic, willQoS, willRetain,willMessage)
// boolean connect (clientID, username, password)
// boolean connect (clientID, username, password, willTopic, willQoS, willRetain, willMessage)
// * boolean connect (clientID, username, password, willTopic, willQoS, willRetain, willMessage, cleanSession)

/*
    Parámetros id: ID de cliente, un identificador de cadena único
    usuario: nombre de usuario para autenticación, valor predeterminado NULL
    pass: contraseña para autenticación, valor predeterminado NULL
    willTopic: el tema que utilizará el mensaje
    willQoS: la calidad del servicio que utilizará el mensaje will
    willRetain: si el testam ento debe publicarse con el indicador de retención
    willMessage: la carga del mensaje del testamento
    cleanSession: Si cleansession se establece en true , se eliminarán todos los temas suscritos
*/

WiFiClient wificlient;
PubSubClient mqttclient(wificlient);
char topic[150];
String mqtt_data = "";
long lastmqtt_reconnectattem = 0; // tiempos para intentar reconexion con mqtt
long lastmsg = 0;
// enviar mensaje o publicar
bool mqtt_connect();
void callback(char *topic, byte *payload, unsigned int length);
String json();
void mqtt_publish();
void mqttloop();

// MQTT CONEXION ( Intentar conexion)

bool mqtt_connect()
{
    mqttclient.setServer(mqtt_server, mqtt_port);
    mqttclient.setCallback(callback);
    // maneja las peticiones que llegan del broker
    // log("INFO MQTT", "intentando conexion...");
    String topic_publish = pathMqttTopic("status"); // topico de status de conexion
    topic_publish.toCharArray(mqtt_willTopic, 150); // mensaje de ultima voluntad cuando el dispositivo se desconecta

    if (mqttclient.connect(mqtt_id, mqtt_usr, mqtt_password, mqtt_willTopic, mqtt_willQos, mqtt_willRetain, mqtt_willmessage, mqtt_clean_session))
    { // parametros (client_id, )
        log("INFO MQTT", "conectado al broker");
        mqttclient.setBufferSize(1024 * 5);
        String topic_subscribe = pathMqttTopic("esp32_control"); // topico de subscripcion para recepcion
        topic_subscribe.toCharArray(topic, 150);
        // Nos suscribimos a comandos Topico: v1/device/usuario/dispositivo/comando
        // boolean subscribe (topic)
        // * boolean subscribe (topic, [qos])
        // qos - optional the qos to subscribe at (int: 0 or 1 only)
        // Función para suscribirnos al Topico

        if (mqttclient.subscribe(topic, mqtt_qos)) // funcion bool
        {
            log("INFO MQTT", "Subscrito al topico: " + String(topic));
        }
        else
        {
            log("ERROR MQTT", "falló  subscripcion al topico");
        }
        if (mqtt_status_send)
        { // si esta habilitado el envio de mensajes
            mqttclient.publish(mqtt_willTopic, "{\"connected\": true}", mqtt_retain);
        }
    }
    else
    {
        log("ERROR MQTT", " Codigo error: " + String(mqttclient.state())); // devuelve numero
        return (0);
    }
    return (1);
}

void callback(char *topic, byte *payload, unsigned int length)
{
    String command = ""; // comando recibido desde el broker
    String str_topic(topic);

    for (int16_t i = 0; i < length; i++)
    {
        command += (char)payload[i];
    }
    command.trim(); // limpiar espacios
    log("INFO MQTT", "Topico-->" + str_topic);
    log("INFO MQTT", "Mensaje-->" + command);
}
// Json con informacion del dispositivo para enviar por mqtt

/* String json()
{
    String response; // compactar el json
    DynamicJsonDocument jsondoc(3000);
    // readSensor();

    jsondoc["Device_serial"] = deviceId();
    jsondoc["device_manufacturer"] = String(device_manufacturer);
    jsondoc["device_hw_version"] = String(device_hw_version);
    jsondoc["device_sdk"] = String(ESP.getSdkVersion());

    JsonObject dataobj = jsondoc.createNestedObject("data");

    dataobj["device_rama_vailable"] = ESP.getFreeHeap();
    dataobj["device_ram_size_kb"] = ESP.getHeapSize();
    dataobj["device_spiff_sused"] = String(SPIFFS.usedBytes());
    dataobj["device_cpu_clock"] = String(getCpuFrequencyMhz());
    dataobj["device_flash_size"] = String(ESP.getFlashChipSize() / (1024.0 * 1024), 2);

    dataobj["mqtt_status"] = mqttclient.connected() ? true : false;
    dataobj["mqtt_server"] = mqttclient.connected() ? F(mqtt_server) : F("server not connected");
    dataobj["wifi_status"] = WiFi.status() == WL_CONNECTED ? true : false;
    dataobj["wifi_rssi_status"] = WiFi.status() == WL_CONNECTED ? WiFi.RSSI() : 0;
    dataobj["wifi_quality"] = WiFi.status() == WL_CONNECTED ? getRSSIasQuality(WiFi.RSSI()) : 0;

    /*    dataobj["id_loadcell"] = id_lc;
       dataobj["peso"] = peso;
       dataobj["milivoltaje"] = milivolt;
       dataobj["voltaje_bateria"] = volt_bt;
       dataobj["resistencia"] = resistencia;
       dataobj["lc_temperatura"] = lc_temp;

    serializeJson(jsondoc, response);
    return response;
} */
// mensajes salientes publicados al topico lc_status

void mqtt_publish(String data)
{
    String topic = pathMqttTopic("scaleStatus"); // mensajes directos del dispositivo
    log("INFO MQTT", topic);
    mqttclient.publish(topic.c_str(), data.c_str(), mqtt_retain);
}

void mqttloop()
{
    if (mqtt_enable)
    {
        if (!mqttclient.connected())
        {
            long now = millis();
            digitalWrite(mqttState, LOW);

            if ((now < 6000 || ((now - lastmqtt_reconnectattem) > 12000)))
            {
                lastmqtt_reconnectattem = now;
                digitalWrite(mqttState, HIGH);
                if (mqtt_connect())
                {
                    // log("INFO MQTT", "Conectado al broker");
                    lastmqtt_reconnectattem = 0; // ya conectado
                }
            }
        }
        else
        {
            digitalWrite(mqttState, HIGH);
            mqttclient.loop();
        }
    }
}