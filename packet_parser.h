#ifndef packet_parser_h
#define packet_parser_h

extern int parse_query(char packet[], DNSHeader* header, DNSQuestion* question);
extern int parse_response(char packet[], Response* response);
extern void convert_string(char src[], char des[]);

#endif /* packet_parser_h */
