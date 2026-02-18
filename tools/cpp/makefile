#开发框架头文件路径
PUBLICL = -I/project/public
#方便调试，没有采用链接库,直接与开发框架源代码一起编译
PUBCPP = /project/public/_public.cpp

#编译选项
CFLAGS = -g
#CFLAGS = -O2


all:procctl deletefiles gzipfiles
procctl:procctl.cpp
	g++ -o procctl procctl.cpp
	cp -f procctl ../bin/.

deletefiles:deletefiles.cpp
	g++ $(CFLAGS) -o deletefiles deletefiles.cpp $(PUBLICL) $(PUBCPP)
	cp -f deletefiles ../bin/.

gzipfiles:gzipfiles.cpp
	g++ $(CFLAGS) -o gzipfiles gzipfiles.cpp $(PUBLICL) $(PUBCPP)
	cp -f gzipfiles ../bin/.

clean:
	rm -rf procctl deletefiles gzipfiles
