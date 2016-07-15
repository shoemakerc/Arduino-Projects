/*
 * 
 *  SOFTWARE:--------------------------------------------------------------------------------------------------------------------
  
      This program is the main code used in the micro-controller. 
      It could communicate with the host in exchange for information and receive inputs from the control cabinets.

      Names of functions that take inputs from control cabinets and host and outputs to display and RYG light are 
      capitalized in order to distinguish them from other helper functions. Input functions are put in the INPUT FUNCTIONS block
      and output functions are put in the OUTPUT FUNCTIONS part. All other user declared functions go to the OTHER HELPER FUNCTIONS
      block. 

      The following table is the list of all states and meanings.  If a pin number is given, 
      then the state is read in hardware on the micro-controller.
      +-------------------+-------+-----+-------+-------------+-------+-------------+
      | __Input__________ | Abbr. | pin | state | __meaning__ | state | __meaning__ |
      +-------------------+-------+-----+-------+-------------+-------+-------------+-----------------------------------------
      | Turn Counter      |  TCR  |   9 |  00   |    turn 0   |   01  |   turn  1   |  10  |   turn  2   |  11  |   turn  3   |  
      +-------------------+-------+-----+-------+-------------+-------+-------------+-----------------------------------------
      | Manual Override   |  MOS  |  22 |  0    |     off     |   1   |     on      |
      +-------------------+-------+-----+-------+-------------+-------+-------------+
      | CtrlCab 24 Volts  |  CVT  |  24 |  0    |     off     |   1   |     on      |
      +-------------------+-------+-----+-------+-------------+-------+-------------+
      | Hardware Ready    |  ESP  |  26 |  0    |   E-STOP    |   1   |  ok to run  | 
      +-------------------+-------+-----+-------+-------------+-------+-------------+
      | Door Open         |  DRO  |  28 |  0    |   closed    |   1   |    open     |
      +-------------------+-------+-----+-------+-------------+-------+-------------+
      | Seat Belt         |  SBT  |  34 |  0    |   buckled   |   1   |  unbuckled  |
      +-------------------+-------+-----+-------+-------------+-------+-------------+
      | Mech Brake        |  MBK  |  36 |  0    | not engaged |   1   |   engaged   |
      +-------------------+-------+-----+-------+-------------+-------+-------------+
      | Commutation On    |  CMT  |  40 |  0    |     off     |   1   |     on      |
      +-------------------+-------+-----+-------+-------------+-------+-------------+
      | Commutation Home  |  CMH  |  42 |  0    | not at home |   1   |   at home   |
      +-------------------+-------+-----+-------+-------------+-------+-------------+
      | Request To Enter  |  R2E  |  48 |  0    |  no reuqest |   1   |   request   |    
      +-------------------+-------+-----+-------+-------------+-------+-------------+
      | Ready To Run      |  R2R  | n/a |  0    |  not ready  |   1   |    ready    |
      +-------------------+-------+-----+-------+-------------+-------+-------------+
      | Servo On          |  SRO  | n/a |  0    |  servo off  |   1   |  servo on   |
      +-------------------+-------+-----+-------+-------------+-------+-------------+
      | In Motion         |  IMN  | n/a |  0    |  not moving |   1   |    moving   |
      +-------------------+-------+-----+-------+-------------+-------+-------------+
    */
    /*  
    HARDWARE:---------------------------------------------------------------------------------------------------------------------
    
      The following table is the list of all controllable power outputs.
      +-----------------------------------------------------------------------------
      | __Power_Output_____ | Abbr. | pin | 
      ==================================================================================
      | Turn Counter Power  |  TCV  |  10 |  Allows TCR to be read
      +-----------------------------------------------------------------------------
      | Commutation Power   |  CMV  |  41 |  Allows CMP to be read
      +-----------------------------------------------------------------------------
      Only CMV (Commutation) is normally gated.  
      The Turn-Counter is turned-on and left on at startup.

      The following table is the list of all controllable ground outputs that pull switch 
      sensor inputs low.  Each of the paired sensors must have a pull-up resistor applied.
      +-----------------------------------------------------------------------------
      | __Ground_Output____ | Abbr. | pin | 
      ==================================================================================
      | Turn Counter Grnd   |  TCG  |  A8 |  0V low end of the turn-counter pot
      +-----------------------------------------------------------------------------
      | Chassis Power Grnd  |  PWG  | A15 |  Sink for Chassis Power LED
      +-----------------------------------------------------------------------------
      | Wall Box Ground     |  WBG  |  23 |  WallBox: Allows MOS,CVT,ESP to be read
      +-----------------------------------------------------------------------------
      | Seat Belt Ground    |  SBG  |  33 |  Allows SBT to be read
      +-----------------------------------------------------------------------------
      | Mech Brake Ground   |  MBG  |  35 |  Allows MBK to be read
      +-----------------------------------------------------------------------------
      | Req2Enter Ground    |  REG  |  49 |  Allows R2E to be read
      +-----------------------------------------------------------------------------
      All of these pins are set LOW at setup.

      The following table is the list of all signal outputs.
      +-----------------------------------------------------------------------------
      | __Signal_Output__ | Abbr. | pin | 
      ==================================================================================
      | Chassis Power RED |  PWR  | A14 |  Indicates Arduino is happy, powers an LED.
      +-----------------------------------------------------------------------------
      | Stoplight RED     |  SLR  |   5 |  Signals a transistor that powers a relay.
      +-----------------------------------------------------------------------------
      | Stoplight YELLOW  |  SLY  |   6 |  Signals a transistor that powers a relay.
      +-----------------------------------------------------------------------------
      | Stoplight GREEN   |  SLG  |   7 |  Signals a transistor that powers a relay.
      +-----------------------------------------------------------------------------
      | Override ENABLE   |  MOR  |  25 |  Enables manual over-ride, powers a relay.
      +-----------------------------------------------------------------------------
      | Override RED      |  MOB  |  27 |  Indicates manual over-ride is completely enabled
      +-----------------------------------------------------------------------------
      | Override GREEN    |  MOA  |  29 |  Indicates manual over-ride switch is ON
      +-----------------------------------------------------------------------------
      | Req2Enter Status  |  RQS  |  47 |  Indicates Request status with BLUE LED
      +-----------------------------------------------------------------------------
      Please note that the Override LED's should be controlled in hardware and this software
      control is unnecessary.  The Override Switch should directly light-up the GREEN LED.  
      When the Override Enable signal comes through, then GREEN should go out and RED illuminate.
      The Override ENABLE should open the gate on a transistor which closes the relay.
 */
 
