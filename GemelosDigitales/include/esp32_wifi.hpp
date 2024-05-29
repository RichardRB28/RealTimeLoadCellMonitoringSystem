
const byte DNSSERVER_PORT = 53;
DNSServer dnsServer;
IPAddress ap_ipv4(192, 168, 10, 250);
IPAddress ap_subnet(255, 255, 255, 0);

// WiFi.mode(WIFI_STA)      - station mode: conecta a un punto de acceso
// WiFi.mode(WIFI_AP)       - Access Point mode: estación se conecta a la esp32
// WiFi.mode(WIFI_AP_STA)   - Modo hbrido

unsigned long previousmiliswifi = 0;
unsigned long previousmilisap = 0;
unsigned long interval = 30000;
bool wifi_change = false;
const char *esp_hostname = nm_device;
void start_ap();
void startclient();
void wifi_setup();
void wifi_loop();
void wifiaploop();

void start_ap() // access point
{
    log("INFO", "Iniciando modo AP...");
    WiFi.mode(WIFI_AP);
    WiFi.disconnect(true, true);
    vTaskDelay(20);
    WiFi.softAPConfig(ap_ipv4, ap_ipv4, ap_subnet);
    WiFi.hostname(esp_hostname);
    WiFi.softAP(ap_ssid, ap_password, ap_chanel, ap_visibility, ap_connect);
    //log("INFO", "WiFi AP " + String(ap_ssid) + " = IP " + ipStr(WiFi.softAPIP()));
    dnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure);
    dnsServer.start(DNSSERVER_PORT, "*", ap_ipv4);
    wifi_app = WIFI_AP;
}
// Conexion en modo cliente
void startclient()// Modo Station
{
    log("INFO", "Iniciando modo cliente");
    WiFi.mode(WIFI_STA);
    if (wifi_ip_static)
    {
        if (!WiFi.config(CharToIP(wifi_ipv4), CharToIP(wifi_gateway), CharToIP(wifi_subnet), CharToIP(wifi_dns_primary), CharToIP(wifi_dns_secondary)))
        {
            log("ERROR", "Falló la configuracion en modo Estación");
        }
    }
    WiFi.hostname(esp_hostname);
    WiFi.begin(wifi_ssid, wifi_password);
    log("INFO", "conectando al SSID: " + String(wifi_ssid));
    byte b = 0;
    while (WiFi.status() != WL_CONNECTED && b < 60)
    {
        b++;
        log("WARNING", "intentando conexion WIFI...");
        vTaskDelay(250);
        // intermitencia de led
    }
    if (WiFi.status() == WL_CONNECTED)
    {
        log("INFO", "Wifi conectado(" + String(WiFi.RSSI()) + ") dBm IPV4 " + ipStr(WiFi.localIP()));
        // indicador led
        wifi_app = WIFI_STA;
        wifi_change = true;
    }
    else
    {
        log("ERROR", "Wifi no conectado");
        wifi_change = true;
        start_ap();
    }
}

void wifi_setup()
{
    WiFi.disconnect(true);
    if (wifi_mode) // WIFI MODE TRUE INICIA COMO STATTION
    {
        startclient();
        if (WiFi.status() == WL_CONNECTED)
        {
            log("INFO WIFI", "Wifi modo estación");
        }
    }
    else
    {
        start_ap();// iniciar modo 
        log("INFO WIFI", "Wifi en modo AP"); // access point
    }
    if (wifi_app == WIFI_STA || wifi_app == WIFI_AP)
    {
        if (MDNS.begin(esp_hostname))
        {
            MDNS.addService("http", "tcp", 80); // agregamos el servicio http en el puerto 80
        }
    }
}

// funciones para garantizar que esté funcionando en alguno de los dos modos
byte w = 0;
void wifi_loop()
{
    unsigned long current_millis = millis();
    if (WiFi.status() != WL_CONNECTED && (current_millis - previousmiliswifi >= interval))
    {
        w++;
        WiFi.disconnect(true);
        WiFi.reconnect();
        previousmiliswifi = current_millis;
        if (w == 2)
        {
            log("INFO", "Cambiando a modo AP");
            wifi_change = true;
            w = 0;
            start_ap();
        }
        else
        {
            log("WARNING", "SSID: " + String(wifi_ssid) + " desconectado");
        }
    }
}
byte a = 0;
void wifiaploop()
{
    dnsServer.processNextRequest();
    unsigned long currentmillis = millis();
    if ((currentmillis - previousmilisap >= interval) && wifi_change)
    {
        a++;
        previousmilisap = currentmillis;

        if (a == 2)
        {
            log("INFO", "Cambiando a modo Estación");
            wifi_change = false;
            a = 0;
            startclient();
        }
    }
}