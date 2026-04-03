# 此脚本用于启动数据共享平台的全部服务程序

# 启动守护模块，已经在/etc/rc.local中配置，以root启动。
#/project/tools/bin/procctl 10 /project/tools/bin/checkproc /tmp/log/checkproc.log

# 生成气象站点观测的分钟数据，每1分钟运行一次, 由调度模块启动。
/project/tools/bin/procctl 60 /project/idc/bin/crtsurfdata /project/idc/ini/stcode.ini /tmp/idc/surfdata /log/idc/crtsurfdata.log csv,xml,json

# 定期300s调用一次deletefiles来清理0.02天前生成的观测数据（/tmp/idc/surfdata）
/project/tools/bin/procctl 300 /project/tools/bin/deletefiles /tmp/idc/surfdata "*" 0.02

# 定期300s调用一次gzipfiles来压缩0.02天前生成的日志文件（/log/idc）
/project/tools/bin/procctl 300 /project/tools/bin/gzipfiles /log/idc "*.log.20*" 0.02