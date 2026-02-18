#include "_public.h"
using namespace idc;

//进程心跳结构体
struct st_proinfo
{
    int pid;              // 进程id
    char pname[51];       // 进程名称, 可以为空
    int timeout;          // 超时时间（秒）
    time_t atime;         // 最后一次心跳时间
};
