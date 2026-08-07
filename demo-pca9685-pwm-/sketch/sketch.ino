// #include <Wire.h>

// void setup()
// {
//     Serial.begin(115200);
//     delay(1000);

//     Wire.begin();

//     Serial.println("Scanning I2C...");

//     for (byte addr = 1; addr < 127; addr++)
//     {
//         Wire.beginTransmission(addr);

//         if (Wire.endTransmission() == 0)
//         {
//             Serial.print("Found device: 0x");
//             Serial.println(addr, HEX);
//         }
//     }

//     Serial.println("Done");
// }

// void loop()
// {
// }



// #include <Wire.h>
// #include <Adafruit_PWMServoDriver.h>

// Adafruit_PWMServoDriver pca(0x40);

// #define SERVO_FREQ 50

// #define SERVO_MIN_US 500
// #define SERVO_MAX_US 2500

// void setServoAngle(uint8_t channel, int angle)
// {
//     angle = constrain(angle, 0, 180);

//     int pulse_us = map(angle,
//                        0,180,
//                        SERVO_MIN_US,
//                        SERVO_MAX_US);

//     int counts =
//         pulse_us * 4096 / 20000;

//     pca.setPWM(channel,0,counts);
// }

// void setup()
// {
//     Serial.begin(115200);

//     Wire.begin();

//     pca.begin();
//     pca.setPWMFreq(SERVO_FREQ);

//     delay(500);

//     Serial.println("Moving Servo...");

//     setServoAngle(0,90);
// }

// void loop()
// {
//       for(int a=0;a<=180;a++)
//     {
//         setServoAngle(0,a);
//         delay(20);
//     }

//     for(int a=180;a>=0;a--)
//     {
//         setServoAngle(0,a);
//         delay(20);
//     }
// }


#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <Arduino_RouterBridge.h>

Adafruit_PWMServoDriver pca = Adafruit_PWMServoDriver(0x40);

#define SERVO_FREQ_HZ 50
#define PWM_FULL 4095

// PCA9685 channel map
#define CH_SERVO1 0
#define CH_SERVO2 1
#define CH_SERVO3 2
#define CH_SERVO4 3

#define CH_MOTOR_L 4
#define CH_MOTOR_R 5

#define CH_LED1 6
#define CH_LED2 7

#define CH_ANALOG1 8
#define CH_ANALOG2 9

#define CH_TRIAC1 10
#define CH_TRIAC2 11

#define CH_RELAY1 12
#define CH_RELAY2 13

#define CH_SPARE1 14
#define CH_SPARE2 15

// Servo pulse range
#define SERVO_MIN_US 500
#define SERVO_MAX_US 2500
#define SERVO_PERIOD_US 20000

bool sine1_on = false;
bool sine2_on = false;

float sine1_freq = 0.5;
float sine2_freq = 1.0;

unsigned long sine_start_ms = 0;

void setRawPWM(uint8_t ch, uint16_t value) {
  value = constrain(value, 0, PWM_FULL);
  pca.setPWM(ch, 0, value);
}

void setPercent(uint8_t ch, int percent) {
  percent = constrain(percent, 0, 100);
  uint16_t value = map(percent, 0, 100, 0, PWM_FULL);
  setRawPWM(ch, value);
}

void setServoAngle(uint8_t ch, int angle) {
  angle = constrain(angle, 0, 180);
  int us = map(angle, 0, 180, SERVO_MIN_US, SERVO_MAX_US);
  int count = (int)((float)us * 4096.0 / SERVO_PERIOD_US);
  setRawPWM(ch, count);
}

void setAnalogVoltage(uint8_t ch, float volts) {
  volts = constrain(volts, 0.0, 5.0);
  uint16_t value = (uint16_t)((volts / 5.0) * PWM_FULL);
  setRawPWM(ch, value);
}

void allOff() {
  // Servos to center
  setServoAngle(CH_SERVO1, 90);
  setServoAngle(CH_SERVO2, 90);
  setServoAngle(CH_SERVO3, 90);
  setServoAngle(CH_SERVO4, 90);

  // PWM outputs off
  setPercent(CH_MOTOR_L, 100);
  setPercent(CH_MOTOR_R, 50);
  setPercent(CH_LED1, 0);
  setPercent(CH_LED2, 0);

  // Analog outputs to 0V
  setAnalogVoltage(CH_ANALOG1, 0);
  setAnalogVoltage(CH_ANALOG2, 0);

  // Logic outputs off
  setPercent(CH_TRIAC1, 0);
  setPercent(CH_TRIAC2, 0);
  setPercent(CH_RELAY1, 0);
  setPercent(CH_RELAY2, 0);

  sine1_on = false;
  sine2_on = false;
}

