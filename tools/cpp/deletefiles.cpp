#include "_public.h"
using namespace idc;

// 添加进程的心跳
cpactive pactive;

// 程序退出，信号2，15的处理函数
void EXIT(int sig);

int main(int argc, char *argv[]) {
  // 0-1 程序参数检查，及使用帮助
  if (argc != 4) {
    printf("\n");

    printf(
        "Using:/project/tools/bin/deletefiles pathname matchstr timeout\n\n");
    printf("Example:/project/tools/bin/deletefiles /tmp/idc/surfdata "
           "\"*.xml,*.json\" 0.01\n");
    cout
        << R"(Example2:/project/tools/bin/deletefiles /log/idc "*.log.20*" 0.01)"
        << endl;
    printf("Example3:/project/tools/bin/procctl 300 "
           "project/tools/bin/deletefiles /log/idc \"*.log20*\" 0.02\n\n");
    printf(
        "Example4:/project/tools/bin/procctl 300 project/tools/bin/deletefiles "
        "/tmp/idc/surfdata \"*.xml,*json\" 0.01\n\n");

    printf("这是一个工具程序，用来清理历史数据文件或日志文件。\n");
    printf("本程序会把pathname目录及其子目录中timeout天之前，且符合matchstr条件"
           "的文件删除掉，timeout可以是小数。\n");
    printf("本程序不会写日志，也不会在terminal上输出任何信息。\n\n\n");

    return -1;
  }

  // 0-2 处理程序退出：忽略全部信号，关闭IO, 设置信号处理函数
  closeioandsignal(true); // 调试时可以注释掉这行代码
  signal(SIGINT, EXIT);
  signal(SIGTERM, EXIT);

  // 将进程的信息加入到共享内存中
  pactive.addpinfo(30, "deletefiles"); // 每30秒更新一次心跳

  // 1-获取被定义为历史数据文件的时间点
  string strtimeout =
      ltime1("yyyymmddhh24miss", 0 - (int)(atof(argv[3]) * 24 * 60 * 60));
  // cout<<"strtimeout="<<strtimeout<<endl; //调试代码

  // 2-打开目录
  cdir dir;
  if (dir.opendir(argv[1], argv[2], 10000, true, false) == false) {
    printf("dir.opendir(%s) failed.\n", argv[1]);
    return -1;
  }

  // 3-遍历目录中的文件，删除符合条件的历史数据文件
  while (dir.readdir() == true) {
    if (dir.m_mtime < strtimeout) {
      if (remove(dir.m_ffilename.c_str()) == 0) {
        cout << "delete file " << dir.m_ffilename << " ok." << endl; // 调试代码
      } else {
        cout << "delete file " << dir.m_ffilename << " failed."
             << endl; // 调试代码
      }
    }
  }

  return 0;
}

void EXIT(int sig) {
  printf("程序退出，sig=%d\n\n", sig);

  exit(0);
}