#include <SPI.h>
#include <Ethernet.h>  // to talk to host
#include <Wire.h>      // to talk to Matrix Display Arduino
//#include "..\ORVuCtrl\CtrlState.h"
#include "CtrlState.h"

//<--------------IP Address and Port Number--------------->
#define IPSUBNET  0    // typically 0 or 1 for 192.168.x.yyy networks
#define IPHSTNUM 50    // final octet
#define portNumber 333
//<-----------------------PINS---------------------------->  
#define TCGpin  A8    // Turn-Counter Ground (Analog)
#define TCRpin  A9    // Turn-Counter Sensor (Analog)
#define TCVpin  A10   // Turn-Counter Power (Analog)
#define PWRpin  A14   // Power Indicator (Analog)
#define PWGpin  A15   // Chassis Power Indicator Ground (Analog)
#define SLRpin   5   // Stoplight RED
#define SLYpin   6   // Stoplight YELLOW
#define SLGpin   7   // Stoplight Green
#define MOSpin  22   // Manual Override Switch On Sense
#define WBGpin  23   // WallBox Ground    <-- This may be removed so the wire can provide 5V to relay -->
#define CVTpin  24   // Control-Cabinet 24 Volts On Sense
#define MORpin  25   // Manual Override Enable Relay Signal
#define ESPpin  26   // E-Stop / Actually "IndraDrive Ready" Sense
#define MOBpin  27   // Manual Override Enabled LED
#define DROpin  28   // Door Open Sense
#define MOApin  29   // Manual Override Ready LED
#define SBTpin  32   // Seatbelt Secure (Black)
#define SBGpin  33   // SeatBelt Ground (White)
#define MBKpin  34   // Mechanical Brake Engaged (Yellow)
#define MBGpin  35   // Mech Brake Ground (Red) 
#define CMTpin  40   // Commutation Sensor On
#define CMVpin  41   // Commutation Power
#define CMHpin  42   // Commutation Sensor Home
#define RQSpin  47   // Request Status LED
#define R2Epin  48   // Enter Requested Sense
#define REGpin  49   // Request-To-Enter Ground  <-- This may be removed as there is already a ground available -->

