#include <ctype.h>

/* borrowed these definitions from apache */
#define ap_tolower(c) (tolower(((unsigned char)(c))))
#define ap_toupper(c) (toupper(((unsigned char)(c))))

char *strcasestr( char *h, char *n )
{ /* h="haystack", n="needle" */
char *a=h, *e=n;

if( !h || !*h || !n || !*n ) { return 0; }
while( *a && *e ) {
if( ap_toupper(*a)!=ap_toupper(*e) ) {
++h; a=h; e=n;
}
else {
++a; ++e;
}
}
return *e ? 0 : h;
}