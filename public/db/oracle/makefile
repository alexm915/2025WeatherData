# oracle头文件路径
ORAINCL = -I/usr/include
# oracle库文件路径
ORALIB =  -L/usr/lib/
# oracle的oci库
ORALIBS = -lclntsh

#CFLAGS = -O2
#CFLAGS = -O2 -Wall
CFLAGS = -g -Wall -Wno-write-strings #-Wno-unused-variable

all:demo

demo: demo.cpp _ooci.h _ooci.cpp
	g++ $(CFLAGS) demo.cpp -o demo $(ORAINCL) $(ORALIB) $(ORALIBS) _ooci.cpp 
	mv -f ./demo ./bin/.

clean:
	rm -rf ./bin/*

# 调试command
rundemo:
	./bin/demo