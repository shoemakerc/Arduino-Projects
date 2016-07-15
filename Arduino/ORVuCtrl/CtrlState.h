#ifndef ORV_CTRL_STATE_H
#define ORV_CTRL_STATE_H

// Goal: 4-bytes, 32-bits
//    6-bits
//    2-bits reserved
//    6-bits R2E and SLT
//   10-bits TCV
//    3-bits TCR
//    4-bits HOST
struct ORVCtrlState
{
  // single-bit fields
  unsigned char CVT:1;  // Control-Cabinet 24V Power, 1=on
  unsigned char ESP:1;  // Hardware ready | E-Stop, 1=ready to run
  unsigned char DRS:1;  // Door secure, 1=closed
  unsigned char SBT:1;  // Seat Belt, 1=latched
  unsigned char MBK:1;  // Mechanical Brake, 1=engaged
  unsigned char SLT:3;  // StopLight: Red, Yellow, Green
  // multi-bit fields
  unsigned short TCV:10;  // Turn-Counter Value 0-1023
  unsigned short TCR:2;  // Turn-Counter: 0=unknown, 1=low-turn, 2=mid-turn, 3=max-turn
  unsigned short R2E:3;  // Request-To-Enter: bit0=button, bit1=requested, bit2=long-press
  unsigned short MOS:1;  // Manual Override Switch, 1=on
  // HOST fields
  unsigned char R2R:1;  // HOST:Software ready: 1=ready to run
  unsigned char SRO:1;  // HOST:Servo On: 1=on
  unsigned char IMN:1;  // HOST:In Motion: 1=moving
  unsigned char :0;  // new byte
};

union ORVCtrlUnion
{
  ORVCtrlState bit;
  unsigned int val;
};

#endif  // ndef ORV_CTRL_STATE_H
