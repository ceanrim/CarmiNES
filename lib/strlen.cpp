/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Carmine Foggia $
   ======================================================================== */
extern "C" size_t strlen(char const* str)
{
    for(size_t i = 0;
        ;
        i++)
    {
        if(str[i] == 0)
        {
            return i;
        }
    }
}
