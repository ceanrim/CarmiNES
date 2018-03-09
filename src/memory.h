#if !defined(MEMORY_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Carmine Foggia $
   ======================================================================== */
#define MEMORY_H
#define ADDR_INDIRECT_X   0b00000000
#define ADDR_ZERO_PAGE    0b00000100
#define ADDR_IMMEDIATE    0b00001000
#define ADDR_ABSOLUTE     0b00001100
#define ADDR_INDIRECT_Y   0b00010000
#define ADDR_ZERO_PAGE_X  0b00010100
#define ADDR_ABSOLUTE_Y   0b00011000
#define ADDR_ABSOLUTE_X   0b00011100
#define ADDR_ZERO_PAGE_Y  0b00100000
#define ADDR_IMPLIED      0b00100100
#define ADDR_ACCUMULATOR  0b00101000
#define ADDR_RELATIVE     0b00101100
#define ADDR_ABSOLUTE_IND 0b00110000
class MemoryClass
{
private:
    unsigned short AddressBus; //To hold data between cycles
    unsigned short temp; //Need it for indexed indirect
    bool AddressCarry;
public:
    MemoryClass();
    unsigned char Read(unsigned short); //Reads the memory
                                        //at the corresponding address
    unsigned char ReadWithNoSideEffects(unsigned short);
    void Read(unsigned short address,   //Processes addressing modes
              unsigned char* cycle,     //in a cycle-accurate way
              unsigned char  addrMode,
              unsigned char* valueToRewrite);
    void Write(unsigned short address,
               unsigned char valueToWrite);
    void Write(unsigned char valueToWrite,
               unsigned char* cycle,
               unsigned char addrMode);
    unsigned char ConversionTable[8]; //for xxxxxx00 instructions
    unsigned short Mapper;
    unsigned short PRGROMSize;
};

#endif