//<-----------StopLight States----------->
#define SLT_Off         0
#define SLT_Red       100
#define SLT_Yellow    200
#define SLT_Green     400
#define SLT_BlinkRed  (SLT_Red | SLT_Green)
#define SLT_BlinkYlw  (SLT_Yellow | SLT_Red)
#define SLT_BlinkGrn  (SLT_Green | SLT_Yellow)
#define SLT_Rotating  (SLT_Red | SLT_Green | SLT_Yellow)
 
//<---------potentiometer range---------------->
#define TCRgnA    20 
#define TCRgnB   220
#define TCRgnC   640
#define TCRgnD   850
#define TCRgnE  1023
#define TCNRgns    5
int TCRegion[TCNRgns] = {TCRgnA, TCRgnB, TCRgnC, TCRgnD, TCRgnE};
unsigned int maxtcv[TCNRgns] = { 0,0,0,0,0 };
unsigned int mintcv[TCNRgns] = { 1024,1024,1024,1024,1024 };

// Global state structure/union
ORVCtrlUnion orvcu = { 0, 0 };

//<----------------from display---------------->
int LN = 0; // number of lines for display
int NC = 0; // number of characters for display
int SC = 0; // scroll or not

//<----------- from host----------->
int RR =0 ;// 0: not ready, 1: ready [Ready Run]  
int IM =0 ; // 0: not in motion, 1: in motion [In Motion]  
String displayString = String("");
int displayColor = 0 ;
int displayLine = 0 ;

// <------------from the control system ------------> 
int SB = 1; // 0: on 1: off [Seat Belt]
int TV = 1;// 0: off, 1: on [24 volts]
int DO = 0; // 0: open, 1:closed [door open]
int CP = 0; // 0: not at home, 1: at home [Commutation Position]
int RE = 0; // 0: no request, 1: request [Request to Enter]
int ES = 1; // 0: on, 1: off [E-stop]
int PW = 0; // 0: off, 1: on [Power-on]

//<------------from potentiometer------------------->
String TC = String("00"); // 0,01,10,11 [Turn Counter] errorMessgae when not in any range

//<-----------to host-------------->

String ST_HOST = String("ST:00001111"); // [4V,EStop,Door,Com,SB,R2E,TC]

String DS_HOST = String("DS:1>0>1"); // [line number, number of characters, scroll]

//<-----------signatures strings-------------->

const String displayMessage = String("DM");
const String Status = String("ST");
const String readyRun = String("RR");
const String inMotion = String("IM");
const String displayInfo = String("DS");
const String powerOn = String("PW");

// for testing the green, red, yellow light, assume that these info also come from the host
const String voltageOn = String("TV");
const String Estop = String("ES");

//----------------------------------------------------------------------
String errorMessage = String("error");
int maxMesLength = 50;
//<------------------------mac,IP,port number-------------------------->
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
IPAddress ip(192, 168, IPSUBNET, IPHSTNUM);

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(portNumber);
//------------------------------------------------------------------------------------

