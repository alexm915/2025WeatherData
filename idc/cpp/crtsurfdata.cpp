#include "_public.h"
using namespace idc;

int main(int argc, char*argv[]){
    if(argc!=4){
        // 如果参数非法，给出帮助文档。
        cout << "Using:./crtsurfdata inifile outpath logfile\n";
        cout << "Example:/project/idc/bin/crtsurfdata /project/idc/ini/stcode.ini /tmp/idc/surfdata /log/idc/crtsurfdata.log\n";

        cout << "inifile  气象站点参数文件名。\n";
        cout << "outpath  气象站点数据文件存放的目录。\n";
        cout << "logfile  本程序运行的日志文件名。\n";

        return -1;
    }
    return 0;
}