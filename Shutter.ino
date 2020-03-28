//
// Shutter
//
// Adafruit feather M0
//
// This is a simple program used to control the shutter. Manual buttons and a digital input are used
// to signal open and close. The shutter coil is pulsed for 100mS, then a 50mS pause, and a second
// 100mS pulse. The shutter we tested was very "sticky" we lossened the screws to free up the shutter.
// When power is first applied both LEDs are out and the user must first issue an open pluse.
//
// Chris Anderson
// Version 1.0, March 21, 2020
//  - Initial release
//
//Indicator lights
const byte open_led = 6;  //open LED indicator                       
const byte close_led = 5; //close LED indicator  
const byte open_switch = 13; //local pushbutton for open
const byte close_switch = 12; //local pushbutton for close
const byte external_input = 24; // external triger input
const byte remote = 11; //pin to read remote or local
const byte open_shutter_ctrl = 21; //to open shutter output
const byte close_shutter_ctrl = 20; //to close shutter output
volatile byte open_state = HIGH; //open LED state
volatile byte close_state = HIGH; //close LED state
volatile bool open_shutter = false; //flag to open shutter
volatile bool close_shutter = false; //flag to close shutter
volatile bool Init = false;          //flag to set to false on power up to forcee first op as open


void setup() 
{
  Serial.begin(9600);
  pinMode(remote, INPUT);
  pinMode(open_switch, INPUT);
  pinMode(open_led, OUTPUT);
  pinMode(close_led, OUTPUT);
  pinMode(open_shutter_ctrl, OUTPUT);
  pinMode(close_shutter_ctrl, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(open_switch), open_shutter_ISR, FALLING);
  pinMode(close_switch, INPUT);
  attachInterrupt(digitalPinToInterrupt(close_switch), close_shutter_ISR, FALLING);
  pinMode(external_input, INPUT);
  attachInterrupt(digitalPinToInterrupt(external_input), external_triger_ISR, CHANGE);
}

void loop() 
{
  digitalWrite(open_led, open_state);
  digitalWrite(close_led, close_state);
  if ( open_shutter == true )
  {
    digitalWrite(open_shutter_ctrl, HIGH);
    delay(100);
    digitalWrite(open_shutter_ctrl, LOW);
    delay(50);
    digitalWrite(open_shutter_ctrl, HIGH);
    delay(100);
    digitalWrite(open_shutter_ctrl, LOW);
    open_shutter = false;
    digitalWrite(open_led, open_state);
    digitalWrite(close_led, close_state);
    delay(1000);
  }

  if ( close_shutter == true )
  {
    digitalWrite(close_shutter_ctrl, HIGH);
    delay(100);
    digitalWrite(close_shutter_ctrl, LOW);
    delay(50);
    digitalWrite(close_shutter_ctrl, HIGH);
    delay(100);
    digitalWrite(close_shutter_ctrl, LOW);
    close_shutter = false;
    digitalWrite(open_led, open_state);
    digitalWrite(close_led, close_state);
    delay(1000);
  }
  
}

void open_shutter_ISR()
{
  if ( open_state == HIGH && open_shutter == false )
  {
    volatile byte remote_state;
    remote_state = digitalRead(remote);
    if ( remote_state == LOW )
    {
      int i,j=0;
      for(i=0;i<5;i++)
      {
        if(j++ > 10000) return;
        if(digitalRead(open_switch) != LOW) i=0;
        delayMicroseconds(500);
      }
      Init = true;
      open_state = LOW;
      close_state = HIGH;
      open_shutter = true;
      close_shutter = false;      
    }
  }
}



void close_shutter_ISR()
{
  if(!Init) return;
  if ( close_state == HIGH && close_shutter == false )
  {
    volatile byte remote_state;
    remote_state = digitalRead(remote);
    if ( remote_state == LOW )
    {
      int i,j=0;    
      for(i=0;i<5;i++)
      {
        if(j++ > 10000) return;
        if(digitalRead(close_switch) != LOW) i=0;
        delayMicroseconds(500);
      }
      close_state = LOW;
      open_state = HIGH;
      open_shutter = false;
      close_shutter = true;  
     }
  }
}

void external_triger_ISR()
{
  byte external_state;

  if(digitalRead(remote) == LOW) return;
  external_state = digitalRead(external_input);
  if ( external_state == LOW)
  {
    close_state = LOW;
    open_state = HIGH;
    open_shutter = false;
    close_shutter = true;   
    Init = true; 
  }
  else if ( external_state == HIGH)
  {
    close_state = HIGH;
    open_state = LOW;
    open_shutter = true;
    close_shutter = false;  
  }
}
