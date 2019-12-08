CXX := g++
CXXFLAGS := -std=c++11 -g


all : server client

target2 : target1 target0
	@echo "target2..."

target1 :
	@echo "target1 making ..."

target0 : 
	@echo "target0 making ..."

server : server.o
	g++ $(CXXFLAGS) -o server.bin $<
	@echo "compile server.bin ok."

client : client.o
	g++ $(CXXFLAGS) -o client.bin $<
	@echo "compile client.bin ok."

.cc.o:
	echo -n "g++ Compiling $<... "
	@g++ $(CXXFLAGS) -c -o $@ $<
	@echo "Done."


.PHONY: clean

clean:
	rm *.o
	rm *.bin