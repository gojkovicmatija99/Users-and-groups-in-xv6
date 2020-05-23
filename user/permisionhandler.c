#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user.h"
#include "usergroups.h"

char* getPermisionsString(int permisions, short type, char* permisionsString)
{
   if(type==T_DIR)
      strcpy(permisionsString, "d");
   else
      strcpy(permisionsString, "-");

   for(int i=2;i>=0;i--) {
      int bitMask=7 << i*3;
      int bitGroup=bitMask & permisions;
      bitGroup=bitGroup >> i*3;

      if(bitGroup & READ)
         strcat(permisionsString, "r");
      else
         strcat(permisionsString, "-");

      if(bitGroup & WRITE)
         strcat(permisionsString, "w");
      else
         strcat(permisionsString, "-");

      if(bitGroup & EXECUTE)
         strcat(permisionsString, "x");
      else
         strcat(permisionsString, "-");
   }

   permisionsString[10]='\0';
   return permisionsString;
}