String rpc_servo(String args) {
  // Format: channel,angle
  int comma = args.indexOf(',');
  if (comma < 0) return "ERR servo format";

  int servo = args.substring(0, comma).toInt();
  int angle = args.substring(comma + 1).toInt();

  int ch = CH_SERVO1 + servo;
  if (servo < 0 || servo > 3) return "ERR servo number";

  setServoAngle(ch, angle);
  return "OK servo";
}

String rpc_pwm(String args) {
  // Format: channel,percent
  int comma = args.indexOf(',');
  if (comma < 0) return "ERR pwm format";

  int ch = args.substring(0, comma).toInt();
  int percent = args.substring(comma + 1).toInt();

  if (ch < 0 || ch > 15) return "ERR channel";

  setPercent(ch, percent);
  return "OK pwm";
}

String rpc_motor(String args) {
  // Format: L,percent or R,percent
  int comma = args.indexOf(',');
  if (comma < 0) return "ERR motor format";

  String side = args.substring(0, comma);
  int percent = args.substring(comma + 1).toInt();

  if (side == "L") setPercent(CH_MOTOR_L, percent);
  else if (side == "R") setPercent(CH_MOTOR_R, percent);
  else return "ERR motor side";

  return "OK motor";
}

String rpc_led(String args) {
  // Format: 1,percent or 2,percent
  int comma = args.indexOf(',');
  if (comma < 0) return "ERR led format";

  int led = args.substring(0, comma).toInt();
  int percent = args.substring(comma + 1).toInt();

  if (led == 1) setPercent(CH_LED1, percent);
  else if (led == 2) setPercent(CH_LED2, percent);
  else return "ERR led";

  return "OK led";
}

String rpc_analog(String args) {
  // Format: 1,volts or 2,volts
  int comma = args.indexOf(',');
  if (comma < 0) return "ERR analog format";

  int out = args.substring(0, comma).toInt();
  float volts = args.substring(comma + 1).toFloat();

  if (out == 1) setAnalogVoltage(CH_ANALOG1, volts);
  else if (out == 2) setAnalogVoltage(CH_ANALOG2, volts);
  else return "ERR analog";

  return "OK analog";
}

String rpc_sine(String args) {
  // Format: 1,on,0.5 or 2,on,1.0 or 1,off,0
  int c1 = args.indexOf(',');
  int c2 = args.indexOf(',', c1 + 1);

  if (c1 < 0 || c2 < 0) return "ERR sine format";

  int out = args.substring(0, c1).toInt();
  String state = args.substring(c1 + 1, c2);
  float freq = args.substring(c2 + 1).toFloat();

  if (out == 1) {
    sine1_freq = freq;
    sine1_on = (state == "on");
  } else if (out == 2) {
    sine2_freq = freq;
    sine2_on = (state == "on");
  } else {
    return "ERR sine output";
  }

  sine_start_ms = millis();
  return "OK sine";
}

String rpc_relay(String args) {
  // Format: 1,on or 2,off
  int comma = args.indexOf(',');
  if (comma < 0) return "ERR relay format";

  int relay = args.substring(0, comma).toInt();
  String state = args.substring(comma + 1);

  int percent = (state == "on") ? 100 : 0;

  if (relay == 1) setPercent(CH_RELAY1, percent);
  else if (relay == 2) setPercent(CH_RELAY2, percent);
  else return "ERR relay";

  return "OK relay";
}

String rpc_triac(String args) {
  // Format: 1,on or 2,off
  // NOTE: This is only logic output demo. Real AC dimming needs zero-cross timing.
  int comma = args.indexOf(',');
  if (comma < 0) return "ERR triac format";

  int triac = args.substring(0, comma).toInt();
  String state = args.substring(comma + 1);

  int percent = (state == "on") ? 100 : 0;

  if (triac == 1) setPercent(CH_TRIAC1, percent);
  else if (triac == 2) setPercent(CH_TRIAC2, percent);
  else return "ERR triac";

  return "OK triac";
}

