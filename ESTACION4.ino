#include <Wire.h>     // libreria para bus I2C
#include <Adafruit_GFX.h>   // libreria para pantallas graficas
#include <Adafruit_SSD1306.h>   // libreria para controlador SSD1306
#include <Adafruit_ADS1X15.h>
#include <SFE_BMP180.h>
#include <sps30.h>
#include "DHT.h"
#include <SPI.h>    // incluye libreria interfaz SPI
#include <SD.h>     // incluye libreria para tarjetas SD
#include "RTClib.h"
#include "WiFi.h"
#include <PubSubClient.h>
#include "SparkFun_SCD30_Arduino_Library.h"


#define DHTPIN 4 // pin  4 para de DHT11
#define DHTTYPE DHT11
#define ALTITUDE 1655.0         // Altitude of SparkFun's HQ in Boulder, CO. in meters
#define SSpin 5                 // Slave Select en pin digital 10
#define LEDC_CHANNEL_0     0
#define LEDC_TIMER_13_BIT  13
#define LEDC_BASE_FREQ     5000
#define LED_PIN            17   //Regulador de voltaje Buck
#define ANCHO 128               // Ancho de pantalla oled 128
#define ALTO 64                 // Alto de pantalla oled 64
#define OLED_RESET 100          // necesario por la libreria pero no usado


DHT dht(DHTPIN, DHTTYPE);
SFE_BMP180 bmp180;
SCD30 airSensor;
RTC_DS1307 rtc;
Adafruit_ADS1115 ads;
File archivo;      // objeto archivo del tipo File para datalooger
File credenciales;  // objeto tipo archivo para conectarse a wifi
Adafruit_SSD1306 oled(ANCHO, ALTO, &Wire, OLED_RESET);  // crea objeto
WiFiClient espClient;
PubSubClient client(espClient);
//**************************************
//*********** MQTT CONFIG **************
//**************************************
const char *mqtt_server = "ioticos.org";
const int mqtt_port = 1883;
const char *mqtt_user = "sXoRKv35L5o6m7x";
const char *mqtt_pass = "2Ho48GIl2qeFc6e";
char *root_topic_subscribe = "XzblrWRu1CApIWw/Dispositivo4";
char *root_topic_publish = "XzblrWRu1CApIWw/Dispositivo4";

//**************************************
//*********** WIFICONFIG ***************
//**************************************

char msg[25];


char daysOfTheWeek[7][12] = {"Domingo", "Lunes", "Martes", "Miercoles", "Jueves", "Viernes", "Sabado"};
int estado = 0;
const float S = 0.1875e-3;
unsigned long time1 = 0;
unsigned long time2 = 0;
unsigned long time3 = 0;
unsigned long time4 = 0;
bool s1 = false;
bool s2 = false;
bool s3 = false;
bool s4 = false;
bool enviar = false;
bool conecto = false;
byte datos = 0;
// Variables sensor matrial particulado
float PM1 = 0.0;
float PM2 = 0.0;
float PM10 = 0.0;
float Temp = 0;

// Variables sensor MQ-4 CH4 Gas Natural
float MQ4;
int gas_sensor_4 = 1; // pin ADC
float RL_4 = 16700;     // Resistencia de carga en Khom, recomendada 20k (10k a 47k)
float RO_4 = -36725.53;   // Resistecia de calibracion hom


// Variables variables sensor MQ-7 CO Monoxido de Carbono
float MQ7;
int gas_sensor_7 = 0;  // pin ADC
float RL_7 = 20500;      // Resistencia de carga en Khom, recomendada 10k (5k a 47k)
float RO_7 = 89134.83;    // Resistecia de calibracion hom


// Variables sensor SCD30 CO2 Dioxido de Carbono


// Variables sensor MQ-131 O3 Ozono
float MQ131;
int gas_sensor_131 = 3;  // Pin ADC
float RL_131 = 191;      // Resistencia de carga en Khom, recomendada 100k (100k a 200k)
float RO_131 = 392.59;    // Resistencia de calibracion en hom

