#ifndef HEADER_H
#define HEADER_H

struct Header
{
  unsigned char HED:2;
};

union HeaderUnion
{
  Header bit;
  byte bytes[1];
};

#endif  // ndef HEADER_H
