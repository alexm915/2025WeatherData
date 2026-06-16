PUBINCL = -I/project/public
PUBCPP = /project/public/_public.cpp

ORAINCL = -I/usr/include -I/project/public/db/oracle
ORALIB =  -L/usr/lib/ -L.
ORALIBS = -lclntsh
ORACPP = /project/public/db/oracle/_ooci.cpp

CFLAGS = -g


all:procctl checkproc deletefiles gzipfiles ftpgetfiles ftpputfiles fileserver tcpputfiles dminingoracle xmltodb

procctl:procctl.cpp
	g++ procctl.cpp -o procctl
	mv -f procctl ../bin/.
checkproc:checkproc.cpp
	g++ $(CFLAGS) checkproc.cpp -o checkproc $(PUBINCL) $(PUBCPP)
	mv -f checkproc ../bin/.
deletefiles:deletefiles.cpp
	g++ $(CFLAGS) deletefiles.cpp -o deletefiles $(PUBINCL) $(PUBCPP)
	mv -f deletefiles ../bin/.
gzipfiles:gzipfiles.cpp
	g++ $(CFLAGS) gzipfiles.cpp -o gzipfiles $(PUBINCL) $(PUBCPP)
	mv -f gzipfiles ../bin/.
ftpgetfiles:ftpgetfiles.cpp
# ftpgetfiles程序需要链接libftp.a库和_ftp.cpp源文件，因为它调用了开发框架提供的ftpgetfiles函数
# -no-pie选项是为了生成一个非位置无关的可执行文件，兼容传统PIE可执行文件
	g++ $(CFLAGS) -g ftpgetfiles.cpp -no-pie -o ftpgetfiles $(PUBINCL) $(PUBCPP) /project/public/libftp.a /project/public/_ftp.cpp
	mv -f ftpgetfiles ../bin/.
ftpputfiles:ftpputfiles.cpp
	g++ $(CFLAGS) -g ftpputfiles.cpp -no-pie -o ftpputfiles $(PUBINCL) $(PUBCPP) /project/public/libftp.a /project/public/_ftp.cpp
	mv -f ftpputfiles ../bin/.
fileserver:fileserver.cpp
	g++ $(CFLAGS) fileserver.cpp -no-pie -o fileserver $(PUBINCL) $(PUBCPP) /project/public/libftp.a /project/public/_ftp.cpp
	mv -f fileserver ../bin/.
tcpputfiles:tcpputfiles.cpp
	g++ $(CFLAGS) -g tcpputfiles.cpp -no-pie -o tcpputfiles $(PUBINCL) $(PUBCPP) /project/public/libftp.a /project/public/_ftp.cpp
	mv -f tcpputfiles ../bin/.
dminingoracle:dminingoracle.cpp
	g++ $(CFLAGS) dminingoracle.cpp -o dminingoracle $(PUBINCL) $(PUBCPP) $(ORAINCL) $(ORALIB) $(ORALIBS) $(ORACPP)
	mv -f dminingoracle ../bin/.
xmltodb:xmltodb.cpp _tools.cpp
	g++ $(CFLAGS) xmltodb.cpp _tools.cpp -g -o xmltodb $(PUBINCL) $(PUBCPP) $(ORAINCL) $(ORALIB) $(ORALIBS) $(ORACPP)
	mv -f xmltodb ../bin/.

clean:
	rm -rf ../bin/*
#	rm -rf /tmp/log/checkproc.log

runstart:
	/project/idc/cpp/start.sh
runstop:
	/project/idc/cpp/stop.sh
##########################################################
############ 调试命令
##########################################################
tcp:tcpputfiles fileserver
catput:
	cat /log/idc/tcpputfiles_surfdata.log
catget:
	cat /log/idc/tcpgetfiles_surfdata.log
catserver:
	cat /log/idc/fileserver.log
cleantcp:
	rm -rf /project/tools/bin/tcpputfiles
	rm -rf /project/tools/bin/fileserver
	rm -rf /log/idc/tcpputfiles_surfdata.log
	rm -rf /log/idc/tcpgetfiles_surfdata.log
	rm -rf /log/idc/fileserver.log

db:xmltodb
cleandb:
	rm -rf /project/tools/bin/xmltodb
	rm -rf /log/idc/xmltodb_vip.log
rundb:
	/project/tools/bin/xmltodb "/log/idc/xmltodb_vip.log" "<connstr>idc/Alex225166@//192.168.145.128:1521/SHAREDPLATFORM</connstr><charset>Simplified Chinese_China.AL32UTF8</charset><inifilename>/project/idc/ini/xmltodb.xml</inifilename><xmlpath>/idcdata/xmltodb/vip</xmlpath><xmlpathbak>/idcdata/xmltodb/vipbak</xmlpathbak><xmlpatherr>/idcdata/xmltodb/viperr</xmlpatherr><timetvl>5</timetvl><timeout>50</timeout><pname>xmltodb_vip</pname>"
catlog:
	cat /log/idc/xmltodb_vip.log
lsv:
	ls /idcdata/xmltodb/vipbak
lsvr:
	ls /idcdata/xmltodb/viper
taillog:
	tail -f /log/idc/xmltodb_vip.log
headlog:
	head -n 10 /log/idc/xmltodb_vip.log
cleanlog:
	rm -rf /log/idc/xmltodb_vip.log
