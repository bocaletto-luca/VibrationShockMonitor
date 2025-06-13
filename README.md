# Vibration & Shock Monitor with LoRaWAN
#### Author: Bocaletto Luca

**Files**  
- `README.md`  
- `VibrationShockMonitor.ino`

## 1. Concept  
Continuously sample a piezo vibration sensor and a 6-axis MPU-6050 accelerometer.  
• **Vibration event**: piezo voltage > threshold → check accelerometer.  
• **Shock event**: acceleration magnitude > threshold → package event data and send via LoRaWAN.  
• **LoRaWAN**: ABP (Activation By Personalization) on 868 MHz (EU).  
• **Status LED** shows join and send activity.

## 2. Bill of Materials  
- 1 × Arduino MKR WAN 1300 (integrated LoRa 868 MHz)  
- 1 × Piezo vibration sensor (analog output)  
- 1 × MPU-6050 6-axis IMU (I²C breakout)  
- 1 × LED + 220 Ω resistor  
- 5 V USB power (or battery)  
- Breadboard & jumper wires  

## 3. Wiring Diagram

    Arduino MKR WAN 1300            Sensor/Actuator
    ┌─────────────────────────┐      ┌──────────────┐
    │ A1  ──► Piezo signal    │─────►│ Piezo sensor │
    │ GND ──► Piezo GND       │      └──────────────┘
    │ 5V  ──► Piezo VCC       │      
    ├─────────────────────────┤      
    │ SDA ──► MPU6050 SDA     │─────►│ MPU6050      │
    │ SCL ──► MPU6050 SCL     │─────►│ (I²C)        │
    │ 5V  ──► MPU6050 VCC     │      └──────────────┘
    │ GND ──► MPU6050 GND     │      
    ├─────────────────────────┤      
    │ D6  ──►─[220 Ω]─► LED ─► GND │ Status LED  
    └─────────────────────────┘      

## 4. Software Setup  
1. In Arduino IDE install **MKRWAN** library (LoRaWAN support).  
2. Install **MPU6050** and **I2Cdev** libraries.  
3. Create folder `VibrationShockMonitor/` and save:
   - `README.md` (this file)  
   - `VibrationShockMonitor.ino` (source code below)  
4. Open `.ino` in Arduino IDE, select **Arduino MKR WAN 1300**, set your LoRa **DevEUI**, **AppEUI**, **AppKey**.  

## 5. Usage  
1. Upload sketch.  
2. Open Serial Monitor @115200 baud.  
3. Wait for “LoRa joined” confirmation (LED blinks twice).  
4. On each vibration over threshold + shock over threshold, a packet is sent; LED blinks once.  
5. Monitor your LoRaWAN network server for incoming messages containing:
   ```json
   {
     "piezo": 732,        // raw analog reading 0–1023
     "accel_g": 2.45      // peak g-force
   }
