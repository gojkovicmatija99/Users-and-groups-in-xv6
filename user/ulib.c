	#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user.h"
#include "kernel/x86.h"

char*
strcpy(char *s, const char *t)
{
	char *os;

	os = s;
	while((*s++ = *t++) != 0)
		;
	return os;
}

char*
strncpy(char *s, const char *t, int n)
{
	char *os;

	os = s;
	while(n-- > 0 && (*s++ = *t++) != 0)
		;
	while(n-- > 0)
		*s++ = 0;
	return os;
}

// Like strncpy but guaranteed to NUL-terminate.
char*
safestrcpy(char *s, const char *t, int n)
{
	char *os;

	os = s;
	if(n <= 0)
		return os;
	while(--n > 0 && (*s++ = *t++) != 0)
		;
	*s = 0;
	return os;
}

int
strcmp(const char *p, const char *q)
{
	while(*p && *p == *q)
		p++, q++;
	return (uchar)*p - (uchar)*q;
}

uint
strlen(const char *s)
{
	int n;

	for(n = 0; s[n]; n++)
		;
	return n;
}

void*
memset(void *dst, int c, uint n)
{
	stosb(dst, c, n);
	return dst;
}

char*
strchr(const char *s, char c)
{
	for(; *s; s++)
		if(*s == c)
			return (char*)s;
	return 0;
}

char*
gets(char *buf, int max)
{
	int i, cc;
	char c;

	for(i=0; i+1 < max; ){
		cc = read(0, &c, 1);
		if(cc < 1)
			break;
		buf[i++] = c;
		if(c == '\n' || c == '\r')
			break;
	}
	buf[i] = '\0';
	return buf;
}

int
stat(const char *n, struct stat *st)
{
	int fd;
	int r;

	fd = open(n, O_RDONLY);
	if(fd < 0)
		return -1;
	r = fstat(fd, st);
	close(fd);
	return r;
}

int
atoi(const char *s)
{
	int n;

	n = 0;
	while('0' <= *s && *s <= '9')
		n = n*10 + *s++ - '0';
	return n;
}

void*
memmove(void *vdst, const void *vsrc, int n)
{
	char *dst;
	const char *src;

	dst = vdst;
	src = vsrc;
	while(n-- > 0)
		*dst++ = *src++;
	return vdst;
}

char* strtok(char* str, const char* delimiters) {
 
 	static char* sp = NULL;
    int i = 0;
    int len = strlen(delimiters);
 
    /* if the original string has nothing left */
    if(!str && !sp)
        return NULL;
 
    /* initialize the sp during the first call */
    if(str && !sp)
        sp = str;
 
    /* find the start of the substring, skip delimiters */
    char* p_start = sp;
    while(1) {
        for(i = 0; i < len; i ++) {
            if(*p_start == delimiters[i]) {
                p_start ++;
                break;
            }
        }
 
        if(i == len) {
               sp = p_start;
               break;
        }
    }
 
    /* return NULL if nothing left */
    if(*sp == '\0') {
        sp = NULL;
        return sp;
    }
 
    /* find the end of the substring, and
        replace the delimiter with null */
    while(*sp != '\0') {
        for(i = 0; i < len; i ++) {
            if(*sp == delimiters[i]) {
                *sp = '\0';
                break;
            }
        }
 
        sp ++;
        if (i < len)
            break;
    }
 
    return p_start;
}

char* strtok2(char* str, const char* delimiters) {
 
 	static char* sp = NULL;
    int i = 0;
    int len = strlen(delimiters);
 
    /* if the original string has nothing left */
    if(!str && !sp)
        return NULL;
 
    /* initialize the sp during the first call */
    if(str && !sp)
        sp = str;
 
    /* find the start of the substring, skip delimiters */
    char* p_start = sp;
    while(1) {
        for(i = 0; i < len; i ++) {
            if(*p_start == delimiters[i]) {
                p_start ++;
                break;
            }
        }
 
        if(i == len) {
               sp = p_start;
               break;
        }
    }
 
    /* return NULL if nothing left */
    if(*sp == '\0') {
        sp = NULL;
        return sp;
    }
 
    /* find the end of the substring, and
        replace the delimiter with null */
    while(*sp != '\0') {
        for(i = 0; i < len; i ++) {
            if(*sp == delimiters[i]) {
                *sp = '\0';
                break;
            }
        }
 
        sp ++;
        if (i < len)
            break;
    }
 
    return p_start;
}

char* strcat(char* destination, const char* source)
{
	// make ptr point to the end of destination string
	char* ptr = destination + strlen(destination);

	// Appends characters of source to the destination string
	while (*source != '\0')
		*ptr++ = *source++;

	// null terminate destination string
	*ptr = '\0';

	// destination is returned by standard strcat()
	return destination;
}

// inline function to swap two numbers
inline void swap(char *x, char *y) {
	char t = *x; *x = *y; *y = t;
}

// function to reverse buffer[i..j]
char* reverse(char *buffer, int i, int j)
{
	while (i < j)
		swap(&buffer[i++], &buffer[j--]);

	return buffer;
}

// Iterative function to implement itoa() function in C
char* itoa(int value, char* buffer, int base)
{
	// invalid input
	if (base < 2 || base > 32)
		return buffer;

	// consider absolute value of number
	int n = abs(value);

	int i = 0;
	while (n)
	{
		int r = n % base;

		if (r >= 10) 
			buffer[i++] = 65 + (r - 10);
		else
			buffer[i++] = 48 + r;

		n = n / base;
	}

	// if number is 0
	if (i == 0)
		buffer[i++] = '0';

	// If base is 10 and value is negative, the resulting string 
	// is preceded with a minus sign (-)
	// With any other base, value is always considered unsigned
	if (value < 0 && base == 10)
		buffer[i++] = '-';

	buffer[i] = '\0'; // null terminate string

	// reverse the string and return it
	return reverse(buffer, 0, i - 1);
}

int abs(int n)
{
	return n<0?-n:n;
}

int isEmptyString(char* string)
{
   if(string[0]==0)
      return 1;
   return 0;
}

int pow(int base, int exponent)
{
	int result=1;
	for(int i=0;i<exponent;i++)
		result*=base;

	return result;
}
