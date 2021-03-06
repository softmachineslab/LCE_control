/*
 * PWMs to control Mason's actuator. Set duty cycle over serial.
 * Copyright Soft Machines Lab 2021.
 * Andrew Sabelhaus and Mason Zadan
 */

// For converting floats to cc register values. If 8-bit PWM...
#define MAX_CC 255
// better here than as an int
#define SERIAL_RATE 115200
// for strtok string conversoin
#define BUFSIZE 200

// pin setup and constants
// will use two PWMs
int fwd_pin = 3;
int rev_pin = 5;
// placeholders for what we'll read from the serial terminal
String received;
float duty_fwd = 0.0;
float duty_rev = 0.0;
int raw_duty = 0;
float highlow_inv_duty = 0.0;
// because compiler is annoying and sscanf can't do floats directly
char *token;
char received_raw[BUFSIZE];
// for the BJT circuit
bool switch_highlow = true;

void setup() {
  Serial.begin(SERIAL_RATE);
  Serial.setTimeout(10); // makes read faster
  // Wait until the port is ready
  while (!Serial)
    ;
  Serial.println("LCE PWM control. Type %f %f to set two percent values (0.0-1.0) for the PWMs.");
  // ensure we start with all transistors off
  pinMode(fwd_pin, OUTPUT);
  pinMode(rev_pin, OUTPUT);
  analogWrite(fwd_pin, convert_duty(0.0));
  analogWrite(rev_pin, convert_duty(0.0));
  // we'll also turn on the LED when any pin is set high
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  // poll the serial port (wait until user sends something)
  if( Serial.available() > 0){
    // presummably the user is smart enough to send the two floats...
    received = Serial.readString();
    // using strtok to parse manually
    received.toCharArray(received_raw, BUFSIZE);
    token = strtok(received_raw, " ");
    if(token == NULL){
      Serial.println("Error! nothing sent!");
    }
    else{
      duty_fwd = atof(token);
      token = strtok(NULL, " ");
    }
    if(token == NULL){
      Serial.println("Error! Got one duty cycle but not the second, OR you didn't send in a valid format.");
    }
    else{
      duty_rev = atof(token);
    }
    Serial.println("Duty cycles read as:");
    Serial.println(duty_fwd);
    Serial.println(duty_rev);
//    Serial.println("Duty cycles converted into CC values (out of 255):");
//    Serial.println(convert_duty(duty_fwd));
//    Serial.println(convert_duty(duty_rev));
//    Serial.println(convert_duty(duty_fwd)); // debugging the convert function
    // actually set the PWM duty cycles
    analogWrite(fwd_pin, convert_duty(duty_fwd));
    analogWrite(rev_pin, convert_duty(duty_rev));
  }
}

int convert_duty(float duty)
{
  // If we're using the inverting BJT circuit, duty cycle is "opposite."
  highlow_inv_duty = duty;
  if(switch_highlow){
    highlow_inv_duty = 1.0 - highlow_inv_duty;
  }
  raw_duty = floor(highlow_inv_duty * MAX_CC);
  // constrain between 0 and MAX_CC.
  raw_duty = (raw_duty < 0) ? 0 : raw_duty;
  raw_duty = (raw_duty > MAX_CC) ? MAX_CC : raw_duty;
  return raw_duty;
}