// variables para sensores mq
float muestras = 0;
float V1, V2, V3, V4, acum1, acum2, acum3, acum4;
float Prom_mq7, Prom_mq4, Prom_mq131, CO2;

// Variables para sensor ML8511 UV mW/m2
float ML8511 = 0, Prom_ML8511 , V5, acum5;
int gas_sensor_8511 = 33;
// Variables para sensor humedad dht11
float h = 0;
float t = 0;
// Variables para sensor barometro bmp180
double T, P;

void callback(char* topic, byte* payload, unsigned int length);
void reconnect();
void setup_wifi();

void setup_wifi() {
  delay(10);
  // Leemos archvio wifi.txt y estraemos credenciales
  int dato = 0;
  char letra ;
  String nombre;
  String clave;
  credenciales = SD.open("/wifi.txt");
  if (credenciales) {
    if (credenciales.available()) {
      while (dato != 10 ) {
        letra = credenciales.read();
        dato = int (letra);
        if (dato != 10) {
          nombre = nombre + String(letra);

        }
      }
      while ( credenciales.available()) {
        letra = credenciales.read();
        clave = clave + letra;
      }
    }

  }
  credenciales.close();
  byte lend1 = nombre.length() + 1;
  byte lend2 = clave.length() + 1;
  char ssid [lend1];
  char password[lend2];
  nombre.toCharArray(ssid, lend1);
  clave.toCharArray(password, lend2);
  Serial.println();
  Serial.print("Conectando a ssid: ");
  Serial.println(ssid);
  oled.clearDisplay();
  oled.setCursor(0, 0);
  oled.print("Conectando a  ");
  oled.print(ssid);
  oled.display();

  WiFi.begin(ssid, password);

  int intentos = 0;
  while ((WiFi.status() != WL_CONNECTED) && intentos <= 20) {
    delay(500);
    Serial.print(".");
    intentos++;
  }

  Serial.println("");
  Serial.println("Conectado a red WiFi!");
  Serial.println("Dirección IP: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  byte intento = 3;
  byte veces = 0; 
  setup_wifi();
  while (!client.connected() && veces <= intento) {

   
    Serial.print("Intentando conexión Mqtt...");
    // Creamos un cliente ID
    String clientId = "MEDIDOR_GASES10";
    clientId += String(random(0xffff), HEX);
    // Intentamos conectar
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_pass)) {
      Serial.println("Conectado!");
      // Nos suscribimos
      if (client.subscribe(root_topic_subscribe)) {
        Serial.println("Suscripcion ok");
        oled.clearDisplay();
        oled.setCursor(0, 0);
        oled.println("MQTT suscrito");
        oled.display();
        delay(1000);
      } else {
        oled.clearDisplay();
        oled.setCursor(0, 0);
        oled.println("Falla en MQTT  No suscribe");
        oled.display();
        Serial.println("fallo Suscripciión");
      }
    } else {
      Serial.print("falló :( con error -> ");
      Serial.print(client.state());
      Serial.println(" Intentamos de nuevo en 5 segundos");
      delay(5000);
    }
    veces++;
  }
}
//*****************************
//***       CALLBACK        ***
//*****************************

void callback(char* topic, byte * payload, unsigned int length) {
  String incoming = "";
  Serial.print("Mensaje recibido desde -> ");
  Serial.print(topic);
  Serial.println("");
  for (int i = 0; i < length; i++) {
    incoming += (char)payload[i];
  }
  incoming.trim();
  Serial.println("Mensaje -> " + incoming);

}

//funcion para publicar
void publicar( String Direccion, float variable)
{
  if (client.connected()) {
    char msg2[50];
    String str = String(variable);
    String ruta = String(root_topic_subscribe) + "/" + Direccion;
    str.toCharArray(msg, 25);
    ruta.toCharArray(msg2, 50);
    client.publish(msg2, msg);
    delay(300);
  }
}

