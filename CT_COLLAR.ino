/** ClevrThings COLLAR **/

/** user editable **/
#define NAME "COLLAR"
<<<<<<< Updated upstream
#define SOFTWARE_VERSION "1.0" // current software version
=======
#define SOFTWARE_VERSION "0.2.0" // current software version
#define UPDATE_URL "https://gitreleases.dev/gh/ricovdh/CT_COLLAR/latest/CT_COLLAR.bin"
>>>>>>> Stashed changes
#define VOLT_CORRECTION -0.10  // correct the voltage level
#define STAY_ON_TIME 10    // time to stay on before going to sleep if no message is received in seconds
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
int stay_on_time = STAY_ON_TIME * 1000;
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

/** WIFI **/
#include <WiFiManager.h>
bool wifi_connected = false;

void wifi_setup() {
    WiFi.mode(WIFI_STA);
    WiFiManager wm;

    //reset settings - wipe credentials for testing
    //wm.resetSettings();

     // id/name(not shown), placeholder/prompt, default, length
    WiFiManagerParameter custom_setting("server", "Sleep time", "30", 40 /**, "readonly"**/);
    wm.addParameter(&custom_setting);

    //wm.setCustomHeadElement("<style>html{filter: invert(100%); -webkit-filter: invert(100%);}</style>");

    //WiFiManagerParameter custom_text("<p>This is just a text paragraph</p>");
    //wm.addParameter(&custom_text); 

    //wm.setClass("invert"); // dark theme

    wm.setDebugOutput(false);
    bool res;
    res = wm.autoConnect("ClevrThings COLLAR","clevrthings");

    String sleeptime = custom_setting.getValue();
    sleep_time = sleeptime.toInt();
    
    if(!res) {
        Serial.println("WIFI failed to connect");
        wifi_connected = false;
        // ESP.restart();
    } 
    else {
        wifi_connected = true;
        //if you get here you have connected to the WiFi    
        Serial.println("WIFI: Connected");
    }

}

/** UPDATE **/
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
bool updating = false;

void update_started() {
  Serial.println("CALLBACK:  HTTP update process started");
}

void update_finished() {
  Serial.println("CALLBACK:  HTTP update process finished");
}

void update_progress(int cur, int total) {
  Serial.printf("CALLBACK:  HTTP update process at %d of %d bytes...\n", cur, total);
}

void update_error(int err) {
  Serial.printf("CALLBACK:  HTTP update fatal error code %d\n", err);
}

void update_loop() {
  // wait for WiFi connection
  if (wifi_connected) {

    WiFiClient client;

    // The line below is optional. It can be used to blink the LED on the board during flashing
    // The LED will be on during download of one buffer of data from the network. The LED will
    // be off during writing that buffer to flash
    // On a good connection the LED should flash regularly. On a bad connection the LED will be
    // on much longer than it will be off. Other pins than LED_BUILTIN may be used. The second
    // value is used to put the LED on. If the LED is on with HIGH, that value should be passed
    ESPhttpUpdate.setLedPin(led, HIGH);

    // Add optional callback notifiers
    ESPhttpUpdate.onStart(update_started);
    ESPhttpUpdate.onEnd(update_finished);
    ESPhttpUpdate.onProgress(update_progress);
    ESPhttpUpdate.onError(update_error);

    t_httpUpdate_return ret = ESPhttpUpdate.update(client, UPDATE_URL);
    // Or:
    //t_httpUpdate_return ret = ESPhttpUpdate.update(client, "server", 80, "file.bin");

    switch (ret) {
      case HTTP_UPDATE_FAILED:
        Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        break;

      case HTTP_UPDATE_NO_UPDATES:
        Serial.println("HTTP_UPDATE_NO_UPDATES");
        break;

      case HTTP_UPDATE_OK:
        Serial.println("HTTP_UPDATE_OK");
        break;
    }
  }
}

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
  digitalWrite(led, LOW);
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

/** commands **/
String commands(String message) {

  String return_msg;
  if (message.endsWith("vib1")) {
    vib1();
    return_msg = "vib1";
  }
  if (message.endsWith("vib2")) {
    vib2();
    return_msg = "vib2";
  }
  if (message.endsWith("vib3")) {
    vib3();
    return_msg = "vib3";
  }
  if (message.endsWith("vib4")) {
    vib4();
    return_msg = "vib4";
  }

  if (message.endsWith("buz1")) {
    buz1();
    return_msg = "buz1";
  }
  if (message.endsWith("buz2")) {
    buz2();
    return_msg = "buz2";
  }
  if (message.endsWith("buz3")) {
    buz3();
    return_msg = "buz3";
  }

  if (message.endsWith("update")) {
    updating = true;
    sleep_enabled = false;
    HC12_send("update");
    wifi_setup();
    message = "";    
  }

  return return_msg;
}

/** vibrator **/
void vib1() {
  digitalWrite(vibrator, HIGH);
  delay(1000);
  digitalWrite(vibrator, LOW);
}

void vib2() {
  digitalWrite(vibrator, HIGH);
  delay(500);
  digitalWrite(vibrator, LOW);
}

void vib3() {
  digitalWrite(vibrator, HIGH);
  delay(500);
  digitalWrite(vibrator, LOW);
  delay(500);
  digitalWrite(vibrator, HIGH);
  delay(500);
  digitalWrite(vibrator, LOW);
  delay(500);
}

void vib4() {
  digitalWrite(vibrator, HIGH);
  delay(200);
  digitalWrite(vibrator, LOW);
  delay(200);
  digitalWrite(vibrator, HIGH);
  delay(200);
  digitalWrite(vibrator, LOW);
  delay(200);
  digitalWrite(vibrator, HIGH);
  delay(200);
  digitalWrite(vibrator, LOW);
  delay(200);
}

/** buzzer **/
void buz1(){
  digitalWrite(buzzer, HIGH);
  delay(50);
  digitalWrite(buzzer, LOW);
}

void buz2(){
  digitalWrite(buzzer, HIGH);
  delay(50);
  digitalWrite(buzzer, LOW);
  delay(50);
  digitalWrite(buzzer, HIGH);
  delay(50);
  digitalWrite(buzzer, LOW);
}

void buz3(){
  digitalWrite(buzzer, HIGH);
  delay(50);
  digitalWrite(buzzer, LOW);
  delay(50);
  digitalWrite(buzzer, HIGH);
  delay(50);
  digitalWrite(buzzer, LOW);
  delay(50);
  digitalWrite(buzzer, HIGH);
  delay(50);
  digitalWrite(buzzer, LOW);
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

  if (millis() > (on_time + stay_on_time)) {
      sleep();
  }

  if (updating) {
    update_loop();
  }
}
