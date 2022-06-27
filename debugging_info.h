#ifndef debugging_info_h
#define debugging_info_h

extern void set_up(int level);
extern void debug_show(DNSHeader *header, DNSQuestion *question, char IP[], int recv_num, char *rev_buffer);
extern void convertEndian(char *data, int size);
extern void debug_show_send_mes(char IP[], int length);
extern void debug_show_local_DNS(DNSHeader *header, DNSQuestion *question, char IP[], int send_num, char *send_buffer, int isSend, int isClient);

#endif /* debugging_info_h */
