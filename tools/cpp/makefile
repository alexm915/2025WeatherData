#开发框架头文件路径
PUBLICL = -I/project/public
#方便调试，没有采用链接库,直接与开发框架源代码一起编译
PUBCPP = /project/public/_public.cpp

#编译选项
#调试
CFLAGS = -g
#正式运行
#CFLAGS = -O2


all:procctl checkproc deletefiles gzipfiles ftpgetfiles ftpputfiles	

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
ftpputfiles:ftpputfiles.cpp
	g++ $(CFLAGS) -g ftpputfiles.cpp -no-pie -o ftpputfiles $(PUBLICL) $(PUBCPP) /project/public/libftp.a /project/public/_ftp.cpp
	mv -f ftpputfiles ../bin/.

clean:
	rm -rf ../bin/*
#	rm -rf /tmp/log/checkproc.log


##########################################################
############ 调试命令
##########################################################
runstart:
	/project/idc/cpp/start.sh
runstop:
	/project/idc/cpp/stop.sh

catputlog:
	cat /log/idc/ftpputfiles_surfdata.log
catgetlog:
	cat /log/idc/ftpgetfiles_surfdata.log
cleanftplog:
	rm -rf /log/idc/ftpputfiles_surfdata.log
	rm -rf /log/idc/ftpgetfiles_surfdata.log
cleansurfdata:
	rm -rf /tmp/idc/surfdata