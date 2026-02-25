# ESP8266 Smart PWM LED Controller

Smooth WiFi LED dimmer with live voltage monitoring and non-blocking fade system.

---

## Live UI Preview

You can preview the web interface design directly:

👉 [Open Web UI Preview](./index.html)

---

## Project Structure

```
ESP8266-Smart-PWM-Light/
│
├── README.md
├── firmware/
│   └── esp8266_smart_pwm.ino
│
└── web/
    └── index.html
```

---

## Features

- Smooth non-blocking PWM dimming
- Power state memory
- Brightness memory
- Live voltage display
- Clean minimal UI
- Access Point mode
- Beginner friendly wiring

---

## Hardware Required

| Component | Quantity |
|-----------|----------|
| ESP8266 NodeMCU | 1 |
| Logic Level MOSFET | 1 |
| LED Load | 1 |
| 100kΩ Resistor | 2 |
| External LED Power | 1 |

---

## Wiring Guide

### MOSFET Connection

```
External Power +  ----->  LED +

LED -              ----->  MOSFET Drain

MOSFET Source      ----->  GND

MOSFET Gate        ----->  D5 (GPIO14)

ESP8266 GND        ----->  Power Supply GND
```

---

### Voltage Divider (Mandatory)

```
Battery +  ---- R1 (100k) ----+---- A0
                               |
                             R2 (100k)
                               |
                              GND
```

---

## Firmware Location

Full firmware code available here:

👉 [Open Firmware File](./ESP8266 LED controller.ini)

---

## How To Use

1. Upload firmware
2. Connect to WiFi: ESP_Light
3. Open browser: 192.168.4.1
4. Control brightness

---

## Notes

- Always share ground between ESP and power supply
- Use logic-level MOSFET
- Adjust voltage multiplier if using different resistor values

---

## License

Free for personal and educational use.
