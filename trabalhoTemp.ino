#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// Dados da rede e servidor MQTT
const char* ssid = "WIFI-ALUNOS-INOVACAO";
const char* password = "";
const char* mqtt_server = "broker.mqtt.cool";

// Definição do pino do DHT11 e inicialização
#define DHTPIN 25
DHT dht(DHTPIN, DHT11);

WiFiClient campainha;
PubSubClient client(campainha);

void setup_wifi() {
    delay(10);
    Serial.println();
    Serial.print("Conectando a ");
    Serial.println(ssid);

    WiFi.mode(WIFI_STA);
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

void callback(char* topic, byte* payload, unsigned int length) {
    String conc_payload;
    for (int i = 0; i < length; i++) {
        conc_payload += ((char)payload[i]);
    }
    Serial.println("Mensagem recebida no tópico: " + String(topic));
    Serial.println("Payload: " + conc_payload);
}

void reconnect() {
    while (!client.connected()) {
        Serial.print("Tentando conexão MQTT...");
        if (client.connect("campainha")) {
            Serial.println("conectado");
            client.subscribe("bobbyjones/campainha");
        } else {
            Serial.print("falhou, rc=");
            Serial.print(client.state());
            Serial.println(" tentando novamente em 5 segundos");
            delay(5000);
        }
    }
}

void setup() {
    Serial.begin(115200);
    dht.begin(); // Inicializa o sensor DHT11
    setup_wifi();
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
}

void loop() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop();

    float temperature = dht.readTemperature(); // Lê a temperatura
    if (!isnan(temperature)) { // Verifica se a leitura é válida
        String tempString = String(temperature);
        client.publish("bobbyjones/temp", tempString.c_str()); // Publica a temperatura
        Serial.println("Temperatura publicada: " + tempString + "°C");
    } else {
        Serial.println("Falha na leitura do sensor DHT11");
    }

    delay(2000); // Aguarda 2 segundos para a próxima leitura
}
