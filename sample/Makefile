all:
	gcc -o server -I ../include -le3tlv -lzmq -L.. test-main.c
	gcc -o client -I ../include -le3tlv -lzmq -L.. test-client.c
clean:
	rm -f server
	rm -f client
