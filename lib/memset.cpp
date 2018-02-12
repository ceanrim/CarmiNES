/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Carmine Foggia $
   ======================================================================== */
extern "C" void* memset(void  *ptr,
                        int    val,
                        size_t num)
{
    if(!(num & 7))
    {
        unsigned __int64 *ptr2 = (unsigned __int64 *)ptr;
        unsigned char valChar = (unsigned char)val;
        unsigned __int64 valCharInt64 = (unsigned __int64)(valChar);
        unsigned __int64 valRepeated =
            valCharInt64|
            valCharInt64 << 8|
            valCharInt64 << 16|
            valCharInt64 << 24|
            valCharInt64 << 32|
            valCharInt64 << 40|
            valCharInt64 << 48|
            valCharInt64 << 54;
        num >>= 3;
        for(size_t i = num - 1; i >= 0 && i < num; i--)
        {
            ptr2[i] = valRepeated;
        }
    }
    else
    {
        unsigned char * ptr2 = (unsigned char *)ptr;
        for(size_t i = num - 1; i >= 0 && i < num; i--)
        {
            ptr2[i] = val;
        }
    }
    return ptr;
}
