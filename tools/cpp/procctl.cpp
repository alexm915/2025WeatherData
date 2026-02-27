#include <cstdio>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

  // 程序使用提示
  if (argc < 3) {

    printf("Using:./procctl timetvl program argv ...\n");
    printf("Example:/project/tools/bin/procctl 10 /usr/bin/tar zcvf "
           "/tmp/tmp.tgz /usr/include\n");
    printf("Example:/project/tools/bin/procctl 60 /project/idc/bin/crtsurfdata "
           "/project/idc/ini/stcode.ini /tmp/idc/surfdata "
           "/log/idc/crtsurfdata.log csv,xml,json\n");

    printf("本程序是服务程序的调度程序，周期性启动服务程序或shell脚本。\n");
    printf("timetvl 运行周期，单位：秒。\n");
    printf(
        "        被调度的程序运行结束后，在timetvl秒后会被procctl重新启动。\n");
    printf("        如果被调度的程序是周期性的任务，timetvl设置为运行周期。\n");
    printf(
        "        如果被调度的程序是常驻内存的服务程序，timetvl设置小于5秒。\n");
    printf("program 被调度的程序名，必须使用全路径。\n");
    printf("...     被调度的程序的参数。\n");
    printf("注意，本程序不会被kill杀死，但可以用kill -9强行杀死。\n\n\n");

    return -1;
  }

  // 关闭信号和IO, 监控程序不被意外终止
  for (int ii = 0; ii < 64; ii++) {
    signal(ii, SIG_IGN);
    close(ii);
  }

  // 让监控调度程序后台运行: fork一个子进程，然后让父进程退出
  if (fork() != 0)
    exit(0);
  signal(SIGCHLD, SIG_DFL);

  // 为 exec 构造一套全新的、语义正确的参数数组
  char *pargv[argc];
  for (int ii = 2; ii < argc; ii++)
    pargv[ii - 2] = argv[ii];
  pargv[argc - 2] = nullptr;

  while (true) {
    if (fork() == 0) { // fork一个子进程来运行被调度的程序
      execv(argv[2], pargv);
      exit(0);
    } else { // 父进程等待子进程结束
      /*  关心子进程退出状态
          int status;
          wait(&status);
      */

      // 不关心子进程退出状态
      wait(nullptr);
      sleep(atoi(argv[1]));
    }
  }
  return 0;
}