void setup() {
  Ethernet.begin(mac, ip);  // start the Ethernet connection and the server
  server.begin();  
   
  initializePins(); // set pinModes for pins used on Arduino board
  
  RYG_OUTPUT(2560);  // set the stop light
  Serial.begin(9600);
  Serial.println("Done with setup.");
}


void loop() {
  static unsigned short last_data = 0;
  // Cycle StopLight  cycle_stoplight();
  // Read Inputs
  read_inputs();
  if (orvcu.bit.MOS) {
    Serial.print("TC ");
    Serial.print(orvcu.bit.TCR,DEC);
    Serial.print(": ");
    Serial.print(orvcu.bit.TCV,DEC);
    Serial.print("  Mn(");
    Serial.print(mintcv[orvcu.bit.TCR],DEC);
    Serial.print(") Mx(");
    Serial.print(maxtcv[orvcu.bit.TCR],DEC);
    Serial.println(")");
    
    delay(500);
  }

  if (orvcu.bit.R2E) {
    orvcu.bit.CMT = !orvcu.bit.CMT; 
  }
  if (orvcu.bit.CMT) 
    digitalWrite(CMVpin,HIGH);
  else
    digitalWrite(CMVpin,LOW);
  
  set_lights();
  
  // Output if changed
  if (orvcu.sval != last_data) {
    Serial.print(orvcu.sval,HEX);
    Serial.print(": ");
    Serial.println(orvcu.sval,BIN);
    last_data = orvcu.sval;
    delay(500);
  }
}

void read_inputs()
{
  // R2E, CVT, ESP, DRS, MOR, SBT, and MBK are normally pulled-up
  // When switched close, they are pulled LOW.
  // Read Req2Enter
  orvcu.bit.R2E = (digitalRead(R2Epin) == HIGH) ? 0 : 1;
  // Read CtrlCab 24V and Ready
  orvcu.bit.CVT = (digitalRead(CVTpin) == HIGH) ? 0 : 1;
  orvcu.bit.ESP = (digitalRead(ESPpin) == HIGH) ? 0 : 1;
  // Read Door Secure
  orvcu.bit.DRO = (digitalRead(DROpin) == HIGH) ? 0 : 1;
  // Read Override Switch
  orvcu.bit.MOS = (digitalRead(MOSpin) == HIGH) ? 0 : 1;
  // Read SeatBelt & Brake
  orvcu.bit.SBT = (digitalRead(SBTpin) == HIGH) ? 0 : 1;
  orvcu.bit.MBK = (digitalRead(MBKpin) == HIGH) ? 0 : 1;
  
  // Read Turn-Counter
  TURN_COUNTER_INPUT();
}

void cycle_stoplight()
{
  unsigned long period = 7500;
  unsigned long tm = millis();
  unsigned long state = (tm/period)%3;
  static unsigned long last_state = 0;
  if (last_state != state) {
    orvcu.bit.SLT = 1<<state;
    RYG_OUTPUT(orvcu.bit.SLT*100);
    if (orvcu.bit.SLT&0x04)
      digitalWrite(RQSpin,HIGH);
    else
      digitalWrite(RQSpin,LOW);
    Serial.print("Time: ");
    Serial.print(tm,DEC);
    Serial.print("  ");
    Serial.print("State: ");
    Serial.print(state,DEC);
    Serial.print("  ");
    Serial.print("RYG: ");
    Serial.println(orvcu.bit.SLT,DEC);
//    delay(500);
    last_state = state;
  }
}

