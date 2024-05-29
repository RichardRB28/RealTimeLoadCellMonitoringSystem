// definicion de pines #define
#define MQTT 2
#define wifi_st 4

#define TEXTIFY(A) #A
#define ESCAPEQUOTE(A) TEXTIFY(A)
String device_fw_version = ESCAPEQUOTE(BUILD_TAG); // Definido en el init

// hardware del fabricante mostrar en visualizacion
#define device_hw_version "Adminv1"
#define device_manufacturer "Bascosta1028"

const size_t cap_settings = 1024 * 5; // 5kb tamaño del Json
const size_t cap_hx711 = 1024 * 5;
//----------configuracion dispositivo--------//
char id_device[30];  // id dispositivo
char nm_device[30];  // nombre dispositivo
int restartno;       // numero de reinicios
char usr_device[15]; // usuario para acceso de servidor
char psw_device[15]; // contraseña del usuario en servidor

//--------------Modo de wifi---------------//
int wifi_app = WIFI_AP; // Modo de wifi

bool wifi_mode;              // uso de modo AP false o cliente true
char wifi_ssid[63];          // nombre de la red wifi a conectar
char wifi_password[63];      // Contraseña de la red wifi a conectar
bool wifi_ip_static;         // uso de ip estatica DHCP true estatica false DHCP
char wifi_ipv4[16];          // Direccion IPV4  estatica
char wifi_gateway[16];       // Direccion gateway
char wifi_subnet[16];        // Direccion ipv4 subred
char wifi_dns_primary[16];   // ipv4 DNS primary
char wifi_dns_secondary[16]; // ipv4 DNS secundary

//-------------modo de wifi----------------//
char ap_ssid[63];     // nombre del SSID modo AP maximo 63
char ap_password[63]; // contraseña del AP minimo 8, NULL red abierta
int ap_chanel;        // canal AP 1-13
int ap_visibility;    // Es visible o no el AP (0-visible, 2- no visible)
int ap_connect;       // numero de conexiones en el AP maximo 6 conexiones

//-----------zona de configuracion MQTT------------//
bool mqtt_enable;          // habilTAR MQTT broker true/ false
char mqtt_server[39];      // servidor del MQTT broker
int mqtt_port;             // puerto servidor mqtt BROKER 1883/8883
bool mqtt_retain;          // Habilitar mensajes retenidos
int mqtt_qos;              // calidad del servicio 0,1,2
char mqtt_id[30];          // cliente ID MQTT Broker
char mqtt_usr[30];         // usuario MQTT Broker
char mqtt_password[39];    // contraseña del MQTT Broker
bool mqtt_clean_session;   // habilitar sesiones limpias
char mqtt_willTopic[150];  // will topic
char mqtt_willmessage[63]; // will mwnsaje
int mqtt_willQos;          // will calidad de servicio
bool mqtt_willRetain;      // will mensaje retenido
bool mqtt_time_send;       // habilitar envio de dato
int mqtt_time_interval;    // tiempo de envio por MQTT en segundos
bool mqtt_status_send;     // habiliatr envio de estados

//----------------CONTADOR DE REINICIOS-------//
#define Start_addres 0
#define Restart_Address Start_addres + sizeof(int)

//---------Otras variables o variables, pines etc-----//
String jsontt = "";
bool ActSend = false;

Preferences SaveMemo;

TaskHandle_t TaskWifi;
TaskHandle_t TaskMqtt;
TaskHandle_t TaskLectLC;
TaskHandle_t Taskadmin;

//-------------Configuracion sensor temperatura-------------------///
#define ONE_WIRE_BUS 13 // pin 13
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensorTemperatura(&oneWire);
#define SENSOR_RESOLUTION 9
#define SENSOR_INDEX 0
DeviceAddress sensorDeviceAddress;
#define LED_BUILTIN 2

//--------------Configuracion multiplexor--------------//

// pines de control
const int S0 = 15;
const int S1 = 4;
const int S2 = 5;
const int S3 = 23;

const int LOADCELL_DOUT_PIN = 18;
const int LOADCELL_SCK_PIN = 19;

HX711_ADC LoadCell(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
unsigned long stabilizingtime = 2000; // preciscion right after power-up can be improved by adding a few seconds of stabilizing time
boolean _tare = true;
int fact_mult = 100000000;
float vref = 5.1;
unsigned long t = 0;

//-------------
byte value = 0;
float conv_peso_ind = 0.0;

#define I2C_ADDRESS 0x40
INA226_WE ina226(I2C_ADDRESS);
float shuntVoltage_mV = 0.0;
float loadVoltage_V = 0.0;
float BatteryVoltage_V = 0.0;
float BatteryCurrent_mA = 0.0;

//--------------Bynary Semaphore-------**
SemaphoreHandle_t binarySemaphore;

//--------------VARIABLES PARA RESET CELDA-------//
int lc_num, // numerode celdas de carga
contCal=0;      // conteo de calibraciones     
int conf_lc; // estado de calibracion
String id="";
const int mqttState = 32;
