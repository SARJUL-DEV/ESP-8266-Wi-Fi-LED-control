#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "ESP_Light";
const char* password = "12345678";

ESP8266WebServer server(80);

#define PWM_PIN 14
#define BATTERY_PIN A0

int brightness = 0;
int currentOutput = 0;
int targetOutput = 0;
bool powerState = false;

unsigned long lastFadeUpdate = 0;
unsigned long lastVoltageUpdate = 0;
float lastVoltage = 0.00;

// -------- Voltage Reading --------
float readVoltage() {
  int raw = analogRead(BATTERY_PIN);

  if (raw <= 3) return 0.00;

  float adcVoltage = (raw / 1023.0) * 3.3;

  // Adjust ratio according to your resistor divider
  // Example for 100k + 100k divider
  float actualVoltage = adcVoltage * 2.0;

  return actualVoltage;
}

// -------- Web Page --------
void handleRoot() {

String page = R"====(
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
width:350px;
padding:35px;
border-radius:28px;
background:rgba(255,255,255,0.05);
backdrop-filter:blur(25px);
box-shadow:0 25px 60px rgba(0,0,0,0.7);
}
.toggle{
width:100%;
height:55px;
border-radius:18px;
background:rgba(255,255,255,0.08);
position:relative;
cursor:pointer;
transition:0.3s;
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
box-shadow:0 5px 15px rgba(0,0,0,0.4);
}
.voltage{
margin-top:30px;
font-size:22px;
text-align:center;
opacity:0.9;
}
</style>
</head>
<body>
<div class="card">

<div class="toggle" onclick="togglePower()">
<div class="knob" id="knob"></div>
</div>

<input type="range" min="0" max="1023" value="0" class="slider" id="slider" oninput="setBrightness(this.value)">

<div class="voltage" id="voltage">0.00 V</div>

</div>

<script>

let power=false;

function syncState(){
fetch("/state")
.then(response=>response.json())
.then(data=>{
power=data.power;
document.getElementById("slider").value=data.brightness;
document.getElementById("knob").style.left= power ? "calc(100% - 52px)" : "4px";
});
}

function togglePower(){
fetch("/toggle");
power=!power;
document.getElementById("knob").style.left= power ? "calc(100% - 52px)" : "4px";
}

function setBrightness(val){
fetch("/brightness?value="+val);
}

function updateVoltage(){
fetch("/voltage")
.then(response=>response.text())
.then(data=>{
document.getElementById("voltage").innerHTML=data+" V";
});
}

setInterval(updateVoltage,1000);
window.onload = syncState;

</script>
</body>
</html>
)====";

server.send(200, "text/html", page);
}

// -------- Handlers --------
void handleToggle() {
  powerState = !powerState;

  if (!powerState) targetOutput = 0;
  else targetOutput = brightness;

  server.send(200, "text/plain", "OK");
}

void handleBrightness() {
  if (server.hasArg("value")) {
    brightness = server.arg("value").toInt();
    if (powerState) targetOutput = brightness;
  }
  server.send(200, "text/plain", "OK");
}

void handleVoltage() {
  server.send(200, "text/plain", String(lastVoltage, 2));
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
  analogWriteRange(1023);
  analogWriteFreq(1000);

  WiFi.softAP(ssid, password);

  server.on("/", handleRoot);
  server.on("/toggle", handleToggle);
  server.on("/brightness", handleBrightness);
  server.on("/voltage", handleVoltage);
  server.on("/state", handleState);

  server.begin();
}

// -------- Main Loop --------
void loop() {

  server.handleClient();

  // Smooth non-blocking fade
  if (millis() - lastFadeUpdate > 2) {
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

  // Voltage update every 1 second
  if (millis() - lastVoltageUpdate > 1000) {
    lastVoltageUpdate = millis();
    lastVoltage = readVoltage();
  }
}