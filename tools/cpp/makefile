#开发框架头文件路径
PUBLICL = -I/project/public
#方便调试，没有采用链接库,直接与开发框架源代码一起编译
PUBCPP = /project/public/_public.cpp

#编译选项
#调试
CFLAGS = -g
#正式运行
#CFLAGS = -O2


all:procctl checkproc deletefiles gzipfiles server_test checkproc_test_demo checkproc_test_demo2

procctl:procctl.cpp
	g++ procctl.cpp -o procctl
	mv -f procctl ../bin/.
checkproc:checkproc.cpp
	g++ $(CFLAGS) checkproc.cpp -o checkproc $(PUBLICL) $(PUBCPP)
	mv -f checkproc ../bin/.
deletefiles:deletefiles.cpp
	g++ $(CFLAGS) deletefiles.cpp -o deletefiles $(PUBLICL) $(PUBCPP)
	mv -f deletefiles ../bin/.
gzipfiles:gzipfiles.cpp
	g++ $(CFLAGS) gzipfiles.cpp -o gzipfiles $(PUBLICL) $(PUBCPP)
	mv -f gzipfiles ../bin/.

server_test:server_test.cpp
	g++ $(CFLAGS) server_test.cpp -o server_test $(PUBLICL) $(PUBCPP)
	mv -f server_test ../bin/.
checkproc_test_demo:checkproc_test_demo.cpp
	g++ $(CFLAGS) checkproc_test_demo.cpp -o checkproc_test_demo $(PUBLICL) $(PUBCPP)
	mv -f checkproc_test_demo ../bin/.
checkproc_test_demo2:checkproc_test_demo2.cpp
	g++ $(CFLAGS) checkproc_test_demo2.cpp -o checkproc_test_demo2 $(PUBLICL) $(PUBCPP)
	mv -f checkproc_test_demo2 ../bin/.

clean:
	rm -rf ../bin/*
	rm -rf /tmp/log/checkproc.log

runstart:
	/project/idc/cpp/start.sh
runstop:
	/project/idc/cpp/stop.sh

# 调试命令
runprocctl:
	/project/tools/bin/procctl 60 /project/idc/bin/crtsurfdata /project/idc/ini/stcode.ini /tmp/idc/surfdata /log/idc/crtsurfdata.log csv,xml,json
runcheckproc:
	/project/tools/bin/procctl 10 /project/tools/bin/checkproc /tmp/log/checkproc.log
catchecklog:
	cat /tmp/log/checkproc.log
runchecktmp:
	/project/tools/bin/checkproc /tmp/log/checkproc.log
runserver_test:
	/project/tools/bin/server_test
runcheckproc_test_demo:
	/project/tools/bin/checkproc_test_demo 100 10
runcheckproc_test_demo_timeout:
	/project/tools/bin/checkproc_test_demo 10 100
runcheckproc_test_demo_timeout2:
	/project/tools/bin/checkproc_test_demo2 10 1000