void set_lights()
{
  // 4-bits = Red,Yel,Grn,Blu
  orvcu.bit.SLT = (orvcu.bit.CVT<<0) | (orvcu.bit.ESP<<1) | (orvcu.bit.CMH<<2);
  unsigned char curr_state = orvcu.bit.SLT | (orvcu.bit.R2E<<3);
  static unsigned char last_state = 0;
  if (curr_state != last_state) {
    RYG_OUTPUT(orvcu.bit.SLT*100);
    if (curr_state&0x08)
      digitalWrite(RQSpin,HIGH);
    else
      digitalWrite(RQSpin,LOW);
    last_state = curr_state;
      /*
    Serial.print("Time: ");
    Serial.print(tm,DEC);
    Serial.print("  ");
    Serial.print("State: ");
    Serial.print(state,DEC);
    Serial.print("  ");
    Serial.print("RYG: ");
    Serial.println(orvcu.bit.SLT,DEC);
    */
  }
}

//<-----------------------------------------User Declared Functions------------------------------------------->


//<-------------------------HOST COMMUNICATION-------------------------->

String HOST_INPUT(String message){
  
  int index = message.indexOf(':');
  String instruct;

  int amp = message.indexOf("@");
  while (amp != -1 && amp == 0){ // there exits extra @s at the beginning of the message, //----@@...@PW:0$ case-----
      message.remove(0,0);
      amp = message.indexOf("@");
  }

  String info;
  if(index != -1){ // message from host 
     instruct = message.substring(0,index);
     info = message.substring(index+1);   
   }
   else{ // if there is no semicolumn within the message
    if (message.length() == 2){ // query from host 
      instruct = message;
    }
    else{ // error case
      return errorMessage;
    }
   }
  if (instruct.equals(readyRun)){                                  // RR
    if(info.equals(String("1")) || info.equals(String("0"))){
      RR = info.toInt();
      return ST_HOST;
    }
    else{
      return readyRun + String(":") + errorMessage;
    }
  }
  else if (instruct.equals(inMotion)){                             // IM 
    if(info.equals(String("1")) || info.equals(String("0"))){
      IM = info.toInt();
      return ST_HOST;
    }
    else{
      return inMotion + String(":") +  errorMessage;
    }
 }
  else if (instruct.equals(displayMessage)){                      //displayMessage on LCD 
    int lineNumber = info.indexOf('>');
    displayString = info.substring(0,lineNumber);
    String rest = info.substring(lineNumber+1);
    int lineNumber2 = rest.indexOf('>');
    
    boolean lineNumberIsNum= checkNumeric(rest.substring(0,lineNumber2));
    boolean displayColorIsNum = checkNumeric(rest.substring(lineNumber2 + 1));
    
    displayLine = rest.substring(0,lineNumber2).toInt(); 
    displayColor = rest.substring(lineNumber2 + 1).toInt();
    
    if( 0 <= lineNumber && lineNumber < NC && displayColor < 16 && lineNumberIsNum && displayColorIsNum){
      return ST_HOST;
      }
    else{
      return displayMessage + String(":") + errorMessage;
      }
  }
  else if (instruct.equals(Status)){                              //Status: 24V, EStop, Door, Com, SB, R2E, TC
    return ST_HOST; 
  }
  else if (instruct.equals(displayInfo)){                         //displayInfo: NumLines>NumChars>Scroll
    return DS_HOST;
  }
  else if (instruct.equals(powerOn)){                            // PW
    if(info.equals(String("0")) || info.equals(String("1")) ){
      PW = info.toInt();
      return ST_HOST;
    }
    else{
      return powerOn + String(":") + errorMessage;
    }
  }
  else{
    return errorMessage;
  }
}

//<-----------------------END HOST COMMUNICATION-------------------------->


//<-------------------------INPUT FUNCTIONS--------------------------------->

//<--------Digital Pins------->
void DIGITAL_PIN_INPUT(){

  // digital reads
   SB = anToDig(analogRead(A2));
   TV =  anToDig(analogRead(A3));
   DO = anToDig(analogRead(A4));
   ES = anToDig(analogRead(A5));  
  
  getRequestToEnter();
  
  if (ES == 0 || TV == 0){
     RR = 0; // when E-stop is on, set not ready to run, power off, and 24 volts off
     PW = 0;
     IM = 0;
     TV = 0;
    }
  
  return;
 }

