PUBINCL = -I/project/public
PUBCPP = /project/public/_public.cpp

ORAINCL = -I/usr/include -I/project/public/db/oracle
ORALIB =  -L/usr/lib/ -L.
ORALIBS = -lclntsh
ORACPP = /project/public/db/oracle/_ooci.cpp

CFLAGS = -g


all:procctl checkproc deletefiles gzipfiles ftpgetfiles ftpputfiles fileserver tcpputfiles dminingoracle

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

db:dminingoracle
rundb:
	/project/tools/bin/dminingoracle "/log/idc/dminingoracle_ZHOBTMIND.log" "<connstr>idc/Alex225166@//192.168.145.128:1521/SHAREDPLATFORM</connstr><charset>Simplified Chinese_China.AL32UTF8</charset><selectsql>select obtid,to_char(ddatetime,'yyyymmddhh24miss'),t,p,u,wd,wf,r,vis,keyid from T_ZHOBTMIND where keyid>:1 and obtid like '5%'</selectsql><fieldstr>obtid,ddatetime,t,p,u,wd,wf,r,vis,keyid</fieldstr><fieldlen>5,19,8,8,8,8,8,8,8,15</fieldlen><bfilename>ZHOBTMIND</bfilename><efilename>togxpt</efilename><outpath>/idcdata/dmindata</outpath><starttime></starttime><incfield>keyid</incfield><incfilename>/idcdata/dmining/dminingoracle_ZHOBTMIND_togxpt.keyid</incfilename><timeout>30</timeout><pname>dminingoracle_ZHOBTMIND_togxpt</pname><maxcount>1000</maxcount><connstr1>alex/Alex225166@//192.168.145.128:1521/XEPDB1</connstr1>"
catlog:
	cat /log/idc/dminingoracle_ZHOBTMIND.log
catfile:
	cat /idcdata/dmining/dminingoracle_ZHOBTMIND_togxpt.keyid
taillog:
	tail -f /log/idc/dminingoracle_ZHOBTMIND.log
headlog:
	head -n 10 /log/idc/dminingoracle_ZHOBTMIND.log
cleanlog:
	rm -rf /log/idc/dminingoracle_ZHOBTMIND.log




cleandb:
	rm -rf /project/tools/bin/dminingoracle
	rm -rf /log/idc/dminingoracle_ZHOBTMIND.log