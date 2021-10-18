/*
 * Manual control of Mason's LCE actuator.
 * Copyright Soft Machines Lab 2021.
 * Andrew Sabelhaus and Mason Zadan
 */

#define SERIAL_RATE 115200

// pin setup and constants
// switch between fwd, rev, off
int status = 0;
int fwd_pin = 3;
int rev_pin = 5;
// for the BJT circuit
bool switch_highlow = true;

void setup() {
  // Serial is the programming port on the Zero
//  int serial_rate = 115200;
  Serial.begin(SERIAL_RATE);
  // Wait until the port is ready
  while (!Serial)
    ;
  Serial.println("LCE Control - manual. Specify 0 = off, 1 = forward, 2 = reverse");
  Serial.println("NOTE! You MUST set the serial terminal to 'No Line Ending', otherwise will reset to 0 immediately.");
  // ensure we start with all transistors off
  pinMode(fwd_pin, OUTPUT);
  pinMode(rev_pin, OUTPUT);
  if(switch_highlow){
    digitalWrite(fwd_pin, HIGH);
    digitalWrite(rev_pin, HIGH);
  }
  else{
    digitalWrite(fwd_pin, LOW);
    digitalWrite(rev_pin, LOW);
  }

  // we'll also turn on the LED when any pin is set high
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  // poll the serial port (wait until user sends something)
  if( Serial.available() > 0){
    // presummably the user is smart enough to have only sent one integer...
    int cmd = Serial.read();
    // remove the ASCII character offset for numerical digits
    status = cmd - '0';
    switch(status){
      case 0:
        Serial.println("Status = 0, Off");
        if(switch_highlow){
          digitalWrite(fwd_pin, HIGH);
          digitalWrite(rev_pin, HIGH); 
        }
        else{
          digitalWrite(fwd_pin, LOW);
          digitalWrite(rev_pin, LOW); 
        }
        digitalWrite(LED_BUILTIN, LOW);
        break;
      case 1:
        Serial.println("Status = 1, Forward");
        if(switch_highlow){
          digitalWrite(fwd_pin, LOW);
          digitalWrite(rev_pin, HIGH);
        }
        else{
          digitalWrite(fwd_pin, HIGH);
          digitalWrite(rev_pin, LOW);
        }
        digitalWrite(LED_BUILTIN, HIGH);
        break;
      case 2:
        Serial.println("Status = 2, Reverse");
        if(switch_highlow){
          digitalWrite(fwd_pin, HIGH);
          digitalWrite(rev_pin, LOW);
        }
        else{
          digitalWrite(fwd_pin, LOW);
          digitalWrite(rev_pin, HIGH);
        }
        digitalWrite(LED_BUILTIN, HIGH);
        break;
      default:
        Serial.println("Error! You sent, in ASCII:");
        Serial.println(cmd);
        Serial.println("...which is not a valid command. Not changing any pins.");
    }
  }
}
