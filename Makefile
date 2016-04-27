# @author Erick Garcia <egarcia87@miners.utep.edu>
# @desc   Makefile for EE4374 Lab04

#All the dependencies for this project
TARGET_CLIENT  = bankClient   				# executable for the client
TARGET_SERVER  = bankServer   				# executable for the server
HEADERS = banking.h               	  # headers with signatures
OBJECTS_CLIENT = bankClient.o 				# All the object files
OBJECTS_SERVER = bankServer.o 				# All the object files
CC      = gcc                         # Compiler
CFLAGS  = -c 													# Flags

#Make all will target the default target which is the executable
#declared above
default: all

all: $(TARGET_CLIENT) $(TARGET_SERVER)

#Compiling our executables
$(TARGET_CLIENT): $(OBJECTS_CLIENT)
	$(CC) $(OBJECTS_CLIENT) -o $@ 

$(TARGET_SERVER): $(OBJECTS_SERVER)
	$(CC) $(OBJECTS_SERVER) -o $@ 

#Compiling all the object files that our executables are dependent on:
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) $< -o $@ 

#Run make run to execute the target(s)
run_client: $(TARGET_CLIENT)
	./$(TARGET_CLIENT) 127.0.0.1 1024 theCommand theAccNum theValue

run_server: $(TARGET_SERVER)
	./$(TARGET_SERVER) 1024

#Removing all of the files except the .c's
clean:
	-rm -f *.o
	-rm -f $(TARGET_CLIENT)
	-rm -f $(TARGET_SERVER) 

