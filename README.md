# 🔥 ESP8266 Smart LED + Smart Heartbeat Buzzer System

Advanced PWM LED Controller with Smart Battery Monitoring  
Accurate Wiring Based on Final Firmware Code

---

# ✅ PIN CONFIGURATION (FROM ACTUAL CODE)

| Function        | GPIO | NodeMCU Pin |
|---------------|------|-------------|
| LED PWM       | 14   | D5          |
| Buzzer (Tone) | 5    | D1          |
| Battery Sense | A0   | A0          |

PWM Frequency: 500Hz  
PWM Range: 0–1023  

---

# 🧠 SYSTEM WORKING

ESP8266 creates:

PWM signal → GPIO14 → MOSFET → High Power LED  
Tone signal → GPIO5 → Passive Buzzer  
Battery → Voltage Divider → A0  

If battery drops below 50% → smart interval beeping starts  
Lower battery = faster beep  

---

# 🔌 COMPLETE HARDWARE DIAGRAM

==============================
SECTION 1: LED + MOSFET DRIVER
==============================

Use Logic Level N-Channel MOSFET  
Example: IRLZ44N / AO3400

MOSFET Front View:

        _______
       |       |
       |       |
       |_______|
         G  D  S

G = Gate  
D = Drain  
S = Source  

CONNECTIONS:

D5 (GPIO14) → 220Ω → Gate  
Gate → 10kΩ → GND  

Battery + → LED +  
LED − → Drain  
Source → GND  
Battery − → GND  

IMPORTANT:
ESP8266 GND and Battery GND must be connected together.

DO NOT connect LED directly to ESP8266.

---

=================================
SECTION 2: BATTERY VOLTAGE MONITOR
=================================

Voltage Divider (Your code uses ×2 scaling)

Battery + → 100kΩ → A0 → 100kΩ → GND  

Why?
ESP8266 A0 max input = 3.3V  
Battery max = 4.2V  
Divider cuts voltage in half.

Formula used in code:

Battery Voltage = (A0 / 1023) × 3.3 × 2

---

=================================
SECTION 3: BUZZER CONNECTION (VERY IMPORTANT)
=================================

Your code uses:

tone(BUZZER_PIN, 2000);

That means you MUST use:

✔ PASSIVE BUZZER  
✘ NOT Active Buzzer  

Connection:

D1 (GPIO5) → Buzzer +
Buzzer − → GND

No resistor required.

If buzzer is large or loud speaker type:
Use NPN transistor driver.

---

# 🎵 SMART HEARTBEAT BUZZER LOGIC

Battery > 50% → No beep  
30–50% → Beep every 8 sec  
20–30% → Beep every 5 sec  
10–20% → Beep every 3 sec  
<10% → Beep every 1.5 sec  

Each beep duration = 80ms  
Tone frequency = 2000Hz  

This is software-controlled interval alert system.

---

# 💡 PWM LED FADE SYSTEM

Your loop creates smooth fade:

Every 5ms:
currentOutput moves 1 step toward targetOutput

Result:
Soft brightness transition  
No sudden jump  

---

# ⚡ POWER ARCHITECTURE

Battery +  
   ├── LED circuit (via MOSFET)
   └── ESP8266 Vin (if 5V regulated)

Common Ground required everywhere.

---

# 🚫 DO NOT DO THIS

✘ Do not use Active Buzzer (won’t work with tone properly)
✘ Do not skip 10k Gate pull-down
✘ Do not connect battery directly to A0
✘ Do not power LED from ESP8266 pin
✘ Do not forget common ground

---

# 🔥 OPTIONAL PROFESSIONAL IMPROVEMENTS

- Add battery < 5% auto LED shutdown
- Add deep sleep mode
- Add OTA update
- Add OLED display
- Add current sensor (INA219)
- Add over-temperature cutoff

---

# 👤 Author

Sarjul  
Smart WiFi LED Controller with Adaptive Battery Alert System
