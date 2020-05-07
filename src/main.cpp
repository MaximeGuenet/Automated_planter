#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <ESP8266WiFi.h>  // Pour ESP32 : <SPIFFS.h>
#include <ArduinoJson.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <DHT.h>
#include <Wire.h>

#define KelvinToCelsius 273.15
#define Seconde 1000
#define Minute 60*Seconde
#define Heure 60*Minute
#define Jour 24*Heure
#define Annee 365*Jour

// Définition de variable
unsigned long utcOffsetInSeconds = 0;
unsigned long previousLoopMillis = 0;
float currentTemperature;
float dailyTemperature;
float currentFeelTemperature;
float dailyFeelTemperature;
int currentSunrise;
int dailySunrise;
int currentSunset;
int dailySunset;
float currentPression;
float dailyPression;
float currentHumidity;
float dailyHumidity;
float currentCloudness;
float dailyCloudness;
float currentWindSpeed;
float dailyWindSpeed;
float currentRain;
float dailyRain;
float currentHourSunrise;
float currentMinuteSunrise;
float currentHourSunset;
float currentMinuteSunset;
float dailyHourSunrise;
float dailyMinuteSunrise;
float dailyHourSunset;
float dailyMinuteSunset;
bool OnOffArrosage = LOW;

// Connection à notre livebox
const char* ssid = "TRUC";
const char* password = "machinbidule13";

// Paramètre de connection à l'API Météo
const char* host = "api.openweathermap.org";
String APPID = "0552682e1f53559fc63f09237255cc7b";
String Lat = "47.73";          // Bateau Horus
String Long = "7.30";          // Bateau Horus

// Définition du client NTP pour récupérer l'heure
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", utcOffsetInSeconds);

// Définition des ports de la carte
const int led=2;
DHT dht(1,DHT22);                     // Numéro de la broche à redefinir
const int CapteurHumiditeAir = 1;     // Numéro de la broche
const int CapteurHumiditeTerre_1 = 1; // Numéro de la broche
const int CapteurHumiditeTerre_2 = 1; // Numéro de la broche
const int CapteurHumiditeTerre_3 = 1; // Numéro de la broche
const int PompeEau = 1;               // Numéro de la broche
const int RubanLed = 1;               // Numéro de la broche

AsyncWebServer server(80);