void demoWave() {
  for (int angle = 40; angle <= 140; angle += 10) {
    setServoAngle(CH_SERVO1, angle);
    setServoAngle(CH_SERVO2, 180 - angle);
    setServoAngle(CH_SERVO3, angle);
    setServoAngle(CH_SERVO4, 180 - angle);
    delay(80);
  }

  for (int angle = 140; angle >= 40; angle -= 10) {
    setServoAngle(CH_SERVO1, angle);
    setServoAngle(CH_SERVO2, 180 - angle);
    setServoAngle(CH_SERVO3, angle);
    setServoAngle(CH_SERVO4, 180 - angle);
    delay(80);
  }
}

void demoFade() {
  for (int p = 0; p <= 100; p += 2) {
    setPercent(CH_LED1, p);
    setPercent(CH_LED2, 100 - p);
    setAnalogVoltage(CH_ANALOG1, p * 0.05);
    setAnalogVoltage(CH_ANALOG2, (100 - p) * 0.05);
    delay(30);
  }

  for (int p = 100; p >= 0; p -= 2) {
    setPercent(CH_LED1, p);
    setPercent(CH_LED2, 100 - p);
    setAnalogVoltage(CH_ANALOG1, p * 0.05);
    setAnalogVoltage(CH_ANALOG2, (100 - p) * 0.05);
    delay(30);
  }
}

void demoBlink() {
  for (int i = 0; i < 5; i++) {
    setPercent(CH_LED1, 100);
    setPercent(CH_LED2, 100);
    setPercent(CH_RELAY1, 100);
    setPercent(CH_RELAY2, 100);
    delay(300);

    setPercent(CH_LED1, 0);
    setPercent(CH_LED2, 0);
    setPercent(CH_RELAY1, 0);
    setPercent(CH_RELAY2, 0);
    delay(300);
  }
}

void demoDance() {
  for (int i = 0; i < 4; i++) {
    setServoAngle(CH_SERVO1, 40);
    setServoAngle(CH_SERVO2, 140);
    setServoAngle(CH_SERVO3, 40);
    setServoAngle(CH_SERVO4, 140);
    setPercent(CH_LED1, 100);
    setPercent(CH_LED2, 0);
    setPercent(CH_MOTOR_L, 60);
    setPercent(CH_MOTOR_R, 20);
    delay(400);

    setServoAngle(CH_SERVO1, 140);
    setServoAngle(CH_SERVO2, 40);
    setServoAngle(CH_SERVO3, 140);
    setServoAngle(CH_SERVO4, 40);
    setPercent(CH_LED1, 0);
    setPercent(CH_LED2, 100);
    setPercent(CH_MOTOR_L, 20);
    setPercent(CH_MOTOR_R, 60);
    delay(400);
  }

  setPercent(CH_MOTOR_L, 0);
  setPercent(CH_MOTOR_R, 0);
}

String rpc_demo(String args) {
  if (args == "wave") demoWave();
  else if (args == "fade") demoFade();
  else if (args == "blink") demoBlink();
  else if (args == "dance") demoDance();
  else if (args == "alloff") allOff();
  else return "ERR demo";

  return "OK demo";
}

void updateSinewaves() {
  float t = (millis() - sine_start_ms) / 1000.0;

  if (sine1_on) {
    float v = 2.5 + 2.5 * sin(2.0 * PI * sine1_freq * t);
    setAnalogVoltage(CH_ANALOG1, v);
  }

  if (sine2_on) {
    float v = 2.5 + 2.5 * sin(2.0 * PI * sine2_freq * t);
    setAnalogVoltage(CH_ANALOG2, v);
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Wire.begin();

  pca.begin();
  pca.setPWMFreq(SERVO_FREQ_HZ);
  delay(10);

  allOff();

  Bridge.begin();

  Bridge.provide("servo", rpc_servo);
  Bridge.provide("pwm", rpc_pwm);
  Bridge.provide("motor", rpc_motor);
  Bridge.provide("led", rpc_led);
  Bridge.provide("analog", rpc_analog);
  Bridge.provide("sine", rpc_sine);
  Bridge.provide("relay", rpc_relay);
  Bridge.provide("triac", rpc_triac);
  Bridge.provide("demo", rpc_demo);

  //Serial.println("UNO Q PCA9685 Demo MCU ready");
}

void loop() {
  Bridge.update();
  updateSinewaves();
}