void setup() {
  int16_t ret;
  uint8_t auto_clean_days = 4;
  uint32_t auto_clean;
  ledcSetup(LEDC_CHANNEL_0, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);
  ledcAttachPin(LED_PIN, LEDC_CHANNEL_0);
  dht.begin();
  Wire.begin();                               // inicializa bus I2C
  oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);     // inicializa pantalla con direccion 0x3C
  oled.setTextColor(WHITE);
  oled.setTextSize(1);// establece color al unico disponible
  oled.clearDisplay();
  oled.setCursor(0, 0);
  oled.println("Iniciando");
  oled.println("Espere");
  oled.display();
  delay(5000);
  Serial.begin(9600);
  Serial.println("----------------ESTACION4----------------");
  oled.clearDisplay();
  bmp180.begin();
  ledcWrite(LEDC_CHANNEL_0, 8191);

  time1 = time2 = time3 = millis();
  ads.begin();
  oled.setTextSize(1);

  if (!bmp180.begin())
  {
    oled.clearDisplay();
    oled.setCursor(0, 0);
    oled.println("BMP180 NO INICIADO");
    Serial.println("BMP180 NO INICIADO");
    oled.display();
    delay(5000);
  }
  else {
    Serial.println("BMP180 correcto");
  }

  if (! rtc.begin()) {
    oled.clearDisplay();
    oled.setCursor(0, 0);
    oled.println("RTC NO INICIADO");
    Serial.println("RTC NO INICIADO");
    Serial.flush();
    oled.display();
    delay(5000);
  }
  else {
    Serial.println("RTC correcto");
  }

  Serial.println("inicializacion correcta");  // texto de inicializacion correcta
  // CO2 configuracion de scd30

  if (airSensor.begin() == false)
  {
    Serial.println("Sensor de CO2 no inicia.");
  }



  // Configuracion para material particulado sps30
  sensirion_i2c_init();
  if (sps30_probe() != 0) {
    Serial.print("Error en sensor material particulado\n");
    delay(500);
  }
  ret = sps30_set_fan_auto_cleaning_interval_days(auto_clean_days);
  ret = sps30_start_measurement();
  delay(1000);
  // inicializacion de tarjeta SD
  oled.setTextSize(1);
  if (!SD.begin()) {
    oled.clearDisplay();
    oled.setCursor(0, 0);
    oled.println("SD NO INICIADO");
    Serial.println("fallo en inicializacion !");// si falla se muestra texto correspondiente y
    oled.display();
    delay(5000);
    return;         // se sale del setup() para finalizar el programa
  }
  //ConfiguracionMQTT
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

}