void setup() 
{
  //---------------------------------- Serial
  Serial.begin(115200);
  while(!Serial){}
  Serial.println("\n");

  //---------------------------------- GPIO
  Serial.println("-- GPIO --");
  // INPUT
  pinMode(CapteurHumiditeTerre_1, INPUT);
  pinMode(CapteurHumiditeTerre_2, INPUT);
  pinMode(CapteurHumiditeTerre_3, INPUT);
  dht.begin();
  // OUTPUT
  pinMode(led, OUTPUT);
  digitalWrite(led,LOW);
  pinMode(PompeEau, OUTPUT);
  digitalWrite(PompeEau,LOW);
  pinMode(RubanLed, OUTPUT);
  digitalWrite(RubanLed,LOW);
  Serial.println("GPIO : ok");
  Serial.println("");

  //---------------------------------- SPIFFS
  // On s'assure que le gestionnaire de fichier démarre correctement
  Serial.println("-- SPIFFS --");
  if (!SPIFFS.begin())
  {
    Serial.println("SPIFFS : erreur");
    Serial.println("");
    return;
  } else 
  {
    Serial.println("SPIFFS : ok");
    Serial.println("");
    return;
  }

  //---------------------------------- WIFI
  Serial.println("-- WIFI --");
   WiFi.begin (ssid, password);
   Serial.print("Tentative de connexion à : ");
   Serial.println(ssid);
    // Attente de la connexion au réseau WiFi
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay ( 100 );
    Serial.print ( "." );
  }
    // Connexion WiFi établie
  Serial.println("");
  Serial.print("Connecté au reseau WiFi : "); 
  Serial.println(ssid);
  Serial.print("L'adresse IP est : "); 
  Serial.println(WiFi.localIP());
  Serial.println("WiFi : ok");
  Serial.println("");
  WiFiClient client;

  //---------------------------------- SERVER
  Serial.println("-- Server --");
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(SPIFFS,"/index.html", "text/html");
  });
    server.on("/w3.css", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(SPIFFS,"/w3.css", "text/css");
  });
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(SPIFFS,"/script.js", "text/javascript");
  });
    server.on("/lireTemperatureAir", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    int val = dht.readTemperature();
    String TemperatureAir = String(val);
    request->send(200, "text/plain", TemperatureAir);
  });
    server.on("/lireTemperatureAirMeteo", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    String TemperatureAirMeteo = String(currentTemperature);
    request->send(200, "text/plain", TemperatureAirMeteo);
  });
    server.on("/lireTemperatureAirRessentie", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    String TemperatureAirRessentie = String(currentFeelTemperature);
    request->send(200, "text/plain", TemperatureAirRessentie);
  });
    server.on("/lireTemperatureAirDay", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    String TemperatureAirDay = String(dailyTemperature);
    request->send(200, "text/plain", TemperatureAirDay);
  });
    server.on("/lireHumiditeAirMeteo", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    String HumiditeAirMeteo = String(currentHumidity);
    request->send(200, "text/plain", HumiditeAirMeteo);
  });
    server.on("/lireHumiditeAirDaily", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    String HumiditeAirDaily = String(dailyHumidity);
    request->send(200, "text/plain", HumiditeAirDaily);
  });
    server.on("/lirePression", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    String Pression = String(currentPression);
    request->send(200, "text/plain", Pression);
  });
  server.on("/lirePressionDaily", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    String PressionDaily = String(dailyPression);
    request->send(200, "text/plain", PressionDaily);
  });
  server.on("/lireCloudness", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    String Cloudness = String(currentCloudness);
    request->send(200, "text/plain", Cloudness);
  });
  server.on("/lireCloudnessDaily", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    String CloudnessDaily = String(dailyCloudness);
    request->send(200, "text/plain", CloudnessDaily);
  });
  server.on("/lireRain", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    String Rain = String(currentRain);
    request->send(200, "text/plain", Rain);
  });
  server.on("/lireRainDaily", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    String RainDaily = String(dailyRain);
    request->send(200, "text/plain", RainDaily);
  });
  server.on("/lireWind", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    String Wind = String(currentWindSpeed);
    request->send(200, "text/plain", Wind);
  });
  server.on("/lireWindDaily", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    String WindDaily = String(dailyWindSpeed);
    request->send(200, "text/plain", WindDaily);
  });
    server.on("/onArrosage", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    Serial.println("Requete : Pompe");
    OnOffArrosage = !OnOffArrosage;
    digitalWrite(PompeEau, OnOffArrosage);    
  });
      server.on("/onEclairage", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    Serial.println("Requete : Allumage ruban led");
    digitalWrite(RubanLed, HIGH);
    request->send(200);
  });
      server.on("/offEclairage", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    Serial.println("Requete : Arret ruban led");
    digitalWrite(RubanLed, LOW);
    request->send(200);
  });
      server.on("/led", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    Serial.println("Requete : Allumage/Arret led");
    digitalWrite(led, !led);
    request->send(200);
  });

  server.begin();
  Serial.println("Server : ok");
  Serial.println("------------------------------------------------------------------");
  Serial.println("\n");

  //---------------------------------- API Météo
  Serial.println("-- API Météo --");
  Serial.printf("\n[Connexion à %s ...", host);
  delay(500);
  if (client.connect(host, 80))
  {
    Serial.println("API Météo : ok");
    delay(500);

    // Préparation et envois de la requête vers l'API avec vos paramètres initialisé ci-dessus
    client.print(String("GET /data/2.5/onecall?lat="+Lat+"&lon="+Long+"&appid="+APPID+" HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection : close\r\n" + "\r\n"));
    // Récupération de la réponse
    char meteo[32] = {0};
    client.readBytesUntil('\r', meteo, sizeof(meteo));
    if (strcmp(meteo, "HTTP/1.1 200 OK") != 0)
    {
      Serial.print(F("Reponse inattendue: "));
      Serial.println(meteo);
      return;
    }
    else 
    {
      delay(500);
      Serial.println("La réponse de la requéte vient d'arriver");
      delay(500);
    }
  }

  //la taille du buffer à créer -> pour le savoir tester sur le site : https://arduinojson.org/v5/assistant/
  const size_t capacity = JSON_ARRAY_SIZE(3) + 2*JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(2) + 3*JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(6) + JSON_OBJECT_SIZE(12) + 480;
  DynamicJsonBuffer JsonBuffer(capacity);
  JsonObject& root = JsonBuffer.parseObject(client);  //création du tableau qui va contenir la réponse à notre requête

  // Traitement de la requête json
  //Parcours du tableau JSON reçus pour avoir les informations que l'on souhaite
  currentTemperature = float(root["current"]["temp"]) - KelvinToCelsius;
  dailyTemperature = float(root["daily"]["temp"]) - KelvinToCelsius;
  Serial.println((String)"Temperture actuelle : " + currentTemperature + " et température de la journée : " + dailyTemperature);
  currentFeelTemperature = float(root["current"]["feels_like"]) - KelvinToCelsius;
  dailyFeelTemperature = float(root["daily"]["feels_like"]) - KelvinToCelsius;
  Serial.println((String)"Temperture ressentie actuelle : " + currentFeelTemperature + " et température ressentie de la journée : " + dailyFeelTemperature);
  currentSunrise = root["current"]["sunrise"];
  dailySunrise = root["daily"]["sunrise"];
  currentHourSunrise = currentSunrise / 3600 % 24;
  currentMinuteSunrise = currentSunrise / 60 % 60;
  dailyHourSunrise = dailySunrise / 3600 % 24;
  dailyMinuteSunrise = dailySunrise / 60 % 60;
  Serial.println((String)"Sunrise actuelle : " +currentHourSunrise+ ":"+currentMinuteSunrise+ " et Sunrise de la journée : " +dailyHourSunrise+ ":"+dailyMinuteSunrise);
  currentSunset = root["current"]["sunset"];
  dailySunset = root["daily"]["sunset"];
  currentHourSunset = currentSunset / 3600 % 24;
  currentMinuteSunset = currentSunset / 60 % 60;
  dailyHourSunset = dailySunset / 3600 % 24;
  dailyMinuteSunset = dailySunset / 60 % 60;
  Serial.println((String)"Sunset actuelle : " +currentHourSunset+ ":"+currentMinuteSunset+ " et Sunset de la journée : " +dailyHourSunset+ ":"+dailyMinuteSunset);
  currentPression = root["current"]["pressure"];
  dailyPression = root["daily"]["pressure"];
  Serial.println((String)"TPression actuelle : " + currentPression + " et Pression de la journée : " + dailyPression);
  currentHumidity = root["current"]["humidity"];
  dailyHumidity = root["daily"]["humidity"];
  Serial.println((String)"Humidité actuelle : " + currentHumidity + " et humidité de la journée : " + dailyHumidity);
  currentCloudness = root["current"]["clouds"];
  dailyCloudness = root["daily"]["clouds"];
  Serial.println((String)"Nebulosité actuelle : " + currentCloudness + " et nébulosité de la journée : " + dailyCloudness);
  currentWindSpeed = root["current"]["wind-speed"];
  dailyWindSpeed = root["daily"]["wind-speed"];
  Serial.println((String)"Vent actuelle : " + currentWindSpeed + " et vent de la journée : " + dailyWindSpeed);
  currentRain = root["current"]["rain"];
  dailyRain = root["daily"]["rain"];
  Serial.println((String)"Pluie actuelle : " + currentRain + " et pluie de la journée : " + dailyRain);
  
}

