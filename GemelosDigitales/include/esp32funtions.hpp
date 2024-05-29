
/*mqtt
puerto 1883 conexion sin cifrado
       8883 conexion cifrada
       8083 web socket
       8084 web socket secure
*/

void log(String type, String msd)
{ // info demo
    Serial.println("[" + type + "] " + msd);
}

String platform()
{
    // retorna esp32 siempre y cuando esté en esp32
#ifdef ARDUINO_ESP32_DEV
    return "ESP32";
#endif
}

// DE HEX A STRING
String HexToStr(const unsigned long &h, const byte &l)
{
    String s;
    s = String(h, HEX);
    s.toUpperCase();
    s = ("00000000" + s).substring(s.length() + 8 - l);
    return s;
}

// captura la mac y retorna en el formato especificado
String UniqueID()
{
    char uniqueid[15];
    uint64_t chipid = ESP.getEfuseMac();
    uint16_t chip = (uint16_t)(chipid >> 32);
    sniprintf(uniqueid, 15, "%04X", chip);
    return uniqueid;
}

String deviceId() // ESP328436T8673487C3D
{
    return String(platform()) + HexToStr(ESP.getEfuseMac(), 8) + String(UniqueID()); // de un archivo hexa devuelve string y el otro devuelve la plataforma
}

String extractNumbers(String e)
{
    String inputString = e;
    inputString.toLowerCase();
    String outString = "";
    int len = inputString.length();
    for (size_t i = 0; i < len; i++)
    {
        if (!isDigit((char)inputString[i])) // si es
        {
            outString += (char)inputString[i];
        }
    }
    return outString;
}

// crear un path para los Topicos en MQTT
//  emqx/ESP328436T8673487C3D/# +/# status si esta conectado o no, true o false
String pathMqttTopic(String topic)
{
    return String(String(mqtt_usr) + "/" + String(mqtt_id) + "/" + topic);
}

/*convierte lo que ingresa por parametro a un formato estipulado de char a ip */
uint8_t ip[4];
IPAddress CharToIP(const char *str)
{
    sscanf(str, "%hhu.%hhu.%hhu.%hhu", &ip[0], &ip[1], &ip[2], &ip[3]);
    return IPAddress(ip[0], ip[1], ip[2], ip[3]);
}

// retorno en formato "n.n.n.n" de ip a string
String ipStr(const IPAddress &ip)
{
    String sfn = "";
    for (byte bfn = 0; bfn < 3; bfn++)
    {
        sfn += String((ip >> (8 * bfn)) & 0xFF) + ".";
    }
    sfn += String(((ip >> 8 * 3)) & 0xFF);
    return sfn;
}

/*

String longTimeStr(const time_t &t){
    String s = String(t / SECS_PER_DAY) + ':';
    if (hour(t) < 10)
    {
        s += '0';
    }
    s += String(hour(t)) + ':';
    if (minute(t) < 10)
    {
        s += '0';
    }
    s += String(minute(t)) + ':';
    if (second(t) < 10)
    {
        s += '0';
    }
    s += String(second(t));
    return s;
}
*/

// Retorna la calidad de señal WIFI en % => 0 - 100%
int getRSSIasQuality(int RSSI)
{
    int quality = 0;
    if (RSSI <= -100)
    {
        quality = 0;
    }
    else if (RSSI >= -50)
    {
        quality = 100;
    }
    else
    {
        quality = 2 * (RSSI + 100);
    }
    return quality;
}

// retorna el contendio del body enviado como json metodo post/put
/* size_t :
Dependiendo de la plataforma y del compilador, size_t puede ser un alias de uno de los tipos de datos enteros sin signo
 como unsigned int, unsigned long, o unsigned long long.
 Su tamaño es lo suficientemente grande como para poder
 representar el tamaño de cualquier objeto en la memoria del sistema.*/

String Getbodycontent(uint8_t *data, size_t len)
{
    String content = "";
    for (size_t i = 0; i < len; i++)
    {
        content.concat((char)data[i]);
    }
    return content;
}

