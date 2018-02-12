/* ========================================================================
   File: 
   Date: 
   Revision: 
   Creator: Carmine Foggia 
   ======================================================================== */
extern "C" void* memcpy(void       *des,
                        void const *src,
                        size_t      len)
{
    if(!(len & 7))
    {
        unsigned __int64 *src2 = (unsigned __int64 *)src,
                         *des2 = (unsigned __int64 *)des;
        len >>= 3;
        for(size_t i = len - 1; i < len; i--)
        {
            des2[i] = src2[i];
        }
    }
    else
    {
        unsigned char *src2 = (unsigned char *)src,
                      *des2 = (unsigned char *)des;
        for(size_t i = len - 1; i < len; i--)
        {
            des2[i] = src2[i];
        }
    }
    return des;
}