void loop() {



  unsigned long tiempo2 = millis() - time2;


  if (tiempo2 >= 60000 && s2 == false) {
    s2 = true;
    s4 = false;
    ledcWrite(LEDC_CHANNEL_0, 8191);
    if (datos >= 6) {
      datos = 0;
      DateTime now = rtc.now();
      File archivo = SD.open("/Estacion4.txt", FILE_APPEND);
      if (archivo) {
        Temp = T;
        archivo.print(now.year());
        archivo.print("-");
        archivo.print(now.month());
        archivo.print("-");
        archivo.print(now.day());
        archivo.print("-");
        archivo.print(now.hour());
        archivo.print(":");
        archivo.print(now.minute());
        archivo.print(":");
        archivo.print(now.second());
        archivo.print(" , ");
        archivo.print(P, 0);
        archivo.print(" , ");
        archivo.print(Temp);
        archivo.print(" , ");
        archivo.print(h, 0);
        archivo.print(" , ");
        archivo.print(PM2, 1);
        archivo.print(" , ");
        archivo.print(PM10, 1);
        archivo.print(" , ");
        archivo.print(MQ7, 1);
        archivo.print(" , ");
        archivo.print(MQ4, 1);
        archivo.print(" , ");
        archivo.print(CO2, 1);
        archivo.print(" , ");
        archivo.print(MQ131, 1);
        archivo.print(" , ");
        archivo.println(ML8511, 1);
        archivo.close();
        Serial.println("Escritura correcta");
      }
      else {
        Serial.print("Error de escritura en Tarjeta SD");
      }
      //ENVIO DE DATOS POR MQTT
      if (!client.connected()) {
        reconnect();
      }
      publicar("Temperatura", Temp);
      publicar("Presion", P);
      publicar("Humedad", h);
      publicar("PM_2.5", PM2);
      publicar("PM_10", PM10);
      publicar("CO", MQ7);
      publicar("CH4", MQ4);
      publicar("CO2", CO2);
      publicar("O3", MQ131);
      publicar("UV", ML8511);
      char Hora[10];
      char msg2[50];
      String hora = String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());
      hora.toCharArray(Hora, 10);
      if (client.connected()) {


        String ruta = String(root_topic_subscribe) + "/Hora";
        ruta.toCharArray(msg2, 50);
        client.publish(msg2, Hora);
        delay(300);
        Serial.println("--------Enviado-----");
      }
      client.loop();
    }
  }
  else {
    if (tiempo2 >= 150000 && s2 == true) {
      s2 = false;
      time2 = millis();
      ledcWrite(LEDC_CHANNEL_0, 8191);
    }
  }
  while (tiempo2 >= 140000 && s4 == false ) {

    V1 = float( ads.readADC_SingleEnded(gas_sensor_7)) * S;
    acum1 = acum1 + V1;
    V2 = float(ads.readADC_SingleEnded(gas_sensor_4)) * S;
    acum2 = acum2 + V2;
    V4 = float(ads.readADC_SingleEnded(gas_sensor_131)) * S;
    acum3 = acum3 + V4;
    V5 = float(analogRead(gas_sensor_8511)) * 3.12 / 4095;
    acum5 = acum5 + V5;
    muestras++;
    if (muestras == 100) {
      s3 = true;
      s4 = true;
    }
  }
  if (s3 == true)
  {
    // Sensor MQ7
    Prom_mq7 = acum1 / muestras;
    float RS_gas_7 = RL_7 * (((5.0) / Prom_mq7) - 1);
    float RS_RO_7 = RS_gas_7 / RO_7; // Get ratio RS_gas/RS_air
    MQ7 = 80.95 * pow(RS_RO_7 , -1.62);
    // Sensor MQ4
    Prom_mq4 = acum2 / muestras;
    float RS_gas_4 = RL_4 * ((5.0 / Prom_mq4) - 1);
    float RS_RO_4 = RS_gas_4 / RO_4; // Get ratio RS_gas/RS_air
    MQ4 = 2.818 * exp(RS_RO_4 * -3.024);



    // Sensor MQ131
    Prom_mq131 = acum3 / muestras;
    float RS_gas_131 = RL_131 * (((5.0) / Prom_mq131) - 1);
    float RS_RO_131 = RS_gas_131 / RO_131; // Get ratio RS_gas/RS_air
    MQ131 = 29.7 * pow(RS_RO_131 , -1.33);
    // Sensor Ml8511
    Prom_ML8511 = acum5 / muestras;
    ML8511 = Prom_ML8511 * 0.12 + 1;
    datos++;
    acum1 = 0;
    acum2 = 0;
    acum3 = 0;
    acum4 = 0;
    acum5 = 0;
    muestras = 0;
    s3 = false;
  }
  // Codigo para BPM
  char status;
  if ((millis() - time3) >= 10000) {
    if (airSensor.dataAvailable())
    {
      CO2 = airSensor.getCO2();
      t = airSensor.getTemperature();
      h = airSensor.getHumidity();
    }

    struct sps30_measurement m;
    char serial[SPS30_MAX_SERIAL_LEN];
    uint16_t data_ready;
    int16_t ret;


    ret = sps30_read_data_ready(&data_ready);
    if (ret < 0) {
      Serial.print("error reading data-ready flag: ");
      Serial.println(ret);
    }
    if (data_ready) {
      Serial.print("leyendo");
      ret = sps30_read_measurement(&m);
      if (ret < 0) {
        Serial.print("error reading measurement\n");
      } else {
        PM1 = m.mc_1p0;
        PM2 = m.mc_2p5;
        float PM4 = m.mc_4p0;
        PM10 = m.mc_10p0;
      }
    }

    DateTime now = rtc.now();
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
    // h = dht.readHumidity();
    //t = dht.readTemperature();
    status = bmp180.startTemperature();//Inicio de lectura de temperatura
    if (status != 0)
    {
      delay(status); //Pausa para que finalice la lectura
      status = bmp180.getTemperature(T); //Obtener la temperatura
      if (status != 0)
      {
        status = bmp180.startPressure(3); //Inicio lectura de presión
        if (status != 0)
        {
          delay(status);//Pausa para que finalice la lectura
          status = bmp180.getPressure(P, T); //Obtenemos la presión
        }
      }
    }

    Serial.print(now.unixtime());
    Serial.print(" , ");
    Serial.print(P);
    Serial.print(" , ");
    Serial.print(T);
    Serial.print(" , ");
    Serial.print(h);
    Serial.print(" , ");
    Serial.print(t);
    Serial.print(" , ");
    Serial.print(PM2, 1);
    Serial.print(" , ");
    Serial.print(PM10, 1);
    Serial.print(" , ");
    Serial.print(MQ7, 1);
    Serial.print(" , ");
    Serial.print(MQ4, 1);
    Serial.print(" , ");
    Serial.print(CO2, 1);
    Serial.print(" , ");
    Serial.print(MQ131, 1);
    Serial.print(" , ");
    Serial.println(ML8511, 1);
    time3 = millis();
    estado++;
    if (estado > 2)
      estado = 0;
  }

  switch (estado) {
    case 0:
      oled.clearDisplay();
      oled.setCursor(0, 0);
      oled.setTextSize(1);
      oled.println("--MQ--");
      oled.print("MQ7: ");
      oled.print(MQ7, 1);
      oled.println("ppm");
      oled.print("MQ4: ");
      oled.print(MQ4, 1);
      oled.println("ppm");
      oled.print("MG811: ");
      oled.print(CO2, 1);
      oled.println("ppm");
      oled.print("MQ131: ");
      oled.print(MQ131, 1);
      oled.println("ppm");
      oled.print(tiempo2/1000);
      oled.println("s");
      oled.display();
      break;
    case 1:    // your hand is close to the sensor
      oled.clearDisplay();
      oled.setCursor(0, 0);
      oled.setTextSize(1);
      oled.println("--BPM180--");
      oled.print("P: ");
      oled.print(P);
      oled.println("mb");
      oled.print("T: ");
      oled.print(T);
      oled.println(" c");
      oled.println("--DHT11--");
      oled.print("H: ");
      oled.print(h);
      oled.println(" % ");
      oled.print("T: ");
      oled.print(t);
      oled.println(" c");
      oled.print(tiempo2/1000);
      oled.println("s");
      oled.display();

      break;
    case 2:    // your hand is a few inches from the sensor
      oled.clearDisplay();
      oled.setCursor(0, 0);
      oled.setTextSize(1);
      oled.println("--PMS5003--");
      oled.print("PM 1:");
      oled.println(PM1, 1);
      oled.print("PM 2.5:");
      oled.println(PM2, 1);
      oled.print("PM 10:");
      oled.println(PM10, 1);
      oled.print("UV: ");
      oled.print(ML8511, 1);
      oled.println("mW/cm2");
      oled.print(tiempo2/1000);
      oled.println("s");
      oled.display();
      break;


  }

}