//---funcion para reconocer el tipo de dato a guardar en preferences y almacenarlo automaticamente
template <typename T>
void save_dat(const char *key, T value)
{
    SaveMemo.begin("dataSave", false); // inicializando
    if (std::is_same<T, float>::value)
    {
        SaveMemo.putFloat(key, value);
        ////log("INFO preferences", "el valor almacenado de: " + String(key) + ", es: " + String(value));
    }
    else if (std::is_same<T, int>::value)
    {
        SaveMemo.putInt(key, value);
        ////log("INFO preferences", "el valor almacenado de: " + String(key) + ", es: " + String(value));
    }
    else if (std::is_same<T, bool>::value)
    {
        SaveMemo.putBool(key, value);
        ////log("INFO preferences", "el valor almacenado de: " + String(key) + ", es: " + String(value));
    }
    else if (std::is_same<T, std::string>::value)
    {
        SaveMemo.putString(key, value.c_str()); // Convertir std::string a const char*
        ////log("INFO preferences", "el valor almacenado de: " + String(key) + ", es: " + String(value));
    }
    else
    {
        // Otros tipos de datos pueden ser manejados aquí según sea necesario
        ////log("ERROR Preferences", "Unsupported data type");
    }
    SaveMemo.end();
}

//----------Funcion para leer celda de carga y retornar json----//

float temperatureread()
{
    sensorTemperatura.requestTemperatures(); // Actualizar la lectura de los sensores
    float temperatureInCelsius = sensorTemperatura.getTempCByIndex(SENSOR_INDEX);
    //////log("INFO DallasSensor", "La temperatura promedio es: " + String(temperatureInCelsius) + " °c");
    if (temperatureInCelsius != DEVICE_DISCONNECTED_C)
    {
        return temperatureInCelsius;
    }
    else
    {
        return -99.99;
    }
}

String genJsonLc(int idLC, float milivolt, float tempLC, float AlVolt, float peso)
{
    String json = "";
    String id = "";

    if (idLC == 0)
    {
        id = "ScaleRef";
        json += "\"" + id + "\":{";
        json += "\"Peso\":\"" + String(peso, 7) + "\"";
        json += ",\"milivolt\":\"" + String(milivolt, 7) + "\"";
        json += "}";
    }
    else
    {
        id = "IdLoadcell[" + String(idLC - 1) + "]";
        json += "\"" + id + "\":{";
        json += "\"Peso\":\"" + String(peso, 7) + "\"";
        json += ",\"milivolt\":\"" + String(milivolt, 7) + "\"";
        json += ",\"tempLc\":\"" + String(tempLC) + "\"";
        json += ",\"AlVolt\":\"" + String(AlVolt) + "\"";
        json += "}";
    }

    return json;
}

//------------------------------Multiplexor------------------------//
void SelectChannelMux(byte channel)
{
    digitalWrite(S0, bitRead(channel, 0));
    digitalWrite(S1, bitRead(channel, 1));
    digitalWrite(S2, bitRead(channel, 2));
    digitalWrite(S3, bitRead(channel, 3));
}

float Scale_mV(float peso)
{

    float mv = (peso / 8388607.0) * (vref / 128.0) * fact_mult;
    if (peso >= -0.02 && peso <= 0.02)
    {
        mv = 0.01;
    }
    return mv;
}
// listar archivos en la memoria spiffs

void listSPIFFSFiles()
{
    File root = SPIFFS.open("/");
    if (!root)
    {
        Serial.println("Error al abrir el directorio raíz");
        return;
    }
    if (!root.isDirectory())
    {
        Serial.println("No es un directorio");
        return;
    }

    File file = root.openNextFile();
    while (file)
    {
        if (file.isDirectory())
        {
            Serial.print("DIR : ");
            Serial.println(file.name());
        }
        else
        {
            Serial.print("FILE: ");
            Serial.print(file.name());
            Serial.print("\tSIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

// leer archivo en la memoria SPIFFS
void readFile(const char *path)
{
    File file = SPIFFS.open(path);
    if (!file)
    {
        Serial.println("Error al abrir el archivo");
        return;
    }

    Serial.print("Leyendo archivo: ");
    Serial.println(path);

    while (file.available())
    {
        Serial.write(file.read());
    }
    file.close();
}

void calibrate(int idLC, char mode, bool stateLC)
{
    log("INFO CALIBRATION", "Start calibration:");
    log("INFO CALIBRATION", "Place the load cell an a level stable surface.");
    log("INFO CALIBRATION", "Remove any load applied to the load cell.");
    log("INFO CALIBRATION", "Send 't' from serial monitor to set the tare offset.");
    
    switch (mode)
    {
    case '0': // modo de calibracion bascula completa
        if (stateLC)
        {
        }
        break;
    case '1': // modo de celdas unitarias
        if (stateLC)
        {
        }
        break;

    case '2': // modo de celdas por modulo
        if (stateLC)
        {
        }
        break;

    default:
        break;
    }
    // Calibracion modo celdas completas
}