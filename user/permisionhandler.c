#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user.h"
#include "usergroups.h"

void getPermisionsString(int permisions, short type, char* permisionsString)
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
}

int convertOctalToDecimal(int octalNumber)
{
	int decimalNumber = 0, i = 0;

	while(octalNumber != 0)
	{
		decimalNumber += (octalNumber%10) * pow(8,i);
		++i;
		octalNumber/=10;
	}

	i = 1;

	return decimalNumber;
}

int convertStringToMode(char* modeString, int oldMode)
{
	int newMode;

	if(strlen(modeString)!=3)
		return -1;

	char who=modeString[0];
	int shift=0;
	switch(who) {
		case 'a':	shift=3;	break;
		case 'u':	shift=2;	break;
		case 'g':	shift=1;	break;
		case 'o':	shift=0;	break;
		default :	return -1;	break;
	}

	char permChar=modeString[2];
	int permBit;
	switch(permChar) {
		case 's': 	permBit=SETUID;		break;
		case 'r':	permBit=READ;		break;
		case 'w':	permBit=WRITE;		break;
		case 'x':	permBit=EXECUTE;	break;
		default :	return -1;			break;
	}

	if(permChar=='s' && who!='u')				// special case, only valid format for setuid is u+s or u-s
		return -1;

	int bitMask=0;
	if(shift==3) 								// if option a is selected, add permBit to every bit group
		for(int i=0;i<shift;i++) {
			bitMask=bitMask << 3; 
			bitMask=bitMask | permBit;
		}
		else
			bitMask=permBit << shift*3;			// else only add permBit only to one bit group

	char op=modeString[1];
	if(op=='+') 
		newMode=oldMode | bitMask;
	else if(op=='-')
		newMode=oldMode & (~bitMask);
	else
		return -1;

	return newMode;
}