PUBINCL = -I/project/public
PUBCPP = /project/public/_public.cpp

CFLAGS = -g

all:crtsurfdata

crtsurfdata:crtsurfdata.cpp
	g++ $(CFLAGS) -o crtsurfdata crtsurfdata.cpp $(PUBINCL) $(PUBCPP) -lm -lc
	cp -f crtsurfdata ../bin/.

clean:
	rm -f crtsurfdata