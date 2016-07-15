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
      | Input             | Abbr. | pin | state |   meaning   | state |   meaning   |
      +-------------------+-------+-----+-------+-------------+-------+-------------+-----------------------------------------
      | Turn Counter      |  TCR  |   9 |  00   |    turn 0   |   01  |   turn  1   |  10  |   turn  2   |  11  |   turn  3   |  
      +-------------------+-------+-----+-------+-------------+-------+-------------+-----------------------------------------
      | Manual Override   |  MOS  |  22 |  0    |     off     |   1   |     on      |
      +-------------------+-------+-----+-------+-------------+-------+-------------+
      | CtrlCab 24 Volts  |  CVT  |  24 |  0    |     off     |   1   |     on      |
      +-------------------+-------+-----+-------+-------------+-------+-------------+
      | Hardware Ready    |  ESP  |  26 |  0    |   E-STOP    |   1   |  ok to run  | 
      +-------------------+-------+-----+-------+-------------+-------+-------------+
      | Door Secure       |  DRS  |  28 |  0    |    open     |   1   |   closed    |
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
      | Power Output        | Abbr. | pin | 
      ==================================================================================
      | Turn Counter Power  |  TCV  |  10 |  Allows TCR to be read
      +-----------------------------------------------------------------------------
      | WallBox Power       |  WBV  |  23 |  WallBox: Allows MOS,CVT,ESP to be read
      +-----------------------------------------------------------------------------
      | Seat Belt Power     |  SBV  |  33 |  Allows SBT to be read
      +-----------------------------------------------------------------------------
      | Mech Brake Power    |  MBV  |  35 |  Allows MBK to be read
      +-----------------------------------------------------------------------------
      | Commutation Power   |  CMV  |  41 |  Allows CMP to be read
      +-----------------------------------------------------------------------------
      | Req2Enter Power     |  REV  |  49 |  Allows R2E to be read
      +-----------------------------------------------------------------------------
      Only CMV (Commutation) is normally gated.  All others are turned-on and left on at startup.

      The following table is the list of all signal outputs.
      +-----------------------------------------------------------------------------
      | Signal Output     | Abbr. | pin | 
      ==================================================================================
      | Chassis Power RED |  PWR  | A14 |  Indicates Arduino is happy. (Analog)
      +-----------------------------------------------------------------------------
      | Stoplight RED     |  SLR  |   5 |  
      +-----------------------------------------------------------------------------
      | Stoplight YELLOW  |  SLY  |   6 |  
      +-----------------------------------------------------------------------------
      | Stoplight GREEN   |  SLG  |   7 |  
      +-----------------------------------------------------------------------------
      | Override ENABLE   |  MOR  |  25 |  Enables manual over-ride 
      +-----------------------------------------------------------------------------
      | Override RED      |  MOB  |  27 |  Indicates manual over-ride is completely enabled
      +-----------------------------------------------------------------------------
      | Override GREEN    |  MOA  |  29 |  Indicates manual over-ride switch is ON
      +-----------------------------------------------------------------------------
      | Request Status    |  RQS  |  47 |  Request-To-Enter LED light 
      +-----------------------------------------------------------------------------
      Please note that the Override LED's should be controlled in hardware and this software
      control is unnecessary.  The Override Switch should directly light-up the GREEN LED.  
      When the Override Enable signal comes through, then GREEN should go out and RED illuminate.

      The following table is the list of all 0V pins.
      +----------------------------------+
      | Ground Input       | Abbr. | pin | 
      ====================================
      | Chassis Power Grnd |  PWG  |  15 |  
      +----------------------------------+
      | Turn Counter Grnd  |  TCG  |   8 |  
      +----------------------------------+
      These are set to LOW at startup.
 */
#include <SPI.h>
#include <Ethernet.h>  // to talk to host
#include <Wire.h>      // to talk to Matrix Display Arduino

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
#define MOSpin  22   // Manual Override Switch On
#define WBVpin  23   // WallBox Power
#define CVTpin  24   // Control-Cabinet 24 Volts On
#define MORpin  25   // Manual Override Enable
#define ESPpin  26   // E-Stop / Actually "IndraDrive Ready"
#define MOBpin  27   // Manual Override Enabled
#define DRSpin  28   // Door Secure
#define MOApin  29   // Manual Override Ready
#define SBVpin  33   // SeatBelt Power
#define SBTpin  34   // Seatbelt Secure
#define MBVpin  35   // Mech Brake Power
#define MBKpin  36   // Mechanical Brake Engaged
#define CMTpin  40   // Commutation Sensor On
#define CMVpin  41   // Commutation Power
#define CMHpin  42   // Commutation Sensor Home
#define R2Epin  48   // Enter Requested
#define REVpin  49   // Request-To-Enter Power