//<--------Turn Counter------->
void TURN_COUNTER_INPUT()
{
  int r = 0;
  int sensorValue = analogRead(TCRpin);
  for (r =0; r<TCNRgns; r++){
    int topofrgn = TCRegion[r];
    int btmofrgn = (r > 0) ? TCRegion[r-1]+1 : 0;
    if ( (btmofrgn <= sensorValue) && (sensorValue <= topofrgn) ) {
      orvcu.bit.TCR = r;
      if (sensorValue > maxtcv[r])
        maxtcv[r] = sensorValue;
      else if (sensorValue < mintcv[r])
        mintcv[r] = sensorValue;
    }
  }
  orvcu.bit.TCV = sensorValue;
  return;
}
 
//<---Helper for Turn Counter--->
void checkRange(int rangeValue){
  switch(rangeValue){
    case 0:{              //3
      TC = String("11");  
      break;
    }
    case 2:{              //2
      TC = String("10");
      break;
    }
    case 4:{              //1
      TC = String("01");
      break;
    }
    case 6:{              //0
      TC = String("00");
      break;
    }
  }
}
//<----Commutation Postion----->

void COMMUTATION_POSITION_INPUT(){
  CP = anToDig(analogRead(A1));
  return;
 }

//<------------------------END INPUT FUNCTIONS----------------------------->



//<--------------------------OUTPUT FUNCTIONS------------------------------>

//<-------RGY LIGHT OUTPUT------>
int  RYG_SET() {
  int stoplightstate = SLT_Off;
  if (TV == 1 && RR == 1 && IM == 0 && ES == 1 && PW == 0) {  // when 24 volts on, ready to run, not moving,  e-stop off, power off
    stoplightstate = SLT_Yellow;
  }
  else{
    if (IM == 1 && TV == 1 && ES == 1){ // machine moving  when 24 volts on and e-stop off 
      stoplightstate = SLT_BlinkRed;
    }
    else if(PW == 1 && TV == 1 && ES == 1){ // power on when ready 
      stoplightstate = SLT_Red;
    }
    else if(TV == 0 || ES == 0){ // 24 volts off or E-stop on 
      stoplightstate = SLT_Green;
    }
    else if(TV == 1 && RR == 0 && PW == 0 && IM == 0){ // 24 volts on but not ready to run and power is off
      stoplightstate = SLT_Green;
    }
 }
 return stoplightstate;
}

void RYG_OUTPUT(unsigned int stoplightstate) {
  // Normal stoplightstate should be 100 - 800
  // Subtract 100 and evaluate three bits
  if (stoplightstate > 2000)
    stoplightstate= RYG_SET();
  unsigned char bits = (stoplightstate/100)&0x7;
  // Turn on first
  if ((bits&0x1)!=0)
    digitalWrite(SLRpin,HIGH);
  if ((bits&0x2)!=0)
    digitalWrite(SLYpin,HIGH);
  if ((bits&0x4)!=0)
    digitalWrite(SLGpin,HIGH);
  // Turn off last
  if ((bits&0x1)==0)
    digitalWrite(SLRpin,LOW);
  if ((bits&0x2)==0)
    digitalWrite(SLYpin,LOW);
  if ((bits&0x4)==0)
    digitalWrite(SLGpin,LOW);
}
  
//<-------DISPLAY OUTPUT------->
void DISPLAY_OUTPUT(){
  /*
   lcd.clear();
   lcd.setCursor(0,0);
   lcd.print("host");
   lcd.print(ST_HOST);
   lcd.setCursor(0,1);
   String separator = String(":");
   String other = String(SB) + separator + String(RE) + separator + String(DO) + separator +  String(RR) + separator + String(IM) + separator + String(PW);
   lcd.print("other");
   lcd.print(other);
   delay(300);
   */
   return;   
}
//<-----------------------END OUTPUT FUNCTIONS----------------------------->



