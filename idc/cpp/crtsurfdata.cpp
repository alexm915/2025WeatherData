#include "_public.h"
using namespace idc;

clogfile logfile;
void EXIT(int sig);

// 气象站点参数的结构体
struct st_stcode {
  char provname[31];
  char obtid[11];
  char obtname[31];
  double lat;
  double lon;
  double height;
};

// 存放气象站点参数的容器
list<struct st_stcode> stlist;
// 将站点参数存放到stlist容器中的函数
bool loadstcode(const string& inifile);

// 观测数据的结构体
struct st_surfdata {
  char obtid[11];      // 站点代码
  char ddatetime[15];  // 数据时间：格式yyyymmddhh24miss，精确到分钟，秒固定填00
  double t;            // 气温，单位：0.1摄氏度
  double p;            // 气压，单位：0.1百帕
  double u;            // 相对湿度，单位：0-100之间的值
  double wd;           // 风向，单位：0-360之间的值
  double wf;           // 风速，单位：0.1m/s
  double r;            // 降水量，单位：0.1mm
  double vis;          // 能见度，单位：0.1米
};

char strddatetime[15];

// 存放观测数据的容器
list<struct st_surfdata> datalist;
// 生成观测数据的函数:
// 根据stlist容器中的站点参数，生成观测数据，并存放到datalist容器中
void crtsurfdata();

// 把datalist容器中的观测数据写入成csv,xml,json三种格式的文件的函数
bool crtsurffile(const string& outpath, const string& datafmt);

/*
  TODO: 1-程序参数检查，及使用帮助。
    1) 程序需要三个参数：站点参数文件stcode.ini, 生成数据的放置目录, 日志文件,
  输出数据文件的格式
*/
int main(int argc, char* argv[]) {
  if (argc != 5) {
    cout << "Using:./crtsurfdata inifile outpath logfile datafmt\n";
    cout << "Examples:/project/idc/bin/crtsurfdata /project/idc/ini/stcode.ini "
            "/tmp/idc/surfdata /log/idc/crtsurfdata.log csv,xml,json\n\n";

    cout << "inifile  气象站点参数文件名。\n";
    cout << "outpath  气象站点数据文件存放的目录。\n";
    cout << "logfile  本程序运行的日志文件名。\n";
    cout << "datafmt  生成数据文件的格式，csv,xml,json三种格式用逗号分隔。\n\n";

    return -1;
  }

  // TODO: 3-处理程序退出：忽略全部信号，关闭IO; 设置2,15的信号处理函数
  closeioandsignal(true);
  signal(SIGINT, EXIT);
  signal(SIGTERM, EXIT);

  // TODO: 2-程序运行日志记录
  if (logfile.open(argv[3]) == false) {
    cout << "logfile.open(" << argv[3] << ") failed.\n";
    return -1;
  }

  // 开始运行记录日志
  logfile.write("crtsurfdata 开始运行。\n");

  // 业务代码功能

  /*
    TODO: 4-读取气象站点参数文件stcode.ini，将参数放入容器中
        1)打开站点参数文件stcode.ini，读取每一行，拆分后存入到结构体变量st_stcode中，再将结构体变量放入到stlist容器中。拆分字符串用到ccmdstr类，具体用法看开发框架_public.h文件中的ccmdstr类的定义和实现。
        2)读取站点参数文件stcode.ini的第一行是标题，要注意跳过。
        3)读取站点参数文件stcode.ini的内容进行调试，确保正确读取和拆分了数据。
  */
  if (loadstcode(argv[1]) == false) {
    EXIT(-1);
  }

  /*
    TODO: 5- 根据站点参数stcode.ini文件生成观测数据(随机数)
        1)根据stlist容器中的站点参数，生成观测数据，并存放到datalist容器中。
         2)生成观测数据的函数crtsurfdata()的实现，具体代码在本文件的后面。
          2.1)设置随机数种子
          2.2)填充st_surfdata结构体变量
            获取观测时间strddatime，作为全局变量;
          2.3)将观测数据结构体变量放入到datalist容器中
         3)生成的观测数据进行调试，确保正确生成了数据。
  */
  // 获取观测数据的时间
  memset(strddatetime, 0, sizeof(strddatetime));
  ltime(strddatetime, "yyyymmddhh24miss");  // 获取当前时间
  strncpy(strddatetime + 12, "00", 2);      // 将秒固定填00
  // 生成观测数据
  crtsurfdata();

  /*
    TODO: 6- 将观测数据存入cvs, xml, json三种格式的文件中
  */
  // strstr()函数的作用是查找字符串argv[4]中是否包含"csv"、"xml"、"json"，如果包含，就调用crtsurffile()函数生成对应格式的文件。
  if (strstr(argv[4], "csv") != 0) crtsurffile(argv[2], "csv");
  if (strstr(argv[4], "xml") != 0) crtsurffile(argv[2], "xml");
  if (strstr(argv[4], "json") != 0) crtsurffile(argv[2], "json");

  // 结束运行记录日志
  logfile.write("crtsurfdata 运行结束。\n");

  return 0;
}

