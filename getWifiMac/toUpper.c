#include <stdio.h>
#include <ctype.h>

int toUpperCase(char *pStr)
{
   int i = 0;
    
   while(pStr[i])
   {
      pStr[i] = toupper(pStr[i]);
      i++;
   }
   
   return(0);
}
