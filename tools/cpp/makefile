#开发框架头文件路径
PUBLICL = -I/project/public
#方便调试，没有采用链接库,直接与开发框架源代码一起编译
PUBCPP = /project/public/_public.cpp

#编译选项
CFLAGS = -g
#CFLAGS = -O2


all:procctl deletefiles gzipfiles server_test
procctl:procctl.cpp
	g++ procctl.cpp -o procctl
	mv -f procctl ../bin/.

deletefiles:deletefiles.cpp
	g++ $(CFLAGS) deletefiles.cpp -o deletefiles $(PUBLICL) $(PUBCPP)
	mv -f deletefiles ../bin/.

gzipfiles:gzipfiles.cpp
	g++ $(CFLAGS) gzipfiles.cpp -o gzipfiles $(PUBLICL) $(PUBCPP)
	mv -f gzipfiles ../bin/.

server_test:server_test.cpp
	g++ $(CFLAGS) server_test.cpp -o server_test $(PUBLICL) $(PUBCPP)
	mv -f server_test ../bin/.

clean:
	rm -rf ../bin/*

runprocctl:
	../bin/procctl 60 /project/idc/bin/crtsurfdata /project/idc/ini/stcode.ini /tmp/idc/surfdata /log/idc/crtsurfdata.log csv,xml,json
runserver_test:
	../bin/server_test