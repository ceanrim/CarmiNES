#if !defined(PPU_H)
/* ========================================================================
    File: 
    Date: 
    Revision: 
    Creator: Carmine Foggia 
   ======================================================================== */
#define PPU_H
#define NTSC_VBLANK_CYCLE       82182
#define NTSC_VBLANK_UNSET_CYCLE 89002
class PPUClass
{
public:
    unsigned long long LastEmulatedCycle;
    unsigned           Scanline;
    unsigned           Dot;
    unsigned           EvenFrame;
    unsigned char* NextFrameBuffer;
    unsigned char  Register2002;
    unsigned char* Nametables[4];
    unsigned char  Nametable0[1024];
    unsigned char  Nametable1[1024];
    unsigned char  Nametable2[1024];
    unsigned char  Nametable3[1024];
    unsigned char* PatternTables[2];
    unsigned char  OAM[256];
    unsigned char  IODB;
    unsigned char  NametableBase;
    unsigned char  PatternTableBase;
    unsigned char  Mirroring;
    unsigned char *CHRROM;
    size_t         CHRROMSize;
    PPUClass();
    void Init(unsigned short);
    void Run(unsigned long long);
};
#endif
