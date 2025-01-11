#include <WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <MFRC522.h>
#include <map>

// Configurações de WiFi
const char* ssid = "IEFP-Formacao";
const char* password = "";

// Configurações do MQTT
const char* mqtt_server = "thingsboard.cloud";
const char* mqtt_token1 = "u7ggtaam7vk52d11c1yn1";
const char* mqtt_token2 = "98XKvKBeKTIfopBgKSc2";
const char* mqtt_token3 = "oJ9bIv9slSvN6rdKkEgp";
WiFiClient espClient1;
WiFiClient espClient2;
WiFiClient espClient3;
PubSubClient client1(espClient1);
PubSubClient client2(espClient2);
PubSubClient client3(espClient3);

#define BUZZER_PIN 12

#define SS_PIN 21
#define RST_PIN 22
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Cria a instância do MFRC522

// Mapa para armazenar UIDs de cartões e os nomes correspondentes
std::map<String, String> cardMap;

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("Endereço IP: ");
  Serial.println(WiFi.localIP());
}

void reconnect(PubSubClient &client, const char* token) {
  while (!client.connected()) {
    Serial.print("Tentando conectar ao MQTT...");
    if (client.connect("ESP32_Client", token, NULL)) {
      Serial.println("conectado");
    } else {
      Serial.print("falhou, rc=");
      Serial.print(client.state());
      Serial.println(" tente novamente em 5 segundos");
      delay(5000);
    }
  }
}

void setup() 
{
  Serial.begin(9600);   // Inicia comunicação serial
  setup_wifi();
  client1.setServer(mqtt_server, 1883);
  client2.setServer(mqtt_server, 1883);
  client3.setServer(mqtt_server, 1883);

  pinMode(BUZZER_PIN, OUTPUT);

  SPI.begin();          // Inicia o barramento SPI
  mfrc522.PCD_Init();   // Inicia o MFRC522
  Serial.println("Aproxime o cartão ao leitor...");
  Serial.println();
  pinMode(17, OUTPUT);
  pinMode(4, OUTPUT);

  // Inicializa o cardMap com UIDs e nomes correspondentes
  cardMap["04 2F 8D 0A BA 34 81"] = "Luiz Felipe";
  cardMap["04 59 1B 02 BA 34 84"] = "Tiago";
  cardMap["04 AA 92 0A BA 34 80"] = "Lucas Cruz";
  cardMap["04 5A B4 0A BA 34 80"] = "Lucas Ramazza";
  cardMap["04 51 26 02 BA 34 80"] = "Afonso";
  cardMap["04 66 1A 02 BA 34 84"] = "Nelson Ricardo";
  cardMap["04 41 F8 0A BA 34 80"] = "Maycon";
  cardMap["04 41 F8 0A BA 34 80"] = "Emanuelle";
  cardMap["04 82 25 02 BA 34 80"] = "Dilean";
  cardMap["04 E1 B7 0A BA 34 80"] = "Amanda";
  cardMap["B7 CF FB D8"] = "Formador Miguel";
}

void loop() 
{
  if (!client1.connected()) {
    reconnect(client1, mqtt_token1);
  }
  if (!client2.connected()) {
    reconnect(client2, mqtt_token2);
  }
  if (!client3.connected()) {
    reconnect(client3, mqtt_token3);
  }
  client1.loop();
  client2.loop();
  client3.loop();

  // Procura por novos cartões
  if (!mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Seleciona um dos cartões
  if (!mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  // Mostra o UID no monitor serial
  Serial.print("UID tag :");
  String content = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();

  // Verifica se o UID do cartão existe no mapa
  if (cardMap.find(content.substring(1)) != cardMap.end()) 
  {
    String name = cardMap[content.substring(1)];
    Serial.println("Acesso autorizado: " + name);
    grantAccess();
    sendToThingsBoard(true, name, content.substring(1));
  }
  else 
  {
    String name = "Usuário desconhecido";
    Serial.println("Acesso negado: " + name);
    denyAccess();
    sendToThingsBoard(false, name, content.substring(1));
  }
}

void grantAccess() 
{
  digitalWrite(17, HIGH);  // Acende LED verde
  tone(BUZZER_PIN, 2000); 
  delay(300);
  noTone(BUZZER_PIN);
  digitalWrite(17, LOW);   // Apaga LED verde
  digitalWrite(4, LOW);    // Garante que o LED vermelho está apagado
  delay(3000);
}

void denyAccess() 
{
  digitalWrite(4, HIGH);   // Acende LED vermelho
  tone(BUZZER_PIN, 500);  
  delay(200);
  noTone(BUZZER_PIN);     
  delay(200);            
  tone(BUZZER_PIN, 500); 
  delay(200);             
  noTone(BUZZER_PIN);   
  digitalWrite(4, LOW);    // Apaga LED vermelho
  digitalWrite(17, LOW);   // Garante que o LED verde está apagado
  delay(3000);
}

void sendToThingsBoard(bool accessGranted, String name, String uid) {
  String payload = "{";
  payload += "\"access\": \"" + String(accessGranted ? "authorized" : "denied") + "\",";
  payload += "\"name\": \"" + name + "\",";
  payload += "\"uid\": \"" + uid + "\"";
  payload += "}";

  client1.publish("v1/devices/me/telemetry", payload.c_str());
  client2.publish("v1/devices/me/telemetry", payload.c_str());
  client3.publish("v1/devices/me/telemetry", payload.c_str());
}