void EXIT(int sig) {
  logfile.write("程序退出,sig=%d\n\n", sig);
  // 此处\n\n，后一个\n是插入空白行, 使日志文件更清晰。
  exit(0);
}
bool loadstcode(const string& inifile) {
  cifile ifile;
  if (ifile.open(inifile) == false) {
    logfile.write("ifile.open(%s) failed.\n", inifile.c_str());
    return false;
  }

  // 1-读取stcode.ini的每一行
  // 2-将每一行数据拆分，存入到结构体变量中
  // 3-将结构体变量放入到stlist容器中
  string strbuffer;
  ifile.readline(strbuffer);  // stcode.ini标题不要扔掉

  ccmdstr cmdstr;    // 用于拆分字符串的类对象
  st_stcode stcode;  // 站点参数结构体

  // 读取行
  while (ifile.readline(strbuffer)) {
    // 调试：读取行是否成功
    // logfile.write("strbuffer=%s\n", strbuffer.c_str());

    // 拆分行
    cmdstr.splittocmd(strbuffer, ",");
    memset(&stcode, 0, sizeof(st_stcode));

    // 获取拆分后的字段内容，存入结构体变量中
    cmdstr.getvalue(0, stcode.provname, 30);
    cmdstr.getvalue(1, stcode.obtid, 10);
    cmdstr.getvalue(2, stcode.obtname, 30);
    cmdstr.getvalue(3, stcode.lat);
    cmdstr.getvalue(4, stcode.lon);
    cmdstr.getvalue(5, stcode.height);

    // 将结构体变量放入到stlist容器中
    stlist.push_back(stcode);
  }

  // 调试: 拆分数据是否成功
  // for (auto& aa : stlist) {
  //   logfile.write(
  //       "provname=%s,obtid=%s,obtname=%s,lat=%.2f,lon=%.2f,height=%.2f\n",
  //       aa.provname, aa.obtid, aa.obtname, aa.lat, aa.lon, aa.height);
  // }

  return true;
}

void crtsurfdata() {
  srand(time(0));  // 设置随机数种子
  st_surfdata stsurfdata;

  for (auto& stcode : stlist) {
    memset(&stsurfdata, 0, sizeof(st_surfdata));
    // 填充stsurfdata结构体
    strcpy(stsurfdata.obtid, stcode.obtid);
    strcpy(stsurfdata.ddatetime, strddatetime);
    stsurfdata.t = rand() % 350;               // 气温，单位：0.1摄氏度
    stsurfdata.p = rand() % 265 + 10000;       // 气压，单位：0.1百帕
    stsurfdata.u = rand() % 101;               // 相对湿度，单位：0-100之间的值
    stsurfdata.wd = rand() % 360;              // 风向，单位
    stsurfdata.wf = rand() % 150;              // 风速，单位：0.1m/s
    stsurfdata.r = rand() % 16;                // 降水量，
    stsurfdata.vis = rand() % 50001 + 100000;  // 能见度，单位：0.1米

    // 将将观测数据结构体stsurfdata放入到datalist容器中
    datalist.push_back(stsurfdata);
  }

  // 调试: 生成的观测数据是否成功
  // for (auto& aa : datalist) {
  //   logfile.write(
  //       "obtid=%s,ddatetime=%s,t=%.1f,p=%.1f,u=%.1f,wd=%.1f,wf=%.1f,r=%.1f,vis="
  //       "%.1f\n",
  //       aa.obtid, aa.ddatetime, aa.t / 10.0, aa.p / 10.0, aa.u, aa.wd,
  //       aa.wf / 10.0, aa.r / 10.0, aa.vis / 10.0);
  // }
}

