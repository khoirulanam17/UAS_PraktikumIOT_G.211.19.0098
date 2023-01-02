#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <stdlib.h>
#include "DHT.h"
#define DHTPIN D7
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);


WiFiClient espClient;
PubSubClient client(espClient);

// Deklarasi ssid, password, mqtt
const char* ssid = "OPPO F9";
const char* password = "asdfghjkl";
const char* mqtt_server = "riset.revolusi-it.com";
const char* topik = "iot/suhu";

String messages_terima;
String messages_kirim;

float nilai_suhu;



void konek_wifi()
{
WiFi.begin(ssid, password);
while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}
Serial.println("");
Serial.println("WiFi connected"); // cetak wifi terkoneksi
}



void reconnect() {
// Ulang terus sampai terkoneksi
while (!client.connected()) {
Serial.print("Menghubungkan diri ke MQTT Server : "+(String)mqtt_server);
// Attempt to connect
if (client.connect("G.211.19.0098")) { // gunakan NIM anda sebagai identitas client
Serial.println("connected");
// .subscribe ke topic
client.subscribe(topik);
} else {
Serial.print("failed, rc=");
Serial.print(client.state());
Serial.println(" coba lagi dalam 5 detik...");
// Wait 5 seconds before retrying
delay(5000);
}
}
}


float baca_suhu(){

 float kelembaban = dht.readHumidity();
float nilai_suhu = dht.readTemperature();
if ( nilai_suhu >= 29) {
    digitalWrite(D1, 1);
  }
  if ( nilai_suhu < 29) {
    digitalWrite(D1, 0);
  }
  if ( nilai_suhu >= 30) {
    digitalWrite(D2, 1);
  }
  if ( nilai_suhu < 30) {
    digitalWrite(D2, 0);
  }
  if ( nilai_suhu >= 31) {
    digitalWrite(D3, 1);
  }
  if ( nilai_suhu < 31) {
    digitalWrite(D3, 0);
  }
  Serial.println("Suhu: "+(String)nilai_suhu+" - Kelembaban : "+(String)kelembaban);
delay(1000);

  return nilai_suhu;
  
  }

void callback(char* topic, byte* payload, unsigned int length) {
Serial.print("Pesan dari MQTT [");
Serial.print(topic);
Serial.print("] ");
messages_terima="";
for (int i=0;i<length;i++) { // susun jadi string saja...
char receivedChar = (char)payload[i];
messages_terima=messages_terima+(char)payload[i]; // ambil pesannya masukkan dalam string
}
Serial.println(messages_terima);

float suhu=baca_suhu();

messages_kirim=(String)suhu;


char attributes[100];
messages_kirim.toCharArray(attributes, 100);
client.publish(topik,attributes,true);
Serial.println("messages :"+messages_kirim+ "terkirim...");
delay(1000);

}
// ------------------------------- begin program utama --------------------------------------- //
void setup()
{
Serial.begin(9600); // mulai komunikasi serial
client.setServer(mqtt_server, 1883); // sambungkan client ke mqtt
client.setCallback(callback); // interaksi callback
pinMode(D1, OUTPUT);
pinMode(D2, OUTPUT);
pinMode(D3, OUTPUT);

}

void loop()
{

if(WiFi.status() != WL_CONNECTED) { konek_wifi(); } // jika tidak konek wifi maka di reconnect
if (!client.connected()) { reconnect(); } // reconnect apabila belum konek
client.loop(); // lakukan client.loop()
};
