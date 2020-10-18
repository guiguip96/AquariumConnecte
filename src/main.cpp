#include <WiFiManager.h>
#include <WebServer.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

WiFiManager wm;
const char* ssid = "ESP-GUIGUI";
const char* password = "Patate123";
WebServer server(80);

void handleRoot()
{
    String page = "<!DOCTYPE html>";
    page += "<html>";
    page += "<head>";
    page += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
    page += "<style>";
    page += ".tab {overflow: hidden;display:flex;justify-content:space-around;width:100%;}";
    page += ".tab button:hover {background-color: #ddd;}";
    page += ".tab button.active {background-color: #ccc;}";
    page += ".tabcontent {padding: 6px 12px;border: 1px solid #ccc;border-top: none;background-color:rgba(255,255,255,0.8);}";
    page += "body {";
    page += "background-image: url('http://getwallpapers.com/wallpaper/full/b/e/9/280762.jpg');}";
    page += "</style>";
    page += "</head>";
    page += "<body>";
    page += "<h2 class='tab'>Panneau de controle</h2>";
    page += "<p class='tab'>Aquarium</p>";
    page += "<div id='Temperature' class='tabcontent'>";
    page += "<h3>Ecart de temperature desiree :</h3>";
    page += "<input type='text' placeholder='De'>";
    page += "<input type='text' placeholder='A'>";
    page += "</div><br>";
    page += "<div id='Pompe' class='tabcontent'>";
    page += "<h3>Cadence de la pompe, aux 500 secondes :</h3>";
    page += "<input type='text' placeholder='Durée en secondes'>";
    page += "</div><br>";
    page += "<div id='Affichage' class='tabcontent'>";
    page += "<h3>Informations a afficher sur l'ecran :</h3>";
    page += "<input type='text' placeholder='Nom du bocal'>";
    page += "<input type='text' placeholder='Couleur du texte'>";
    page += "</div>";
    page += "</body>";
    page += "</html>";

    server.send(200, "text/html", page);
}

void handleNotFound()
{
    server.send(404, "text/plain", "404: Not found");
}

void setup()
{

    WiFi.mode(WIFI_STA);
    Serial.begin(9600);
    delay(1000);
	Serial.println("\n");

if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);}

	
	if(!wm.autoConnect(ssid, password))
		Serial.println("Erreur de connexion.");
	else
		Serial.println("Connexion etablie!");

    Serial.println("\n");
    Serial.println("Connexion etablie!");
    Serial.print("Adresse IP: ");
    Serial.println(WiFi.localIP());

    server.on("/", handleRoot);
    server.onNotFound(handleNotFound);
    server.begin();

    Serial.println("Serveur web actif!");

    delay(2000);
    display.clearDisplay();

    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 40);
    // Display static text
    display.println("IP: 192.168.0.121");
        display.setCursor(0, 20);
    // Display static text
    display.println("Aquarium de Guigui");
            display.setCursor(0, 50);
    // Display static text
    display.println("Temp: 15 C");
    display.display(); 

}

void loop()
{
    server.handleClient();
}