CROSS_COMPILE ?= arm-linux-
CC = $(CROSS_COMPILE)gcc 

TARGET	=  NRF24L01
SRC 	=  $(wildcard *.c)
SRC 	+= $(wildcard *.h)
$(TARGET):$(SRC)
	$(CC) -o $@ -L ../../lib/ -I ../../lib/includes/ $^ -lfahw -lm -Wall
.PHONY: clean
clean:
	rm -rf $(TARGET)
