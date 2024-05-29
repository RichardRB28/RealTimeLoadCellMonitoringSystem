// archivo para creacion de tareas
void wifitskreconnect(void *pvParameters);
void mqttreconnect(void *pvParameters);
void ReadLoadcells(void *pvParameters);
void TskAdmin(void *pvParameters);

void wifitskreconnect(void *pvParameters)
{
    (void)pvParameters; // por si se envia un parametro recibirlo aqui
    while (1)
    {
        // log("INFO WIFI", "ejecutando wifi task");
        vTaskDelay(10 / portTICK_PERIOD_MS);
        if (wifi_app == WIFI_STA)
        {
            wifi_loop();
        }
        else if (wifi_app == WIFI_AP)
        {
            wifiaploop();
        }
        else
        {
        }
        vTaskDelay(pdMS_TO_TICKS(150));
    }
}
// RECONECTAR AL SERVIDOR MQTT SIEMPRE Y CUANDO ESTÉ CONECTADO AL WIFI Y EN MODO STATION
void mqttreconnect(void *pvParameters)
{
    (void)pvParameters;
    while (1)
    {
        // log("INFO MQTT", "ejecutando MQTT task");
        //  //log("MQTT INFO", "En tarea de reconexion");
        if ((WiFi.status() == WL_CONNECTED) && (wifi_app == WIFI_STA))
        {
            // SE INTENTA RECONEXION
            if (mqtt_server != 0)
            {
                // llamar la funcion loop
                mqttloop();
                // enviar json serializado
                if (mqttclient.connected() && mqtt_status_send)
                {
                    if (ActSend) // millis() - lastmsg > mqtt_time_interval
                    {
                        lastmsg = millis();
                        mqtt_publish(jsontt);
                        log("INFO MQTT", "Mensaje enviado por MQTT...");
                        log("JSON TT", jsontt);
                        ActSend = false;
                    }
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

void ReadLoadcells(void *pvParameters)
{
    (void)pvParameters;
    float tempC = 0.0, peso = 0.00, milivolts = 0.00;
    byte cont_id = 0;
    String json_temp = "{\"Loadcell\": {";
    bool chrVlues = false, time = false, habTm = false, hab_tm2 = false;
    int contPeso = 0, contND = 0;
    float pesoAnt = 0.00;
    bool newDataReady = false;
    long time_actual = 0, tiempo_final = 0, timeActual = 0;
    while (1)
    {
        if (LoadCell.getTareTimeoutFlag() || LoadCell.getSignalTimeoutFlag())
        { // revisar si están los pines conectados correctamente
            Serial.printf("Timeout, check MCU>HX711 wiring and pin designations\n");
        }

        // log("INFO READLOADCELL", "ejecutando readloadcells task");
        const int serialPrintInterval = 0; // increase value to slow down serial print activity
        UBaseType_t stack_left = uxTaskGetStackHighWaterMark(NULL);
        size_t stack_left_bytes = stack_left * sizeof(StackType_t);
        // Serial.println(stack_left_bytes);
        if (conf_lc == 0) // configuraciones de celdas
        {
            // Calibrar celdas
            contCal += 1;
            SelectChannelMux(cont_id); // SELECCIONADOR DE CANALES DEL MUX
            // Calibrar celdas
        }
        else
        {
            SelectChannelMux(cont_id); // SELECCIONADOR DE CANALES DEL MUX

            Serial.printf("la lectura en el canal: %i\n", cont_id);
            if (LoadCell.update())
            {
                newDataReady = true;
            }

            if (newDataReady)
            {
                peso = LoadCell.getData();
                // peso = roundf((roundf(peso * 100) / 100) / 0.5) * 0.5; // Redondeamos a dos decimales
                peso = roundf(peso * 100) / 100;
                Serial.printf("peso es: %f\n", peso);
                milivolts = Scale_mV(peso);
                if (!time) // tiempo de estabilizacion del dato
                {
                    time_actual = millis(); //       523.5698//  523.6 // 5.236  (10.442)    10.5*0.5
                    time = true;
                }
                if (peso == pesoAnt)
                {
                    contPeso += 1;
                    if (contPeso > 5) // esperas estabilidad en lectura y cambias de canal
                    {
                        ActSend = false;
                        chrVlues = true;
                        habTm = true;
                        newDataReady = false;
                        contPeso = 0;

                        if (time)
                        {
                            tiempo_final = (millis() - time_actual) / 1000;
                            Serial.printf("el tiempo en estabilizarse fue:--------------------------------- %ld\n", tiempo_final);
                            time = false;
                        }
                    }
                }
                else
                {
                    contPeso = 0;
                }
            }
            pesoAnt = peso;
            // Serial.printf("peso anterior: %.2f\n", pesoAnt);
            tempC = temperatureread();
            ina226.readAndClearFlags();
            BatteryCurrent_mA = ina226.getCurrent_mA();
            shuntVoltage_mV = ina226.getShuntVoltage_mV();
            loadVoltage_V = BatteryVoltage_V + (shuntVoltage_mV / 1000);
            Serial.printf("voltaje: %f\n", shuntVoltage_mV);
            if (!ina226.overflow)
            {
                BatteryVoltage_V = ina226.getBusVoltage_V();
            }
            else
            {
                BatteryVoltage_V = -99.99;
            }

            if (chrVlues)
            {
                if (cont_id == lc_num) // numero de celdas totales traido desde el json de hx711
                {
                    // Serial.println("generando reporte");
                    json_temp += genJsonLc(cont_id, milivolts, tempC, BatteryVoltage_V, peso);
                    // int tam=json_temp.length();
                    // json_temp[tam-1]=' ';
                    jsontt = json_temp;
                    jsontt += "},\"NumLc\": \"" + String(lc_num) + "\"";
                    jsontt += "}";
                    json_temp = "{\"Loadcell\": {";
                    ActSend = true;
                    cont_id = 0;
                    habTm = false; // toma un nuevo tiempo para esperar y actualizar el valor de la celda 1 vez
                }
                if (cont_id < lc_num && !ActSend)
                {
                    LoadCell.setCalFactor(-296264.41); // vecfacCal[cont_id]
                    json_temp += genJsonLc(cont_id, milivolts, tempC, BatteryVoltage_V, peso) + ",";
                    cont_id += 1;
                    Serial.println("Seteando nuevo valor de calibracion");

                    if (!LoadCell.getTareStatus())
                    {
                        LoadCell.tareNoDelay();
                    }
                    Serial.println(LoadCell.getSPS());
                    // Serial.printf("El tare offset es:%u\n ", LoadCell.getTareOffset());
                }
                if (cont_id > lc_num)
                {
                    json_temp = "{\"Loadcell\": {";
                    String mensaje = "Actualizando numero de celdas: " + String(lc_num);
                    jsontt = "{";
                    jsontt += "\"UploadScale\": \"" + mensaje + "\"";
                    jsontt += "}";
                    ActSend = true;
                    cont_id = 0;
                }
                chrVlues = false;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
void TskAdmin(void *pvParameters)
{
    // Control de flujo de tareas
    (void)pvParameters; // por si se envia un parametro recibirlo aqui
    char cont_st = '1';
    bool habCont = false;
    while (1)
    {
        // log("INFO ADMIN", "ejecutando ADMIN task");
        if (cont_st != '6')
        {
            delay(50);
            digitalWrite(LED_BUILTIN, LOW);
            delay(50);
            digitalWrite(LED_BUILTIN, HIGH);
        }
        else
        {
            digitalWrite(LED_BUILTIN, LOW);
        }
        switch (cont_st)
        {
        case '1':
            // //log("INFO ADMIN", "Iniciando lectura de memoria SPIFFS ");

            if (!SPIFFS.begin(true))
            {
                // log("ERROR SPIFFS", "fallo la inicializacion");
            }
            else
            {
                listSPIFFSFiles();
                // log("INFO SPIFFS", "Iniciada correctamente memoria ");
                if (!settings_read())
                {
                    // log("INFO SETTINGS", "false return");
                    settings_save();
                }
                cont_st = '2';
            }
            break;
        case '2':
            // //log("INFO ADMIN", "Iniciando configuración wifi");
            xTaskCreate(wifitskreconnect, "wifitskrn", 1024 * 5, NULL, 4, &TaskWifi);
            wifi_setup(); // iniciando configuracion wifi
            initserver(); // inicializando server local
            // vTaskSuspend(TaskWifi);
            cont_st = '3';
            break;

        case '3':
            // //log("ADMIN INFO", "Inicializando tarea lecturas Celdas de carga y configuracion de sensores");
            SelectChannelMux(0);               // seleccionando nacal 0 inicio de mux
            LoadCell.setCalFactor(-236099.98); // vecfacCal[cont_id]
            sensorTemperatura.begin();         // sensor de temperatura
            sensorTemperatura.getAddress(sensorDeviceAddress, 0);
            sensorTemperatura.setResolution(sensorDeviceAddress, SENSOR_RESOLUTION);
            Wire.begin(); // inicializando sensor de coriente
            ina226.init();
            cont_st = '4';
            break;
        case '4': // CONFIGURAR HX711 Y REVISAR RESTO DE CONFIGURACIONES REALIZADAS
            LoadCell.begin();
            LoadCell.start(stabilizingtime, _tare);
            // LoadCell.setCalFactor(-236099.98);
            if (!habCont) // habilitar continuar
            {
                if (LoadCell.getTareTimeoutFlag() || LoadCell.getSignalTimeoutFlag())
                { // revisar si están los pines conectados correctamente
                    Serial.printf("Timeout, check MCU>HX711 wiring and pin designations\n");
                }

                else
                {
                    log("INFO ADMIN", "loadcell startup");
                    while (!LoadCell.update())
                        ;

                    xTaskCreate(ReadLoadcells, "lcrd", 1024 * 2, NULL, configMAX_PRIORITIES - 1, &TaskLectLC);
                    cont_st = '5';
                    habCont = true;
                }
            }
            break;
        case '5':
            // //log("INFO ADMIN", "Configuraciones realizadas con exito");
            xTaskCreate(mqttreconnect, "mqtttskrn", 1024 * 5, NULL, 3, &TaskMqtt);
            cont_st = '6';
            break;

        default:
            // log("INFO ADMIN", "NO LOOK");
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}