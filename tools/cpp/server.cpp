#include <cstdio>
#include <stdlib.h>
#include <unistd.h>

int main() {
  while (true) {
    printf("服务程序正在运行中...\n");
    sleep(1);
  }
  return 0;
}