#include <ctype.h>

/* borrowed these definitions from apache */
#define ap_tolower(c) (tolower(((unsigned char)(c))))
#define ap_toupper(c) (toupper(((unsigned char)(c))))

char *strcasestr(const char *h,const char *n )
{ /* h="haystack", n="needle" */
const char *a=h, *e=n;

if( !h || !*h || !n || !*n ) { return 0; }
while( *a && *e ) {
if( ap_toupper(*a)!=ap_toupper(*e) ) {
++h; a=h; e=n;
}
else {
++a; ++e;
}
}
return (char *)(*e ? 0 : h);
}