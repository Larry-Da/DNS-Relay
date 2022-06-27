SRCS := ./src/*.c
OBJS := *.o
${OBJS}:${SRCS}
	gcc -std=gnu99 -pthread -c ${SRCS}
dnsrelay:${OBJS}
	gcc -std=gnu99 -pthread -o dnsrelay ${OBJS}
clean:
	-rm ${OBJS} 
deploy:
	./start.sh
