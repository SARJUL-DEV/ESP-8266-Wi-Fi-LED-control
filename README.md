# ESP8266 Smart PWM LED Controller

A beginner-friendly WiFi controlled LED dimmer using ESP8266 and MOSFET with smooth fading and live voltage monitoring.

---

# What This Project Does

- Control LED brightness from phone
- Turn LED ON / OFF from browser
- Smooth fade without lag
- Show live battery voltage
- Works without internet (Access Point mode)

---

# Required Components

| Component | Quantity |
|-----------|----------|
| ESP8266 NodeMCU | 1 |
| Logic Level N-Channel MOSFET (IRLZ44N / IRLZ34N / AO3400 etc.) | 1 |
| LED Strip / LED Load | 1 |
| External Power Supply for LED | 1 |
| Resistor 100kΩ | 2 |
| Wires | As needed |

---

# Important Safety

Never connect 4.2V battery directly to A0.  
You MUST use a voltage divider.

---

# Step 1: MOSFET Connection (LED Control)

Follow this exactly:

```
External Power +  ----->  LED +

LED -              ----->  MOSFET Drain

MOSFET Source      ----->  GND

MOSFET Gate        ----->  D5 (GPIO14)

ESP8266 GND        ----->  Power Supply GND
```

Important:
ESP8266 GND and Power Supply GND must be connected together.

---

# Step 2: Battery Voltage Divider (Very Important)

We use two 100k resistors.

```
Battery +  ---- R1 (100k) ----+---- A0
                               |
                             R2 (100k)
                               |
                              GND
```

R1 = 100k  
R2 = 100k  

This divides voltage safely.

Why?
ESP8266 A0 maximum input = 3.3V  
Battery maximum = 4.2V  
Divider protects the pin.

---

# Step 3: Full Wiring Overview

```
                 +--------------------+
                 |      ESP8266       |
                 |                    |
Battery + ----R1-+---- A0             |
Battery - ----------- GND              |
                 |                    |
D5 (GPIO14) -------- MOSFET Gate       |
GND ---------------- MOSFET Source     |
                 +--------------------+

External Power + ---- LED +
LED - ------------- MOSFET Drain
```

---

# Step 4: Upload Firmware

1. Install ESP8266 board package in Arduino IDE
2. Select NodeMCU 1.0
3. Paste firmware code
4. Upload

---

# Step 5: Connect

After upload:

1. Turn on ESP8266
2. Connect WiFi: ESP_Light
3. Password: 12345678
4. Open browser
5. Go to: 192.168.4.1

---

# How It Works

- Slider controls PWM signal on D5
- MOSFET switches LED power
- Smooth fade runs in background
- Voltage updates every second
- State remains after refresh

---

# Common Mistakes

If voltage shows 0.00:

- Battery negative not connected to ESP GND
- Divider wired wrong
- Only one resistor used
- Wrong resistor values

If LED does not dim:

- MOSFET not logic-level type
- Gate not connected to D5
- Ground not shared

---

# Recommended MOSFET Types

- IRLZ44N
- IRLZ34N
- AO3400
- Any logic-level N-channel MOSFET

Avoid:
IRFZ44N (not ideal for 3.3V logic)

---

# Final Result

- Smooth brightness
- No hanging
- Live voltage
- Clean web UI
- Beginner friendly wiring

---

# License

Free for learning and personal projects.