// Fonction qui va gérer l'enregistrement des donnée de température et d'humidité
void addData()
{
  char strbufferTempWeb[64];
  char strbufferHumWeb[64];
  char strbufferTempMes[64];
  char strbufferHumAirMes[64];
  char strbufferHumSoil1[64];
  char strbufferHumSoil2[64];
  char strbufferHumSoil3[64];

  int timestamp = timeClient.getEpochTime();

  snprintf(strbufferTempWeb, 64, "%d,%.2f;",timestamp,currentTemperature);
  snprintf(strbufferHumWeb, 64, "%d,%.2f;",timestamp,currentHumidity);
  snprintf(strbufferTempMes, 64, "%d,%.2f;",timestamp,dht.readTemperature());
  snprintf(strbufferHumAirMes, 64, "%d,%.2f;",timestamp,dht.readHumidity());
  snprintf(strbufferHumSoil1, 64, "%d,%.3d;",timestamp,analogRead(CapteurHumiditeTerre_1));
  snprintf(strbufferHumSoil2, 64, "%d,%.3d;",timestamp,analogRead(CapteurHumiditeTerre_2));
  snprintf(strbufferHumSoil3, 64, "%d,%.3d;",timestamp,analogRead(CapteurHumiditeTerre_3));

  File T = SPIFFS.open("/temperature.csv","a+");
  if (T)
  {
    T.print(strbufferTempMes);
    T.close();
  }
  File H = SPIFFS.open("/humidite.csv","a+");
  if (H)
  {
    H.print(strbufferHumAirMes);
    H.close();
  }
}

void loop() 
{
  unsigned long currentLoopMillis = millis();
  if(currentLoopMillis - previousLoopMillis >= 30*Minute)
  {
    addData();
    previousLoopMillis = millis();
  }

  timeClient.update();
}