//<----------------------OTHER HELPER FUNCTIONS---------------------------->
//<--------------Initialize Pins------------------->

void initializePins() {
  
  // chassis power indicator
    pinMode(PWGpin,OUTPUT);
    pinMode(PWRpin,OUTPUT);
    digitalWrite(PWGpin,LOW);
    digitalWrite(PWRpin,HIGH);
    
  // turn-counter
    pinMode(TCGpin,OUTPUT);
    pinMode(TCRpin,INPUT);
    pinMode(TCVpin,OUTPUT);
    digitalWrite(TCGpin,LOW);  // Ground
    digitalWrite(TCVpin,HIGH); // Reference Voltage
    
  // output pins for StopLight 
    pinMode(SLRpin,OUTPUT);
    pinMode(SLGpin,OUTPUT);
    pinMode(SLYpin,OUTPUT);
    digitalWrite(SLRpin,LOW);  // relay off
    digitalWrite(SLYpin,LOW);  // relay off
    digitalWrite(SLGpin,LOW);  // relay off

  // request to enter
    pinMode(R2Epin,INPUT_PULLUP);
    pinMode(REGpin,OUTPUT);
    pinMode(RQSpin,OUTPUT);
    digitalWrite(REGpin,LOW);  // Ground
    digitalWrite(RQSpin,LOW);  // LED off

  // control-cab, wallbox
    pinMode(CVTpin,INPUT_PULLUP);
    pinMode(ESPpin,INPUT_PULLUP);
    pinMode(MOSpin,INPUT_PULLUP);
    pinMode(DROpin,INPUT_PULLUP);
    pinMode(WBGpin,OUTPUT);
    pinMode(MORpin,OUTPUT);
    pinMode(MOApin,OUTPUT);
    pinMode(MOBpin,OUTPUT);
    digitalWrite(WBGpin,LOW);  // Ground
    digitalWrite(MORpin,LOW);  // Manual Override off
    digitalWrite(MOApin,LOW);  // LED off
    digitalWrite(MOBpin,LOW);  // LED off

  // seatbelt and brake
    pinMode(MBKpin,INPUT_PULLUP);
    pinMode(SBTpin,INPUT_PULLUP);
    pinMode(MBGpin,OUTPUT);
    pinMode(SBGpin,OUTPUT);
    digitalWrite(MBGpin,LOW);  // Ground
    digitalWrite(SBGpin,LOW);  // Ground

  // commutation 
    pinMode(CMTpin,INPUT);
    pinMode(CMHpin,INPUT);
    pinMode(CMVpin,OUTPUT);
    digitalWrite(CMVpin,LOW);  // Power Off
  
  return;
}

boolean checkHeader(String input){
  if(input.substring(0) == String("@@@"))
  { 
    return true;
  }
  else{
    return false;
  }
}

//<----check if the input string is a number---->
boolean checkNumeric(String input){
  boolean isNum = true;
  for(int i =0; i< input.length();i++){
    if (!isDigit(input.charAt(i))){
      isNum = false;
    }
  }
  return isNum;
}
  
//<----update statuses---->
void updateStatus(){
  
  ST_HOST = String("ST:") + String(TV) + String(ES) + String(DO) + String(CP) + String(SB) + String(RE) + TC; 
  return;

}

int anToDig(int analog){
  int digital = 0;
  if (analog > 500) {
    digital = 1;
  }
  return digital;
}

void getRequestToEnter(){
  
  if (RE == 1 && DO == 1){ // keep RE to be on until the door is closed
      RE = 1;
  }
  else{
      RE = digitalRead(R2Epin);
  }// temporary switch

  return;
}
//<---------------------END OTHER HELPER FUNCTIONS------------------------->



