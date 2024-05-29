/* //--------------VARIABLES PARA RESET CELDA-------//
int lc_num;     // numerode celdas de carga
bool CalibState;    // estado de calibracion 1 calibrada, 0 no calibrada
long vecfacCal;    // valor de calibracion */

bool hxV_read();
void hxV_reset();
bool hxV_save();

bool hxV_read()
{
    DynamicJsonDocument jsonhx711(cap_hx711);
    File file = SPIFFS.open("/hx711.json", "r"); // abrir archivo en modo de lectura "r"
    DeserializationError error = deserializeJson(jsonhx711, file);
    if (deserializeJson(jsonhx711, file))
    {
        hxV_reset(); // llamar la función que formatea a fabrica                                                                  // Tomar los valores de Fábrica
        DeserializationError error = deserializeJson(jsonhx711, file);
        // log("ERROR","Falló la lectura de las configuraciones, tomando valores por defecto");
        if (error)
        {
            // log("ERROR", String(error.c_str()));
        }
        return false;
    }
    else
    { // 5 celdas de carga son 0,1,2,3,4
        lc_num = jsonhx711["LoadCell"]["LoadCellNumbs"];         // numero de celdas de carga
        contCal = jsonhx711["LoadCell"]["CalibrationCount"];    // cuantas veces se han calibrado
        conf_lc = jsonhx711["LoadCell"]["ConfigurationStatus"];// si las celdas de carga se encuentran configuradas o no

        vecfacCal.resize(lc_num, 0);
        vecstCal.resize(lc_num, 0);
        if (conf_lc == 0)
        {
            for (int i = 0; i < lc_num; i++)
            {
                String id = "loadCell[" + String(i) + "]";
                vecfacCal.push_back(jsonhx711["LoadCell"][id]["CalibrationFactor"]); // recupero el factor de calibracion
                vecstCal.push_back(jsonhx711["LoadCell"][id]["CalibrationStatus"]);
                Serial.printf("%s = con un factor de calibracion = %u y estatus tare = %s \r\n", id, vecfacCal[i], vecstCal[i]);
            }
        }
        log("INFO HX711 SET", "lectura de las configuraciones correctamente");
        file.close();
        return true;
    }
}
void hxV_reset()
{

    conf_lc = 1;
    lc_num = 2;
    vecfacCal.resize(lc_num, 0);
    vecstCal.resize(lc_num, 0);
    // resetear a valores iniciales o de fabrica
}

bool hxV_save()
{
    DynamicJsonDocument jsonhx711(cap_hx711);
    File file = SPIFFS.open("/hx711.json", "w+"); // abrir el archivo en modo escritura
    /* se crea el json con las respectivas estructuras*/
    if (file)
    {
        JsonObject lcobj = jsonhx711.createNestedObject("LoadCell");
        lcobj["LoadCellNumbs"] = lc_num;
        lcobj["CalibrationCount"] = contCal;
        lcobj["ConfigurationStatus"] = conf_lc;
        if (conf_lc == 0)
        {
            for (int i = 0; i < lc_num; i++)
            {
                id = "loadCell[" + String(i) + "]";
                JsonObject lcidobj = lcobj.createNestedObject(id);
                lcidobj["CalibrationFactor"] = vecfacCal[i];
                lcidobj["CalibrationStatus"] = vecstCal[i];
            }
        }
        serializeJsonPretty(jsonhx711, file);
        file.close();
        // log("INFO", "Configuracion guardada correctamente");
        serializeJsonPretty(jsonhx711, Serial); // mostrar en el serial
        return true;
    }
    else
    {
        // log("ERROR", "Falló el guardado de la configuracion");
        return false;
    }
}