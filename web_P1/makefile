CC = gcc -Wall -c -I $(INCLUDE_DIR)
LCC = gcc -Wall -o

TARGET_S = server
TARGET_C = client

OBJS_DIR = ./obj
SRC_DIR = ./src
INCLUDE_DIR = ./includes
LIB_DIR = ./lib

OBJS = $(OBJS_DIR)/utils.o
OBJS_C = $(OBJS) $(OBJS_DIR)/client.o 
OBJS_S = $(OBJS) $(OBJS_DIR)/server.o $(OBJS_DIR)/http.o

HEADERS = client.h server.h utils.h http.h

.PHONY: all clean

all: $(TARGET_S) $(TARGET_C)

$(TARGET_S): $(OBJS_S) $(LIB_DIR)/libnet_lvl.a $(LIB_DIR)/libpicohttpparser.a
	$(LCC) $@ $^ -g -lm -lpthread -lconfuse 

$(TARGET_C): $(OBJS_C) $(LIB_DIR)/libnet_lvl.a $(LIB_DIR)/libpicohttpparser.a
	$(LCC) $@ $^ -lm -lpthread -lconfuse 

$(OBJS_DIR)/%.o: $(SRC_DIR)/%.c $(INCLUDE_DIR)/%.h | $(OBJS_DIR)
	$(CC) $< -o $@

$(OBJS_DIR):
	mkdir -p $(OBJS_DIR)

clean:
	rm -rf $(OBJS_DIR) $(TARGET_S) $(TARGET_C)
