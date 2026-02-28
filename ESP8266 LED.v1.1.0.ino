#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "ESP_Light";
const char* password = "12345678";

ESP8266WebServer server(80);

#define PWM_PIN 14       // D5
#define BATTERY_PIN A0
#define BUZZER_PIN 5     // D1 (SAFE PIN)

int brightness = 0;
int currentOutput = 0;
int targetOutput = 0;
bool powerState = false;

unsigned long lastFadeUpdate = 0;
unsigned long lastVoltageUpdate = 0;
unsigned long lastBeep = 0;
unsigned long beepStart = 0;

bool beepActive = false;

float lastVoltage = 0.0;

// -------- Voltage --------
float readVoltage() {
  int raw = analogRead(BATTERY_PIN);
  if (raw <= 3) return 0.0;
  float adcVoltage = (raw / 1023.0) * 3.3;
  return adcVoltage * 2.0;
}

// -------- Battery % --------
int getBatteryPercent(float voltage) {
  float minV = 3.2;
  float maxV = 4.2;
  if (voltage <= minV) return 0;
  if (voltage >= maxV) return 100;
  return (int)(((voltage - minV) / (maxV - minV)) * 100);
}

// -------- Clean Fixed UI --------
const char page[] PROGMEM = R"====(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<style>
body{
margin:0;
background:#0c0c12;
font-family:-apple-system,BlinkMacSystemFont,"Segoe UI",Roboto;
display:flex;
justify-content:center;
align-items:center;
height:100vh;
color:white;
}
.card{
width:360px;
padding:35px;
border-radius:28px;
background:rgba(255,255,255,0.05);
box-shadow:0 20px 50px rgba(0,0,0,0.6);
}
.toggle{
width:100%;
height:55px;
border-radius:18px;
background:rgba(255,255,255,0.08);
position:relative;
cursor:pointer;
}
.knob{
position:absolute;
width:48px;
height:48px;
border-radius:14px;
background:white;
top:3.5px;
left:4px;
transition:0.3s;
}
.slider{
margin-top:35px;
width:100%;
appearance:none;
height:6px;
border-radius:5px;
background:rgba(255,255,255,0.2);
outline:none;
}
.slider::-webkit-slider-thumb{
appearance:none;
width:22px;
height:22px;
border-radius:50%;
background:white;
cursor:pointer;
}
.statusRow{
margin-top:30px;
display:flex;
justify-content:space-between;
font-size:20px;
}
</style>
</head>
<body>
<div class="card">

<div class="toggle" onclick="togglePower()">
<div class="knob" id="knob"></div>
</div>

<input type="range" min="0" max="1023" value="0"
class="slider" id="slider"
oninput="setBrightness(this.value)">

<div class="statusRow">
<div id="voltage">0.00 V</div>
<div id="percent">0%</div>
</div>

</div>

<script>
let power=false;

function syncState(){
fetch("/state")
.then(r=>r.json())
.then(data=>{
power=data.power;
document.getElementById("slider").value=data.brightness;
document.getElementById("knob").style.left=
power ? "calc(100% - 52px)" : "4px";
});
}

function togglePower(){
fetch("/toggle");
power=!power;
document.getElementById("knob").style.left=
power ? "calc(100% - 52px)" : "4px";
}

function setBrightness(val){
fetch("/brightness?value="+val);
}

function updateBattery(){
fetch("/battery")
.then(r=>r.json())
.then(data=>{
document.getElementById("voltage").innerHTML=data.voltage+" V";
document.getElementById("percent").innerHTML=data.percent+"%";
});
}

setInterval(updateBattery,2000);
window.onload = syncState;
</script>

</body>
</html>
)====";

// -------- Handlers --------
void handleRoot() {
  server.send_P(200, "text/html", page);
}

void handleToggle() {
  powerState = !powerState;
  targetOutput = powerState ? brightness : 0;
  server.send(200, "text/plain", "OK");
}

void handleBrightness() {
  if (server.hasArg("value")) {
    brightness = server.arg("value").toInt();
    if (powerState) targetOutput = brightness;
  }
  server.send(200, "text/plain", "OK");
}

void handleBattery() {
  int percent = getBatteryPercent(lastVoltage);
  String json = "{";
  json += "\"voltage\":" + String(lastVoltage,2) + ",";
  json += "\"percent\":" + String(percent);
  json += "}";
  server.send(200, "application/json", json);
}

void handleState() {
  String json = "{";
  json += "\"power\":" + String(powerState ? "true" : "false") + ",";
  json += "\"brightness\":" + String(brightness);
  json += "}";
  server.send(200, "application/json", json);
}

// -------- Setup --------
void setup() {

  pinMode(PWM_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  analogWriteRange(1023);
  analogWriteFreq(500);

  WiFi.setSleepMode(WIFI_LIGHT_SLEEP);
  WiFi.softAP(ssid, password);

  server.on("/", handleRoot);
  server.on("/toggle", handleToggle);
  server.on("/brightness", handleBrightness);
  server.on("/battery", handleBattery);
  server.on("/state", handleState);

  server.begin();
}

// -------- Loop --------
void loop() {

  server.handleClient();

  // Smooth Fade
  if (millis() - lastFadeUpdate > 5) {
    lastFadeUpdate = millis();
    if (currentOutput < targetOutput) {
      currentOutput++;
      analogWrite(PWM_PIN, currentOutput);
    }
    else if (currentOutput > targetOutput) {
      currentOutput--;
      analogWrite(PWM_PIN, currentOutput);
    }
  }

  // Voltage update
  if (millis() - lastVoltageUpdate > 2000) {
    lastVoltageUpdate = millis();
    lastVoltage = readVoltage();
  }

  // Smart Heartbeat Buzzer
  int percent = getBatteryPercent(lastVoltage);

  if (percent < 50) {

    unsigned long interval;

    if (percent > 30) interval = 8000;
    else if (percent > 20) interval = 5000;
    else if (percent > 10) interval = 3000;
    else interval = 1500;

    if (!beepActive && millis() - lastBeep > interval) {
      lastBeep = millis();
      tone(BUZZER_PIN, 2000);
      beepStart = millis();
      beepActive = true;
    }

    if (beepActive && millis() - beepStart > 80) {
      noTone(BUZZER_PIN);
      beepActive = false;
    }
  }
}