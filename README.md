# 2025WeatherData
" 添加DEV分支，DEV+Main分支
> test for revert
## 0- 项目目录安排
    1. 主程序放在/project/idc/cpp (4个.cpp)
    2. 通用模块放到/projec/tools/cpp(20个.cpp)，如监控调度模块
## 1- 程序总体框架
    1. 主程序
    2. 监控调度
        - 调度模块procctl
        - 进程心跳(写进了项目框架_public.h中)
        - 守护模块checkproc
        三者运行逻辑：
            全部服务程序都有这3部分，
            其中调度模块负责启动服务，
            守护模块负责终止超时的服务，
            调度模块又将其重启。
        - 通过start.sh stop.sh脚本来启动/停止服务程序
        - 系统启动时就运行程序，/etc/rc.local加入以下内容:
            #以root启动守护模块
            /project/tools/bin/procctl 10 /project/tools/bin/checkproc /tmp/log/checkproc.log
            #启动数据共享平台的服务程序
            su - alex -c "/project/idc/cpp/start.sh
            #意思是切换到普通用户alex去执行此脚本
        两个工具
            - 清理历史数据文件deletefiles.cpp, 并在start.sh脚本中启用
            - 压缩历史数据文件
        
    3. 操作数据库开发
        数据库设计
    4. 

## 2- 程序的监控与调度
要解决的问题：
    1. 让监控程序周期性运行一些调度任务；
    2. 检查服务程序是否活着；
    3. 如果服务程序已经终止，重启它。
两种服务程序:
    1. 周期性运行的：如，每一分钟要运行一次，生成一次测试数据； ===> 周期性启动
    2. 常驻服务：如，网络通讯服务，不间断7x24运行 ===> 正常/异常终止后, 重启它。
## 3- 