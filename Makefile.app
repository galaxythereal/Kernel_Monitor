CC = arm-none-linux-gnueabihf-gcc
CFLAGS = -Wall -g

all: monitor_app

monitor_app: monitor_app.c
	$(CC) $(CFLAGS) -o monitor_app monitor_app.c

clean:
	rm -f monitor_app
