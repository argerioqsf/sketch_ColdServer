  #include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRsend.h>
#include <IRutils.h>

const uint16_t kIrLed = D6;  // ESP8266 GPIO pin to use. Recommended: 4 (D2).

IRsend irsend(kIrLed);  // Set the GPIO to be used to sending the message.

int RECV_PIN = D6; //an IR detector/demodulatord is connected to GPIO pin D2
const char* ssid = "UCHOA_QUEIROS";
const char* password = "22041975";
const int ligado = D1;
const int desligado = D2;
bool Status = false;

ESP8266WebServer server(80);
IRrecv irrecv(RECV_PIN);
decode_results results;

void returnOK() {
  server.send(200, "text/plain", "OK");
    Serial.println(server.args());
}

void handleRoot() {
  server.send(200, "text/plain", "hello from esp8266!");
  if(Status == true){
      digitalWrite(desligado, 1);
      digitalWrite(ligado, 0);
        Serial.println("Desigado");
    }else if(Status == false){
      digitalWrite(ligado, 1);
      digitalWrite(desligado, 0);
      Serial.println("Ligado");
    }
}

 void Stat(String stat){
  // HTML da pagina Status
  String html = "<html><head><title>HOME</title>";
  html += "<style> header {min-height: 60px;position: fixed;top: 0;right: 0;";
  html += "left: 0;border-bottom: 1px solid #ccc;background: #ECECEC;z-index: 2;}</style>";
  html += "</head><body>";
  html += "<header><h1 class='float-l'>Home - ColdServe</h1><br>";
    html += "<h2>"+ stat +"</h2>";
    html +="<p><a href=/>Voltar</a></p>";
  html += "</header></body></html>";
  // Enviando HTML para o servidor
  server.send(200, "text/html", html);
 }

 void Home(){
  // HTML da pagina principal
  String html = "<html><head><title>HOME</title>";
  html += "<style> header {min-height: 60px;position: fixed;top: 0;right: 0;";
  html += "left: 0;border-bottom: 1px solid #ccc;background: #ECECEC;z-index: 2;}</style>";
  html += "</head><body>";
  html += "<header><h1 class='float-l'>Home - ColdServe</h1>";
    html += "<form method='POST' action='/ligarServer'>";
      html += "<p><input type=submit name=post value=Ligar /></p>";
    html += "</form>";
    html += "<form method='POST' action='/ligarServer'>";
      html += "<p><input type=submit name=post value=Desligar /></p>";
    html += "</form>";
  html += "</header></body></html>";
  // Enviando HTML para o servidor
  server.send(200, "text/html", html);
  }

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void postar(){
    Serial.println("Quant - " + server.args());
    Serial.println("Plain - " + server.arg("plain"));
    Serial.println("Post - " + server.arg("modo"));
    StaticJsonBuffer<300> newBuffer;
    JsonObject& newjson = newBuffer.parseObject(server.arg("plain"));
    int var1 = (int) newjson["modo"];
    Serial.println("Json - " + var1);
    Serial.println("post - " + var1);
    if(var1 == 1){
      Status = true;
      digitalWrite(ligado, 1);
      digitalWrite(desligado, 0);
      Serial.println("Ligado");
      server.send(200, "text/plain", "LIGADO");
      irsend.sendNEC(0xA90, 32);
    }else if(var1 == 0){
      Status = false;
      digitalWrite(ligado, 0);
      digitalWrite(desligado, 1);
      Serial.println("Desligado");
      server.send(200, "text/plain", "DESLIGADO");
      irsend.sendNEC(0xA91, 32);
    }else{
      Serial.println("Comando inválido");
      server.send(200, "text/plain", "INVÁLIDO");
      digitalWrite(ligado, 0);
      digitalWrite(desligado, 0);
      int cont = 0;
      for(int i = 0;i<7;i++){
        digitalWrite(desligado, !cont);
        digitalWrite(ligado, cont);
        cont = !cont;
        delay(200);
      }
      digitalWrite(ligado, 0);
      digitalWrite(desligado, 0);
      if(Status == true){
        digitalWrite(ligado, 1);
        Serial.println("Ligado");
      }else if(Status == false){
        digitalWrite(desligado, 1);
        Serial.println("Desligado");
      }
    }    
    char JSONmessageBuffer[300];
    newjson.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
    Serial.println(JSONmessageBuffer);
  }
    

    void postarServer(){
    Serial.println("Quant - " + server.args());
    Serial.println("Plain - " + server.arg("plain"));
    Serial.println("Post - " + server.arg("post"));
    String var = server.arg("post");
    Serial.println("post - " + var);
    if(var == "Ligar"){
      Status = true;
      digitalWrite(ligado, 1);
      digitalWrite(desligado, 0);
      Serial.println("Ligado");
      Stat(var);
      irsend.sendNEC(0xA90, 32);
    }else if(var == "Desligar"){
      Status = false;
      digitalWrite(ligado, 0);
      digitalWrite(desligado, 1);
      Serial.println("Desligado");
      Stat(var);
      irsend.sendNEC(0xA91, 32);
    }else{
      Serial.println("Comando inválido");
      Stat("INVALIDO");
      digitalWrite(ligado, 0);
      digitalWrite(desligado, 0);
      int cont = 0;
      for(int i = 0;i<7;i++){
        digitalWrite(desligado, !cont);
        digitalWrite(ligado, cont);
        cont = !cont;
        delay(200);
      }
      digitalWrite(ligado, 0);
      digitalWrite(desligado, 0);
      if(Status == true){
        digitalWrite(ligado, 1);
        Serial.println("Ligado");
      }else if(Status == false){
        digitalWrite(desligado, 1);
        Serial.println("Desligado");
      }
    }
  }
  
void IPGET(){
  Serial.println(server.args());
  Serial.println(server.arg("teste"));
  String html = "<html><head><title>HOME</title>";
  html += "<style> header {min-height: 60px;position: fixed;top: 0;right: 0;";
  html += "left: 0;border-bottom: 1px solid #ccc;background: #ECECEC;z-index: 2;}</style>";
  html += "</head><body>";
  html += "<header><h1 class='float-l'>Trocar IP - ColdServe</h1>";
  html += "<p><a href=/>Pagina Principal</a></p>";
  html += "</header></body></html>";
  // Enviando HTML para o servidor
  server.send(200, "text/html", html);
}

void setup(void) {
  pinMode(ligado, OUTPUT);
  pinMode(desligado, OUTPUT);
  digitalWrite(ligado, 0);
  digitalWrite(desligado, 1);
  irsend.begin();
  Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  IPAddress subnet(255, 255, 255, 0);  
  WiFi.config(IPAddress(192, 168, 43, 115),
              IPAddress(192, 168, 43, 1), subnet);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  irrecv.enableIRIn(); // Start the receiver
  

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/",HTTP_GET, Home);
  
  server.on("/ligar",HTTP_POST,postar);
  
  server.on("/ip",HTTP_GET,IPGET);

  server.on("/ligarServer",HTTP_POST,postarServer);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}



void loop(void) {
  server.handleClient();
}
