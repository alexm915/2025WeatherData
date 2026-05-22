# 此脚本用来停止数据共享平台全部的服务程序

# 先停止调度程序
killall -9 procctl


# 再停止其他服务程序
# 尝试让其他服务程序正常终止
killall crtsurfdata deletefiles gzipfiles ftpgetfiles ftpputfiles
killall fileserver tcpputfiles tcpgetfiles obtmindtodb
#让其他服务程序有足够多的时间退出
sleep 5

# 不管其他服务程序是否正常退出，全部强制终止
killall -9 crtsurfdata deletefiles gzipfiles ftpgetfiles ftpputfiles
killall -9 fileserver tcpputfiles tcpgetfiles obtmindtodb