PUBINCL = -I/project/public
PUBCPP = /project/public/_public.cpp

ORAINCL = -I/usr/include -I/project/public/db/oracle
ORALIB =  -L/usr/lib/ -L.
ORALIBS = -lclntsh
ORACPP = /project/public/db/oracle/_ooci.cpp

CFLAGS = -g


all:crtsurfdata obtcodetodb obtmindtodb

crtsurfdata:crtsurfdata.cpp
	g++ $(CFLAGS) crtsurfdata.cpp -o crtsurfdata $(PUBINCL) $(PUBCPP) -lm -lc
	mv -f crtsurfdata ../bin/.
obtcodetodb:obtcodetodb.cpp
	g++ $(CFLAGS) obtcodetodb.cpp -o obtcodetodb $(PUBINCL) $(PUBCPP) $(ORAINCL) $(ORALIB) $(ORALIBS) $(ORACPP)
	mv -f obtcodetodb ../bin/.
obtmindtodb:obtmindtodb.cpp idcapp.h idcapp.cpp
	g++ $(CFLAGS) obtmindtodb.cpp idcapp.cpp -o obtmindtodb $(PUBINCL) $(PUBCPP) $(ORAINCL) $(ORALIB) $(ORALIBS) $(ORACPP)
	mv -f obtmindtodb ../bin/.

clean:
	rm -rf ../bin/*
	rm -f /log/idc/crtsurfdata.log
	rm -rf /tmp/idc/surfdata
	rm -rf /project/idc/bin/obtmindtodb
	rm -rf /log/idc/obtmindtodb.log
runstart:
	/project/idc/cpp/start.sh
runstop:
	/project/idc/cpp/stop.sh