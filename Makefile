CC=gcc

TARGET:=test

SRCS :=$(wildcard src/*.c) main.c 

CFLAG:= -std=c89 -W -Wall -O3

$(TARGET):$(SRCS)
	$(CC) -o $@ $^ $(CFLAG) 

.PHONY:clean
clean:
	@rm -rvf $(TARGET) 


