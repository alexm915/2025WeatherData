#include "_public.h"
using namespace idc;

clogfile logfile;  // 日志文件对象。

void EXIT(int sig);

int main(int argc, char* argv[]) {
  if (argc != 4) {
    // 0-1 程序参数检查，及使用帮助。
    cout << "Using:./crtsurfdata inifile outpath logfile\n";
    cout << "Example:/project/idc/bin/crtsurfdata /project/idc/ini/stcode.ini "
            "/tmp/idc/surfdata /log/idc/crtsurfdata.log\n";

    cout << "inifile  气象站点参数文件名。\n";
    cout << "outpath  气象站点数据文件存放的目录。\n";
    cout << "logfile  本程序运行的日志文件名。\n";

    return -1;
  }

  // 0-2 处理程序退出：忽略全部信号，关闭IO, 设置信号处理函数
  closeioandsignal(true);
  signal(SIGINT, EXIT);
  signal(SIGTERM, EXIT);

  // 0-3 程序运行日志记录
  if (logfile.open(argv[3]) == false) {
    cout << "logfile.open(" << argv[3] << ") failed.\n";
    return -1;
  }

  logfile.write("crtsurfdata 开始运行。\n");

  // 业务代码功能

  // 1-读取气象站点参数文件stcode.ini，得到站点参数。
  /*
      1)读取参数文件中的每一行成string,再将string拆分得到各个参数数据。
      2）
  */

  // 2- 模拟生成全国气象站点的观测数据。

  // 3- 把生成的观测数据写入到指定目录的文件中。
  /*
      1)将容器datalist中的观测数据写入成csv,xml,json三种格式的文件；
  */

  sleep(100);  // 模拟程序运行过程

  logfile.write("crtsurfdata 运行结束。\n");

  return 0;
}

void EXIT(int sig) {
  logfile.write("程序退出,sig=%d\n\n", sig);
  exit(0);
}