#include <WiFi.h>
#include "FastLED.h"
#include "pitches.h"
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#define NUM_LEDS 1
CRGB leds[NUM_LEDS];
// Data pin that led data will be written out over
#define LED_DATA_PIN 6
#define BUZZER_PIN 2
#define DETONATE_PIN 12

// Replace with your network credentials
const char* ssid = "HspPhone";
const char* password = "eprue123";

const char* PARAM_INPUT_1 = "output";
const char* PARAM_INPUT_2 = "state";

int melody[] = {
  NOTE_E5, NOTE_E5, NOTE_E5,
  NOTE_E5, NOTE_E5, NOTE_E5,
  NOTE_E5, NOTE_G5, NOTE_C5, NOTE_D5,
  NOTE_E5,
  NOTE_F5, NOTE_F5, NOTE_F5, NOTE_F5,
  NOTE_F5, NOTE_E5, NOTE_E5, NOTE_E5, NOTE_E5,
  NOTE_E5, NOTE_D5, NOTE_D5, NOTE_E5,
  NOTE_D5, NOTE_G5
};

int durations[] = {
  8, 8, 4,
  8, 8, 4,
  8, 8, 8, 8,
  2,
  8, 8, 8, 8,
  8, 8, 8, 16, 16,
  8, 8, 8, 8,
  4, 4
};
bool playingBeep=false;
// Set web server port number to 80
AsyncWebServer server(80);
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    h2 {font-size: 3.0rem;}
    p {font-size: 3.0rem;}
    body {max-width: 600px; margin:0px auto; padding-bottom: 25px;}
    .switch {position: relative; display: inline-block; width: 120px; height: 68px} 
    .switch input {display: none}
    .slider {position: absolute; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; border-radius: 6px}
    .slider:before {position: absolute; content: ""; height: 52px; width: 52px; left: 8px; bottom: 8px; background-color: #fff; -webkit-transition: .4s; transition: .4s; border-radius: 3px}
    input:checked+.slider {background-color: #b30000}
    input:checked+.slider:before {-webkit-transform: translateX(52px); -ms-transform: translateX(52px); transform: translateX(52px)}
  </style>
</head>
<body>
  <h2>ESP Web Server</h2>
  %BUTTONPLACEHOLDER%
<script>function toggleCheckbox(element) {
  var xhr = new XMLHttpRequest();
  if(element.checked){ xhr.open("GET", "/update?output="+element.id+"&state=1", true); }
  else { xhr.open("GET", "/update?output="+element.id+"&state=0", true); }
  xhr.send();
}
</script>
</body>
</html>
)rawliteral";
String outputState(int output){
  if(digitalRead(output)){
    return "checked";
  }
  else {
    return "";
  }
}
String musicState(){
  if(playingBeep){
    return "checked";
  }
  else {
    return "";
  }
}
String processor(const String& var){
  //Serial.println(var);
  if(var == "BUTTONPLACEHOLDER"){
    String buttons = "";
    buttons += "<h4>Output - Detonate</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\""+String(DETONATE_PIN)+"\" " + outputState(DETONATE_PIN) + "><span class=\"slider\"></span></label>";
    buttons += "<h4>Output - Play beep instead of music</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"9991\" " + musicState() + "><span class=\"slider\"></span></label>";
    return buttons;
  }
  return String();
}

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;
const long songTime = 0;
const long beepDelay = 800;

void setup() {
  delay(2000);
  Serial.begin(921600);
  FastLED.addLeds<SM16703, LED_DATA_PIN, BRG>(leds, NUM_LEDS);
  pinMode(BUZZER_PIN,OUTPUT);
  digitalWrite(BUZZER_PIN,LOW);
  pinMode(DETONATE_PIN,OUTPUT);
  digitalWrite(DETONATE_PIN,LOW);
  pinMode(LED_DATA_PIN,OUTPUT);
  digitalWrite(LED_DATA_PIN,LOW);

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());


  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // Send a GET request to <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
  server.on("/update", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage1;
    String inputMessage2;
    // GET input1 value on <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
    if (request->hasParam(PARAM_INPUT_1) && request->hasParam(PARAM_INPUT_2)) {
      inputMessage1 = request->getParam(PARAM_INPUT_1)->value();
      inputMessage2 = request->getParam(PARAM_INPUT_2)->value();
      if(inputMessage1.toInt()-9990==1){
        playingBeep=inputMessage2.toInt();
      }
      else{
        digitalWrite(inputMessage1.toInt(), inputMessage2.toInt());
      }
    }
    else {
      inputMessage1 = "No message sent";
      inputMessage2 = "No message sent";
    }
    Serial.print("GPIO: ");
    Serial.print(inputMessage1);
    Serial.print(" - Set to: ");
    Serial.println(inputMessage2);
    request->send(200, "text/plain", "OK");
  });

  // Start server
  server.begin();
}

void loop(){
  if(playingBeep){
    tone(BUZZER_PIN, 1000);
    leds[0] = CRGB::Red;
    FastLED.show();
    delay(beepDelay); 
    noTone(BUZZER_PIN); 
    leds[0] = CRGB::Black;
    FastLED.show();
    delay(beepDelay);  
    return;
  }
  int size = sizeof(durations) / sizeof(int); 
  for (int note = 0; note < size; note++) { 
    //to calculate the note duration, take one second divided by the note type. 
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc. 
    int duration = 1000 / durations[note]; 
    leds[0] = CRGB::Red;
    FastLED.show();
    tone(BUZZER_PIN, melody[note], duration); 
    leds[0] = CRGB::Blue;
    FastLED.show();
    //to distinguish the notes, set a minimum time between them. 
    //the note's duration + 30% seems to work well: 
    int pauseBetweenNotes = duration * 1.30; 
    delay(pauseBetweenNotes); 
    //stop the tone playing: 
    noTone(BUZZER_PIN); 
  } 

}