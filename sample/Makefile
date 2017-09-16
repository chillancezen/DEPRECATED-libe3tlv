all:
	gcc -o server -I ../include -le3tlv -lpthread -lzmq -L.. test-main.c
	gcc -o client -I ../include -le3tlv -lpthread -lzmq -L.. test-client.c
clean:
	rm -f server
	rm -f client
