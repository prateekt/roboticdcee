CC = g++
dir = /root/player-2.1.1

INCLUDE = \
	-I. \
	-I$(dir) \
	-I$(dir)/libplayercore/ \
	-I$(dir)/client_libs/ \
	-I$(dir)/client_libs/libplayerc++\ 

LDLIBS = \
       -lpthread `pkg-config --cflags playerc++` `pkg-config --libs playerc++`	
	

SRCS = \
	main.c \
	Network.c \
	helper.c \
	dcop.c \
	iwspy.c \
	create_lib.c \
	sampler.c \
	config.c

OUTPUT = dcop 

all:
	$(CC) $(CFLAGS) $(INCLUDE) $(SRCS) $(LDFLAGS) $(LDLIBS) -o $(OUTPUT)

clean:
	rm -rf *.o dcop 
	rm $(OUTPUT)






