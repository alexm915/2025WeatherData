# 开发框架搜索目录
PUBINCL = -I/project/public
# 开发框架源码，没有使用链接库是为了方便调试:可以进入库函数里面，链接库不可以
PUBCPP = /project/public/_public.cpp
#编译参数 -g生成调试信息，-o指定生成的可执行文件名，-lm链接数学库，-lc链接c库
CFLAGS = -g

all:crtsurfdata

crtsurfdata:crtsurfdata.cpp
	g++ $(CFLAGS) -o crtsurfdata crtsurfdata.cpp $(PUBINCL) $(PUBCPP) -lm -lc
	mv -f crtsurfdata ../bin/.

clean:
	rm -rf ../bin/*
	rm -f /log/idc/crtsurfdata.log
	rm -rf /tmp/idc/surfdata

# 调试命令
runmain:
	/project/tools/bin/procctl 60 /project/idc/bin/crtsurfdata /project/idc/ini/stcode.ini /tmp/idc/surfdata /log/idc/crtsurfdata.log csv,xml,json
catmainlog:
	cat /log/idc/crtsurfdata.log