//<-----------StopLight States----------->
#define SLT_Off         0
#define SLT_Red       100
#define SLT_Yellow    200
#define SLT_Green     400
#define SLT_BlinkRed  500

//<---------potentiometer range---------------->
#define A_low 0
#define A_high 100 
#define B_low 100
#define B_high 300
#define C_low 300
#define C_high 600
#define D_low 600
#define D_high 1023
#define rangeLength 8
int range[rangeLength] = {A_low,A_high,B_low,B_high,C_low,C_high,D_low,D_high};


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
  
  RYG_OUTPUT();  // set the stop light
}


void loop() {
  
    DIGITAL_PIN_INPUT(); // get input from control system, door, and light sensor
    TURN_COUNTER_INPUT(); //  get turn number from turn counter 
    COMMUTATION_POSITION_INPUT(); // get commutation position
  
    updateStatus(); // update the status string 
    RYG_OUTPUT();  // update stop light
    DISPLAY_OUTPUT(); // update display 

  EthernetClient client = server.available();  // listen for incoming clients
  
  if (client) {
   
    client.flush();
    
    boolean header = false;
    String head;
    String message;
    int countChar = 0;
    
    while (client.connected()) {
      
      DIGITAL_PIN_INPUT(); // get input from control system, door, and light sensor
      TURN_COUNTER_INPUT(); //  get turn number from turn counter 
      COMMUTATION_POSITION_INPUT(); // get commutation position
    
      updateStatus(); // update the status string 

      getRequestToEnter();
      
      RYG_OUTPUT();  // update stop light
      
      DISPLAY_OUTPUT(); // update display 

      getRequestToEnter();
             
      if (client.available()) {
       
//---------------------------------------------------------------- // Communication with the host
       
        char c = client.read();
 
         if(!header){ // keep looking for header
          head += String(c);
          if(head.length()>2){
            header = checkHeader(head);   // update the header information 
            if(!header){ 
              head = head.substring(1);
            }
          }
        }
        else{ // find a header
          if(!String(c).equals(String("$")) && countChar < maxMesLength){ // not a terminator
            countChar = countChar + 1;
            message += String(c);
          }
          else{ 
             getRequestToEnter();
             
             String returnString = HOST_INPUT(message); // read in host message and update variables
             
             getRequestToEnter();
             
             String toClient = String("@@@") + returnString + String("$"); // format the string returned to client 
             int len = toClient.length() + 1;
             char st[len];
             toClient.toCharArray(st,len);
             client.write(st);  // write back to the client
             
             header = false; // end of the message, initialize new header,head, and message
             head = String("");
             message = String("");
          }
        }   
      }
    }
   
    delay(10);  // give client time to receive the data
    client.stop();  // close the connection:
    
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
void TURN_COUNTER_INPUT(){

  int hasRange = false;
  int sensorValue = analogRead(A0);
  for (int i =0; i <rangeLength; i+=2){
    if(range[i] < sensorValue && sensorValue < range[i+1]){
      checkRange(i);   
      hasRange = true;
    }
  }
  if(!hasRange){
    TC = errorMessage;
    }
   
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

void RYG_OUTPUT(){
  int stoplightstate = RYG_SET();
  switch(stoplightstate){
    case SLT_Red:
      digitalWrite(SLRpin,HIGH);
      digitalWrite(SLYpin,LOW);
      digitalWrite(SLGpin,LOW);
      break;
    case SLT_Green:
      digitalWrite(SLRpin,LOW);
      digitalWrite(SLYpin,LOW);
      digitalWrite(SLGpin,HIGH);
      break;
    case SLT_BlinkRed:
      digitalWrite(SLRpin,HIGH);
      digitalWrite(SLYpin,LOW);
      digitalWrite(SLGpin,HIGH);
      break;
    case SLT_Yellow:
      digitalWrite(SLRpin,LOW);
      digitalWrite(SLYpin,HIGH);
      digitalWrite(SLGpin,LOW);
      break;
    default:
      digitalWrite(SLRpin,LOW);
      digitalWrite(SLYpin,LOW);
      digitalWrite(SLGpin,LOW);
    }  
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
    digitalWrite(TCGpin,LOW);
    digitalWrite(TCVpin,HIGH);
    
  // output pins for StopLight 
    pinMode(SLRpin,OUTPUT);
    pinMode(SLGpin,OUTPUT);
    pinMode(SLYpin,OUTPUT);

  // request to enter
    pinMode(R2Epin,INPUT);
    pinMode(REVpin,OUTPUT);
    digitalWrite(REVpin,HIGH);

  // control-cab
  
  // input analog pins 
  
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



