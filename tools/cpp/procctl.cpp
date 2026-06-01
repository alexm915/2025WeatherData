#include <cstdio>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char* argv[]) {

  // TODO: 1-程序使用提示
  if (argc < 3) {

    printf("Using:./procctl restart_time program argv ...\n");
    printf(
      "Example:/project/tools/bin/procctl 10 /usr/bin/tar zcvf "
      "/tmp/tmp.tgz /usr/include\n"
    );
    printf(
      "Example:/project/tools/bin/procctl 60 /project/idc/bin/crtsurfdata "
      "/project/idc/ini/stcode.ini /tmp/idc/surfdata "
      "/log/idc/crtsurfdata.log csv,xml,json\n"
    );

    printf("本程序是服务程序的调度程序，周期性启动服务程序或shell脚本。\n");
    printf("restart_time 运行周期，单位：秒。\n");
    printf(
      "    "
      "被调度的程序运行结束后，在restart_time秒后会被procctl重新启动。\n"
    );
    printf("    如果被调度的程序是周期性的任务，restart_time设置为运行周期。\n");
    printf(
      "    "
      "如果被调度的程序是常驻内存的服务程序，restart_time设置小于5秒。\n"
    );
    printf("program 被调度的程序名，必须使用全路径。\n");
    printf("    被调度的程序的参数。\n");
    printf("注意，本程序不会被kill杀死，但可以用kill -9强行杀死。\n\n\n");

    return -1;
  }

  // TODO: 2-关闭信号和IO, 调度程序不被意外终止
  /*
    (1) 忽略全部64个信号
    (2) 通过close函数关闭全部文件描述符，即关闭IO
  */
  for (int ii = 0; ii < 64; ii++) {
    signal(ii, SIG_IGN);
    close(ii);
  }

  // 让监控调度程序后台运行: fork一个子进程，然后让父进程正常退出exit(0),
  // 即：让程序变成守护进程Daemon, 再后台运行
  if (fork() != 0) {
    exit(0);
  }
  // 恢复子进程的SIGCHLD信号的默认行为，子子进程结束时子进程可以wait到它
  signal(SIGCHLD, SIG_DFL);

  // 为 execv 构造一套全新的、语义正确的参数数组
  // 程序参数: 本程序名 restart_time 调用的program argv ...
  char* pargv[argc];
  for (int ii = 2; ii < argc; ii++) {
    pargv[ii - 2] = argv[ii];
  }
  pargv[argc - 2] = nullptr;

  /*
    调度周期的逻辑:
    1. 父进程负责监工不断循环，fork一个子进程去execv启动目标程序;
    2. 父进程等待子进程结束后，sleep restart_time秒后继续循环。
    additional:
    1. execv运行成功不会返回任何值，失败返回-1;
    2. 子进程退出前会向父进程发送SIGCHLD信号;
    前面已经恢复的父进程的sigchld信号的默认行为;
    3. 父进程使用wait(int *status)等待子进程结束再退出，防止僵尸进程的产生。
  */
  while (true) {
    if (fork() == 0) { // fork一个子进程来运行被调度的程序
      execv(argv[2], pargv);
      exit(0);
    } else { // 父进程等待子进程结束,父进程会等待子进程运行完再退出
      int child_status;
      wait(&child_status); // 不关心子进程退出状态填nullptr
      sleep(atoi(argv[1]));
    }
  }
  return 0;
}