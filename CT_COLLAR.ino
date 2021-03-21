/** ClevrThings COLLAR **/

/** user editable **/
#define NAME "COLLAR"
#define SOFTWARE_VERSION "0.1.0" // current software version
#define VOLT_CORRECTION -0.10  // correct the voltage level
#define STAY_ON_TIME 5    // time to stay on before going to sleep if no message is received in seconds
#define SLEEP_TIME 60     // how long to sleep in seconds
#define ENABLE_SLEEP true // set to false to disable sleep
#define MSG_BUFFER_SIZE 100 // the maximum length of a message to send

/** inputs & outputs **/
#define hc12_rx 14
#define hc12_tx 12
#define hc12_set 13
#define led 2
#define vibrator 4
#define buzzer 5

/** global variables **/
int chipID;
ADC_MODE(ADC_VCC);
char voltage_char[] = "0.00";
float voltage = 0.00;
float voltage_corection = VOLT_CORRECTION;
int stay_on_time = STAY_ON_TIME / 1000;
unsigned long on_time = 0; // current on time
int sleep_time = SLEEP_TIME;
bool sleep_enabled = ENABLE_SLEEP;
String name_dash;

/** HC-12 **/
#include <SoftwareSerial.h>
SoftwareSerial HC12(hc12_rx, hc12_tx);
byte incomingByte;
String readBuffer = "";
char hc12_message[100];

/** FUNCTIONS **/
float get_voltage() {
  float v = ESP.getVcc();
  v = (v/1000) + voltage_corection;
  String volt = "";
  volt += v;
  volt.toCharArray(voltage_char, volt.length()+1);
  return v;
}

void esp_sleep() {
  Serial.printf("ESP sleep for %d seconds\n\n", sleep_time);
  delay(200);
  ESP.deepSleep(sleep_time*1000000);
}

void HC12_current_settings() {
  HC12_CmdMode(true);
  HC12.print("AT+RX");
  delay(500);
  Serial.printf("HC12 settings:\n");
  while (HC12.available()) {
    Serial.write(HC12.read());
    delay(10);
  }
  HC12_CmdMode(false);
  Serial.println();
}

void HC12_CmdMode(bool state) {
  if (state) {
    digitalWrite(hc12_set, 0);
    delay(40);
  }
  else {
    digitalWrite(hc12_set, 1);
    delay(80);
  }
}

void HC12_send(char* message) {
  char MESSAGE[MSG_BUFFER_SIZE];
  String msg = NAME;
  msg += "/";
  name_dash = msg;
  msg += chipID;
  msg += ",";
  msg += voltage;
  msg += ",";
  msg += message;
  msg.toCharArray(MESSAGE, msg.length()+1);  

  HC12_CmdMode(false);
  HC12.write(MESSAGE);
  delay(100);
  while (HC12.available()) {
    Serial.write(HC12.read());
  }
  Serial.printf("HC12 send: %s\n", MESSAGE);
}

bool HC12_receive() {
  if (HC12.available()) {
    while (HC12.available()) {
      incomingByte = HC12.read();
      readBuffer += char(incomingByte);
      delay(50);
    }
    readBuffer.toCharArray(hc12_message, readBuffer.length() + 1);
    Serial.printf("HC12 message received, data: %s\n", hc12_message);
  }
}

void HC12_sleep() {
  Serial.println();
  HC12_CmdMode(true);
  HC12.print("AT+SLEEP");
  delay(100);
  while (HC12.available()) {
    Serial.write(HC12.read());
  }
  Serial.println("\nHC12 off");
}

void sleep() {
    if (sleep_enabled){
    HC12_sleep();
    esp_sleep();
    Serial.print("\n\n");
  }
}

/** RUNTIME **/
void setup() {
  chipID = ESP.getChipId();
  Serial.begin(9600);
  voltage = get_voltage();
  Serial.printf("\n** ClevrThings %s V%s **\n\nBattery: %sv\n\n", NAME, SOFTWARE_VERSION, voltage_char);

  pinMode(led, OUTPUT);
  pinMode(vibrator, OUTPUT);
  pinMode(buzzer, OUTPUT);

  pinMode(hc12_set, OUTPUT);
  HC12.begin(1200);
  HC12_current_settings();

  char number[] = "000";
  itoa(random(100), number, 3);
  HC12_send("ok");


  on_time = millis();
}

void loop() {
  HC12_receive();
  if (readBuffer.startsWith(name_dash + chipID)) {
    if (readBuffer.endsWith(",")) {
      sleep();
    }
    else {
      char message[10];
      commands(readBuffer).toCharArray(message, 10);
      HC12_send(message);
      sleep();
    }
  }

  if (millis() > (on_time + stay_on_time*1000)) {
      sleep();
  }
}
