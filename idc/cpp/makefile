PUBINCL = -I/project/public
PUBCPP = /project/public/_public.cpp

ORAINCL = -I/usr/include -I/project/public/db/oracle
ORALIB =  -L/usr/lib/ -L.
ORALIBS = -lclntsh
ORACPP = /project/public/db/oracle/_ooci.cpp

CFLAGS = -g


all:crtsurfdata obtcodetodb

crtsurfdata:crtsurfdata.cpp
	g++ $(CFLAGS) crtsurfdata.cpp -o crtsurfdata $(PUBINCL) $(PUBCPP) -lm -lc
	mv -f crtsurfdata ../bin/.
obtcodetodb:obtcodetodb.cpp
	g++ $(CFLAGS) obtcodetodb.cpp -o obtcodetodb $(PUBINCL) $(PUBCPP) $(ORAINCL) $(ORALIB) $(ORALIBS) $(ORACPP)
	mv -f obtcodetodb ../bin/.

clean:
	rm -rf ../bin/*
	rm -f /log/idc/crtsurfdata.log
	rm -rf /tmp/idc/surfdata
cleandb:
	rm -f ../bin/obtcodetodb
	rm -f /log/idc/obtcodetodb.log

runstart:
	/project/idc/cpp/start.sh
runstop:
	/project/idc/cpp/stop.sh

# 调试命令
runmain:
	/project/tools/bin/procctl 60 /project/idc/bin/crtsurfdata /project/idc/ini/stcode.ini /tmp/idc/surfdata /log/idc/crtsurfdata.log csv,xml,json
catmainlog:
	cat /log/idc/crtsurfdata.log

rundb:
	/project/idc/bin/obtcodetodb /project/idc/ini/stcode.ini "idc/Alex225166@//192.168.145.128:1521/SHAREDPLATFORM" "Simplified Chinese_China.AL32UTF8" /log/idc/obtcodetodb.log
catdb:
	cat /log/idc/obtcodetodb.log