bool crtsurffile(const string& outpath, const string& datafmt) {
  // 拼接文件名，如：/tmp/idc/surfdata/SURF_ZH_20260221092200_2254.csv
  string strfilename = outpath + "/SURF_ZH_" + strddatetime + "_" +
                       to_string(getpid()) + "." + datafmt;

  cofile ofile;  // 写入数据文件的对象
  if (ofile.open(strfilename) == false) {
    logfile.write("ofile.open(%s) failed.\n", strfilename.c_str());
    return false;
  }

  // 将datalist容器中的观测数据写入成csv,xml,json三种格式的文件
  if (datafmt == "csv")
    ofile.writeline(
        "站点代码,数据时间,气温,气压,相对湿度,风向,风速,降雨量,能见度\n");

  // 写入xml开始标签
  if (datafmt == "xml") ofile.writeline("<data>\n");
  if (datafmt == "json") ofile.writeline("{\"data\":[\n");

  for (auto& aa : datalist) {
    if (datafmt == "csv") {
      ofile.writeline("%s,%s,%.1f,%.1f,%d,%d,%.1f,%.1f,%.1f\n", aa.obtid,
                      aa.ddatetime, aa.t / 10.0, aa.p / 10.0, aa.u, aa.wd,
                      aa.wf / 10.0, aa.r / 10.0, aa.vis / 10.0);
    }
    if (datafmt == "xml") {
      ofile.writeline(
          "<obtid>%s</obtid><ddatetime>%s</ddatetime><t>%.1f</t><p>%.1f</"
          "p><u>%d</u><wd>%d</wd><wf>%.1f</wf><r>%.1f</r><vis>%.1f</vis><endl/"
          ">\n",
          aa.obtid, aa.ddatetime, aa.t / 10.0, aa.p / 10.0, aa.u, aa.wd,
          aa.wf / 10.0, aa.r / 10.0, aa.vis / 10.0);
    }
    if (datafmt == "json") {
      ofile.writeline(
          "{\"obtid\":\"%s\",\"ddatetime\":\"%s\",\"t\":%.1f,\"p\":%.1f,"
          "\"u\":%d,\"wd\":%d,\"wf\":%.1f,\"r\":%.1f,\"vis\":%.1f},\n",
          aa.obtid, aa.ddatetime, aa.t / 10.0, aa.p / 10.0, aa.u, aa.wd,
          aa.wf / 10.0, aa.r / 10.0, aa.vis / 10.0);

      // json格式的文件最后一个记录后面不能有逗号，所以在写入最后一个记录时，需要去掉逗号
      static int ii = 0;
      if (ii < datalist.size() - 1) {
        // 不是最后一个记录，写入逗号
        ofile.writeline(",\n");
        ii++;
      } else {
        ofile.writeline("\n");
      }
    }
  }

  // 写入xml结束标签
  if (datafmt == "xml") ofile.writeline("</data>\n");
  // 写入json结束标签
  if (datafmt == "json") ofile.writeline("]}\n");

  ofile.closeandrename();

  logfile.write("生成数据文件%s成功,数据时间%s,记录数%d.\n",
                strfilename.c_str(), strddatetime, datalist.size());

  return true;
}