
#define BLYNK_PRINT Serial
#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>
#include <FreqMeasure.h>

int pinPir = 7;
int pinRelay = 4;
bool nilaiPir = false;
int baca;
double sum = 0;
int count = 0;

char auth[] = "InAED30dkbxEVRhTduynAS_ROb-vg4MR";

char ssid[] = "AndroidAP";
char pass[] = "10102000";

#include <SoftwareSerial.h>
SoftwareSerial EspSerial(2, 3); // RX, TX

#define ESP8266_BAUD 115200
ESP8266 wifi(&Serial);

void setup()
{
  Serial.begin(ESP8266_BAUD);
  delay(10);
  FreqMeasure.begin();
  Blynk.begin(auth, wifi, ssid, pass);
  pinMode(pinPir, INPUT);
  pinMode(pinRelay, OUTPUT);
}

void loop()
{
  if (FreqMeasure.available()) {
    baca = digitalRead(pinPir);                                                           // Membaca senosor PIR
    sum = sum + FreqMeasure.read();
    count = count + 1;
    if (count > 30) {                                                                    // Pembacaan sejumlah 30x
      float frequency = FreqMeasure.countToFrequency(sum / count);
      float spd = frequency / 19.49;                                                    //konversi nilai frekuensi ke km/jam
      //to improve speed, we update only the bottom row of the LCD
      Serial.print("F= ");
      Serial.print(frequency);
      Serial.print("Hz ");
      Serial.print("SPD= ");
      Serial.print(spd);
      Serial.println("km/h");
      sum = 0;
      count = 0;
      Blynk.virtualWrite(V5, frequency);                                              // Menampilkan nilai frekuensi pada dashboard Blynk
      Blynk.virtualWrite(V6, spd);                                                    // Menampilkan Kecepatan dalam KM/JAM pada dashboard Blynk

      if ((baca == HIGH)) {                                                           // mendeteksi dengan Sensor PIR 


        if (nilaiPir == HIGH) {
          Blynk.notify("Sensor PIR Motion Detected, Relay is ON");                    // Mengirimkan notifikasi pada Blynk
          Blynk.virtualWrite(V0, nilaiPir);                                           // Menampilkan nilai sensor PIR pada Blynk
          digitalWrite(pinRelay, LOW);                                                // Menghidupkan Relay
          delay(5000);                                                                // Lampu menyala selama 5 detik
          nilaiPir = LOW;                                                             // Mereset kembali nilai PIR
        }

      } else if ((baca == HIGH) && (frequency > 10)) {                                // Mendeteksi gerakan dengan Sensor PIR dan HB100

        if (nilaiPir == HIGH) {                                                       
          Blynk.notify("Both Sensor detected , Relay is ON");
          Blynk.virtualWrite(V0, nilaiPir);
          digitalWrite(pinRelay, LOW);
          delay(5000);
          nilaiPir = LOW;
        }

      } else if ((baca == LOW) && (frequency > 10)) {                                 // Mendeteksi gerakan dengan Sensor HB100

        if (nilaiPir == LOW) {
          Blynk.notify("HB Motion detected, Relay is ON");
          Blynk.virtualWrite(V0, nilaiPir);
          digitalWrite(pinRelay, LOW);
          delay(5000);
          nilaiPir = HIGH;
        }
      } else {
        digitalWrite(pinRelay, HIGH);
        delay(2000);

        if (nilaiPir == LOW) {
          Blynk.notify("Motion Undetected, Relay is OFF");
          Blynk.virtualWrite(V0, nilaiPir);
          //      digitalWrite(pinRelay, HIGH);                         // tidak Terdeteksi gerakan
          //      delay(5000);
          nilaiPir = HIGH;
        }

      }
    }
  }

  Blynk.run();
}
