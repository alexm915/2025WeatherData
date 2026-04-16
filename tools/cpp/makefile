#开发框架头文件路径
PUBLICL = -I/project/public
#方便调试，没有采用链接库,直接与开发框架源代码一起编译
PUBCPP = /project/public/_public.cpp

#编译选项
#调试
CFLAGS = -g
#正式运行
#CFLAGS = -O2


all:procctl checkproc deletefiles gzipfiles server_test checkproc_test_demo checkproc_test_demo2 ftpgetfiles

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
ftpgetfiles:ftpgetfiles.cpp
# ftpgetfiles程序需要链接libftp.a库和_ftp.cpp源文件，因为它调用了开发框架提供的ftpgetfiles函数
# -no-pie选项是为了生成一个非位置无关的可执行文件，兼容传统PIE可执行文件
	g++ $(CFLAGS) -g ftpgetfiles.cpp -no-pie -o ftpgetfiles $(PUBLICL) $(PUBCPP) /project/public/libftp.a /project/public/_ftp.cpp
	mv -f ftpgetfiles ../bin/.

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
#	rm -rf /tmp/log/checkproc.log


##########################################################
############ 调试命令
##########################################################

cleanftpgetlog:
	rm -rf /log/idc/ftpgetfiles_surfdata.log
cleanftplocal:
	rm -rf /idcdata/surfdata/*

runstart:
	/project/idc/cpp/start.sh
runstop:
	/project/idc/cpp/stop.sh
runftpget_help:
	/project/tools/bin/ftpgetfiles
runftpget2:
#	/project/tools/bin/ftpgetfiles /log/idc/ftpgetfiles_surfdata.log "<host>127.0.0.1</host><mode>1</mode><username>ftp_remote</username><password>225166</password><remotepath>idcdata/surfdata</remotepath><localpath>/idcdata/surfdata</localpath><matchname>SURF_ZH*.XML,SURF_ZH*.CSV</matchname>"
	/project/tools/bin/ftpgetfiles /log/idc/ftpgetfiles_surfdata.log "<host>127.0.0.1</host><mode>1</mode><username>alex</username><password>225166</password><remotepath>/tmp/idc/surfdata</remotepath><localpath>/idcdata/surfdata</localpath><matchname>SURF_ZH*.XML,SURF_ZH*.CSV</matchname><ptype>2</ptype><remotepathbak>/tmp/idc/surfdatabak</remotepathbak>"
runftpget1:
	/project/tools/bin/ftpgetfiles /log/idc/ftpgetfiles_surfdata.log "<host>127.0.0.1</host><mode>1</mode><username>alex</username><password>225166</password><remotepath>/tmp/idc/surfdata</remotepath><localpath>/idcdata/surfdata</localpath><matchname>SURF_ZH*.XML,SURF_ZH*.CSV</matchname><ptype>1</ptype><remotepathbak>/tmp/idc/surfdatabak</remotepathbak>"
runftpget3:
	/project/tools/bin/ftpgetfiles /log/idc/ftpgetfiles_surfdata.log "<host>127.0.0.1</host><mode>1</mode><username>alex</username><password>225166</password><remotepath>/tmp/idc/surfdata</remotepath><localpath>/idcdata/surfdata</localpath><matchname>SURF_ZH*.XML,SURF_ZH*.CSV</matchname><ptype>3</ptype><remotepathbak>/tmp/idc/surfdatabak</remotepathbak>"
catftpgetlog:
	cat /log/idc/ftpgetfiles_surfdata.log

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