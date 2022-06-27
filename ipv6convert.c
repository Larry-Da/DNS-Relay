#include <stdio.h>
#include <string.h>
#include <stdlib.h>
void strrev(char str[])
{
    int i = 0;
    int j = strlen(str) - 1;
    while(i < j)
    {
        char temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
}
int ipv6convert(const char *point, unsigned int result[4]) {
    for (int i = 0; i < 4; i++) {
        result[i] = 0;
    }
    char buf[40] = {0}, revbuf[40], p = 0, q = 0;
    strcpy(buf, point);
    strcpy(revbuf, point);
    strrev(revbuf);
    buf[strlen(point)] = ':';
    revbuf[strlen(point)] = ':';
    for(int i = 0;i < 8; i++) {
        q = strchr(buf+p, ':') - buf;
        buf[q] = '\0';
        if (i % 2 == 0) {
            result[i/2] = 0;
            result[i/2] = strtol(buf+p, NULL, 16)<<16;
        } else {
            result[i/2] += strtol(buf+p, NULL, 16);
        }
        p = q + 1;
        // if we find  ::, then we should scan revbuf.2019:1::1 1::1:2019
        if (buf[p] == ':') {
            p = q = 0;
            for (int j = 7;j > i;j--) {
                q = strchr(revbuf+p, ':') - revbuf;
                revbuf[q] = '\0';
                strrev(revbuf+p);
                if (j % 2 == 0) {
                    result[j/2] += strtol(revbuf+p, NULL, 16)<<16;
                } else {
                    result[j/2] = 0;
                    result[j/2] += strtol(revbuf+p, NULL, 16);
                }
                p = q + 1;
                if (revbuf[p] == ':') {
                    break;
                }
            }
            break;
        }
    }
    return 1;
}
 
