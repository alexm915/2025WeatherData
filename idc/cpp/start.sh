# 此脚本用于启动数据共享平台的全部服务程序
# 启动守护模块，已经在/etc/rc.local中配置，以root启动。
#/project/tools/bin/procctl 10 /project/tools/bin/checkproc /tmp/log/checkproc.log


##############################################################################
##########       crtsurfdata程序
##############################################################################
# 生成气象站点观测的分钟数据，每1分钟运行一次, 由调度模块启动。
/project/tools/bin/procctl 60 /project/idc/bin/crtsurfdata /project/idc/ini/stcode.ini /tmp/idc/surfdata /log/idc/crtsurfdata.log csv,xml,json
# 定期300s调用一次deletefiles来清理0.02天前生成的观测数据（/tmp/idc/surfdata）
/project/tools/bin/procctl 300 /project/tools/bin/deletefiles /tmp/idc/surfdata "*" 0.02
# 定期300s调用一次gzipfiles来压缩0.02天前生成的日志文件（/log/idc）
/project/tools/bin/procctl 300 /project/tools/bin/gzipfiles /log/idc "*.log.20*" 0.02


##############################################################################
##########       ftp文件传输
##############################################################################
# 从/tmp/idc/surfdata目录下载原始的气象观测数据文件，存放在/idcdata/surfdata目录。
/project/tools/bin/procctl 30 /project/tools/bin/ftpgetfiles /log/idc/ftpgetfiles_surfdata.log "<host>127.0.0.1:21</host><mode>1</mode><username>ftp_remote</username><password>225166</password><localpath>/idcdata/surfdata</localpath><remotepath>/tmp/idc/surfdata</remotepath><matchname>SURF_ZH*.XML,SURF_ZH*.CSV</matchname><listfilename>/idcdata/ftplist/ftpgetfiles_surfdata.list</listfilename><ptype>1</ptype><okfilename>/idcdata/ftplist/ftpgetfiles_surfdata.xml</okfilename><checkmtime>true</checkmtime><timeout>80</timeout><pname>ftpgetfiles_surfdata</pname>"
# 清理/idcdata/surfdata目录中0.04天之前的文件。
/project/tools/bin/procctl 300 /project/tools/bin/deletefiles /idcdata/surfdata "*" 0.04

# 把/tmp/idc/surfdata目录的原始气象观测数据文件上传到/tmp/ftpputest目录。
# 注意，先创建好服务端的目录：mkdir /tmp/ftpputest 
/project/tools/bin/procctl 30 /project/tools/bin/ftpputfiles /log/idc/ftpputfiles_surfdata.log "<host>127.0.0.1:21</host><mode>1</mode><username>ftp_remote</username><password>225166</password><localpath>/tmp/idc/surfdata</localpath><remotepath>/tmp/ftpputest</remotepath><matchname>SURF_ZH*.JSON</matchname><ptype>1</ptype><okfilename>/idcdata/ftplist/ftpputfiles_surfdata.xml</okfilename><timeout>80</timeout><pname>ftpputfiles_surfdata</pname>"
# 清理/tmp/ftpputest目录中0.04天之前的文件。
/project/tools/bin/procctl 300 /project/tools/bin/deletefiles /tmp/ftpputest "*" 0.04


##############################################################################
##########       tcp文件传输
##############################################################################
# 文件传输的服务端程序。
/project/tools/bin/procctl 10 /project/tools/bin/fileserver 5005 /log/idc/fileserver.log

# 把目录/tmp/ftpputest中的文件上传到/tmp/tcpputest目录中。
/project/tools/bin/procctl 20 /project/tools/bin/tcpputfiles /log/idc/tcpputfiles_surfdata.log "<ip>127.0.0.1</ip><port>5005</port><ptype>1</ptype><clientpath>/tmp/ftpputest</clientpath><andchild>true</andchild><matchname>*.XML,*.CSV,*.JSON</matchname><srvpath>/tmp/tcpputest</srvpath><timetvl>10</timetvl><timeout>50</timeout><pname>tcpputfiles_surfdata</pname>"
# 把目录/tmp/tcpputest中的文件下载到/tmp/tcpgetest目录中。
/project/tools/bin/procctl 20 /project/tools/bin/tcpgetfiles /log/idc/tcpgetfiles_surfdata.log "<ip>127.0.0.1</ip><port>5005</port><ptype>1</ptype><srvpath>/tmp/tcpputest</srvpath><andchild>true</andchild><matchname>*.XML,*.CSV,*.JSON</matchname><clientpath>/tmp/tcpgetest</clientpath><timetvl>10</timetvl><timeout>50</timeout><pname>tcpgetfiles_surfdata</pname>"

# 清理/tmp/tcpgetest目录中的历史数据文件。
/project/tools/bin/procctl 300 /project/tools/bin/deletefiles /tmp/tcpgetest "*" 0.02