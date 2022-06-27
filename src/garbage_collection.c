#include "dns.h"
#include <stdlib.h>

void free_question_list(DNSQuestion *question_list)
{
    DNSQuestion *p = question_list;
    while(p)
    {
        if(p->qName)
        {
            free(p->qName);
            p->qName = NULL;
        }
        DNSQuestion *temp = p;
        p = p->next;
        free(temp);
    }
}
void free_response(Response *response)
{
    DNSRR *p = response->firstRR;
    while(p)
    {
        free(p->rName);
        free(p->rData);
        DNSRR *temp = p;
        p = p->next;
        free(temp);
    }
    free(response);
}
