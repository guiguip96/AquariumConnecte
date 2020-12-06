#include <Arduino.h>
#include <WiFiManager.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <SPI.h>
#include <Wire.h>
#include <Onewire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_SPIDevice.h>
#include <DallasTemperature.h>

#define Webserver_H
#define SCREEN_WIDTH 128                                            // OLED display width, in pixels
#define SCREEN_HEIGHT 64                                            // OLED display height, in pixels
#define Pompe 27

WiFiManager wm;                                                     // Instance WiFiManager
OneWire oneWire(25);                                                // Indique que la sonde est connectée sur la pin 25 de l'ESP
AsyncWebServer server(80);                                          // Port utilisé pour le serveur hébergé sur l'ESP
DallasTemperature sensors(&oneWire);                                // Lecture des données de la sonde sur la pin 25 de l'ESP
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);   // Déclaration de l'écran OLED

const int LED = 2;                                                  //La PIN GPIO de la LED
const char* ssid = "GuiguiAquarium";                                //Info de connection pour le wifi
const char* password = "InfoAutomne2020$";                          //Info de connection pour le wifi
String paramNomAquarium = "Test";                                   //Nom défini sur l'interface web
int paramTemperatureMin = 0;                                        //Température minimale définie sur l'interface web
int paramTemperatureMax = 0;                                        //Température maximale définie sur l'interface web   
int paramDureePompe = 0;   
int paramTimerPompe = 0;                                            //Nombre de millisecondes pour laquelle la pompe doit s'activer
bool etatPompe;                                                     //Indicateur servant à annoncer si la température se trouve dans le bon range
int previousMillis = 0;

// Fonction qui permet d'aller chercher une température en celsius avec le senseur.
float demanderTemperature()
{
  sensors.requestTemperatures();
  return sensors.getTempCByIndex(0);
}

// Fonction qui vérifie si la pompe doit s'activer et pour quelle durée
void verifierPompe()
{
  unsigned long currentMillis = millis();
  unsigned long timingPompe = paramTimerPompe * 1000;
  unsigned long dureePompe = paramDureePompe * 1000;

  if(currentMillis - previousMillis >= timingPompe)
  {
    previousMillis = currentMillis;

    if(etatPompe == false)
    {
      etatPompe = true;
      digitalWrite(Pompe, HIGH);
      delay(dureePompe);
    }
    else
    {
      etatPompe = false;
      digitalWrite(Pompe, LOW);
    }
  }
}

// Vérifie si la température est dans le range requis. Si non, la LED s'allume.
void verifierTemperature()
{
  Serial.print(demanderTemperature());

  if (demanderTemperature() <= paramTemperatureMax && demanderTemperature() >= paramTemperatureMin)
    digitalWrite(LED, LOW);
  else
    digitalWrite(LED, HIGH);
}

// Fonction qui permet d'afficher les informations sur l'OLED
void afficherOLED()
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  display.setCursor(0, 0);
  display.print("Nom: ");
  display.println(paramNomAquarium);

  display.setCursor(0, 20);
  display.println("Temp. actuelle (C): ");
  display.println(String(demanderTemperature()));

  display.setCursor(0, 40);
  display.println("Adresse IP : ");
  display.println(WiFi.localIP().toString());
  display.display();
}

void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Not found");
}

void setup() {

  Serial.begin(9600);                                               //Initialisation du serial monitor
  pinMode(LED, OUTPUT);                                        //Définition de la LED en OUTPUT
  pinMode(Pompe, OUTPUT);

  //Initialisation de l'OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  { 
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }
  delay(2000);

  // Initialisation du WiFi-Manager
  WiFi.mode(WIFI_STA);
  if (!wm.autoConnect(ssid, password))
    Serial.println("Erreur de connexion.");
  else
    Serial.println("Connexion etablie!");

  Serial.println();
  Serial.print("Adresse IP: ");
  Serial.println(WiFi.localIP());

  // Initialisaton du senseur de température
  sensors.begin();

  // Initialisation du SPIFFS
  if (!SPIFFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  server.onNotFound(notFound);

  // Envoie la page d'accueil
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) 
  {
    request->send(SPIFFS, "/indexFR.html", "text/html");
  });

  // Envoie la page francophone
  server.on("/indexFR.html", HTTP_GET, [](AsyncWebServerRequest *request) 
  {
    request->send(SPIFFS, "/indexFR.html", "text/html");
  });

  // Envoie la page anglophone
  server.on("/indexEN.html", HTTP_GET, [](AsyncWebServerRequest *request) 
  {
    request->send(SPIFFS, "/indexEN.html", "text/html");
  });

  //Envoie la page de style
  server.on("/w3.css", HTTP_GET, [](AsyncWebServerRequest *request) 
  {
    request->send(SPIFFS, "/w3.css", "text/css");
  });

  //Envoie la page de sripts
  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request) 
  {
    request->send(SPIFFS, "/script.js", "text/javascript");
  });

  //Envoie le jQuery
  server.on("/jquery-3.5.1.min.js", HTTP_GET, [](AsyncWebServerRequest *request) 
  {
    request->send(SPIFFS, "/jquery-3.5.1.min.js", "text/javascript");
  });

  //Reçoit le nom de notre interface web
  server.on("/inputNomAquarium", HTTP_POST, [](AsyncWebServerRequest *request) 
  {
    
    if(request->hasParam("proprieteNom", true))
    {
      String nomAquarium;
      nomAquarium = request->getParam("proprieteNom", true)->value();
      paramNomAquarium = nomAquarium;
    }
    request->send(204, "Le nom a été mis a jour");
  });

  //Reçoit les températures de notre interface web
  server.on("/inputTempAquarium", HTTP_POST, [](AsyncWebServerRequest *request) 
  {
    String tempMinAquarium;
    String tempMaxAquarium;
    if(request->hasParam("proprieteTempMin", true) && request->hasParam("proprieteTempMax", true))
    {
      tempMinAquarium = request->getParam("proprieteTempMin", true)->value();
      tempMaxAquarium = request->getParam("proprieteTempMax", true)->value();
      paramTemperatureMin = tempMinAquarium.toInt();
      paramTemperatureMax = tempMaxAquarium.toInt();
    }
    request->send(204, "La température a été mise a jour");
  });

  //Reçoit les infos de la pompe de notre interface web
  server.on("/inputPompeAquarium", HTTP_POST, [](AsyncWebServerRequest *request) 
  {
    String pompeDureeAquarium;
    String pompeTimerAquarium;
    if(request->hasParam("proprietePompeDuree", true) && request->hasParam("proprietePompeTimer", true))
    {
      pompeDureeAquarium = request->getParam("proprietePompeDuree", true)->value();
      pompeTimerAquarium = request->getParam("proprietePompeTimer", true)->value();
      paramDureePompe = pompeDureeAquarium.toInt();
      paramTimerPompe = pompeTimerAquarium.toInt();
    }
    request->send(204, "La pompe a été mise a jour");
  });

  
  // Fonction qui démarre le serveur local
  server.begin();
}


void loop() {
  // Vérifie les états pour la plaque chauffante et la pompe
  verifierTemperature();
  verifierPompe();

  Serial.println(paramDureePompe);
  Serial.println(paramTimerPompe);

  // Fonction qui permet d'afficher les informations sur l'OLED
  afficherOLED();
}