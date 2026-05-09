/**
 * @file ftpgetfiles.cpp
 * @author Alex Mak (aliasgmail@duck.com)
 * @brief {ftp文件传输模块: 把远程ftp服务端的文件下载到本地目录}
 * @version 0.1
 * @date 2026-05-10
 *
 * @copyright Copyright (c) 2026
 *
 */

#include "_ftp.h"
#include "_public.h"
using namespace idc;

cftpclient ftp_client;
clogfile log_file;
cpactive pactive;

// TODO:数据结构1: 参数结构体
struct st_arg {
  char host[31];
  int mode;
  char username[31];
  char password[31];
  char remotepath[256];
  char localpath[256];
  char matchname[256];
  int ptype;
  char remotepathbak[256];
  char okfilename[256]; // 已下载成功文件的信息，文件名+文件时间
  bool checkmtime;      // 是否需要检查服务端文件时间
  int timeout;          // 进程心跳超时时间
  char pname[51];       // 进程名，ftpgetfiles_
} starg;

// TODO:数据结构2: 文件结构体
struct st_file_info {
  string file_name_;
  string file_time_;
  st_file_info() = default;
  st_file_info(const string& file_name, const string& file_time)
      : file_name_(file_name), file_time_(file_time) {
  }
  void clear() {
    file_name_.clear();
    file_time_.clear();
  }
};

// TODO:数据结构3: 四个容器
// 1需要搜索用二叉树map, 234需要遍历用list
map<string, string> mfromok;          // container1:下载过的文件,从okfilename加载
list<struct st_file_info> vfromnlist; // container2:现在ftp上最新的文件列表,从.nlist文件加载
list<struct st_file_info> vtook;      // container3:没更改过的文件
list<struct st_file_info> vdownload;  // container4:本次需要下载


bool loadokfile();
bool load_list_file();
bool compmap();
bool writetookfile();
bool appendtookfile(const struct st_file_info& stfileinfo);

void app_exit(const int sig);
void app_help();
bool xml_to_arg(const char* strxmlbuffer);


int main(int argc, char* argv[]) {
  if (argc != 3) {
    app_help();
    return -1;
  }
  // closeioandsignal(true);
  signal(SIGINT, app_exit);
  signal(SIGTERM, app_exit);
  // 打开日志, 准备写程序日志
  if (log_file.open(argv[1]) == false) {
    printf("打开日志文件失败，程序退出。\n");
    return -1;
  }


  //////////////////////////////////////////////////////////////////////////////////////////////
  // TODO1:解析XML参数到starg结构体中, 编写xml_to_arg()实现
  //////////////////////////////////////////////////////////////////////////////////////////////
  if (xml_to_arg(argv[2]) == false) {
    printf("解析xml参数失败，程序退出。\n");
    return -1;
  }
  // 写入进程心跳
  pactive.addpinfo(starg.timeout, starg.pname);


  //////////////////////////////////////////////////////////////////////////////////////////////
  // TODO2:连接FTP进入指定目录，nlist()列出远程文件,保存到vfromlist容器中
  // 由ftp_client.login(), ftp_client.chdir(), ftp_client.nlist()和编写load_list_file()实现
  //////////////////////////////////////////////////////////////////////////////////////////////

  // 登入ftp服务器
  if (ftp_client.login(starg.host, starg.username, starg.password, starg.mode) == false) {
    log_file.write(
      "ftp_client.login(%s, %s, %s) failed.\n%s\n",
      starg.host,
      starg.username,
      starg.password,
      ftp_client.response()
    );
    return -1;
  }
  log_file.write(
    "ftp_client.login(%s, %s, %s) success.\n", starg.host, starg.username, starg.password
  );

  // 进入ftp服务器目录,nlist列出文件名并保存到本地文件中
  if (ftp_client.chdir(starg.remotepath) == false) {
    log_file.write("ftp_client.chdir(%s) failed.\n%s\n", starg.remotepath, ftp_client.response());
    return -1;
  }
  if (ftp_client.nlist(".", sformat("/tmp/nlist/ftpgetfiles_%d.nlist", getpid())) == false) {
    log_file.write("ftp_client.nlist(%s) failed.\n%s\n", starg.remotepath, ftp_client.response());
    return -1;
  }
  log_file.write(
    "ftp_client.nlist(%s) success.\n", sformat("/tmp/nlist/ftpgetfiles_%d.nlist", getpid()).c_str()
  );
  // nlist后update心跳
  pactive.uptatime();

  // 将获取到的文件名加载到vfromnlist容器中
  if (load_list_file() == false) {
    log_file.write("load_list_file() failed.\n");
    return -1;
  }


  //////////////////////////////////////////////////////////////////////////////////////////////
  // TODO3:下载方式, 哪些文件需要下载, 增量or全量？
  // type=1增量下载, 处理四个容器,编写loadokfile(), compmap()和writetookfile()实现
  // - loadokfile(): 加载okfilename到mfromok容器
  // - compmap():比较vfromnlist,mfromok得到vtook和vdownload
  // - writetookfile():将vtook写入okfilename文件中,覆盖原来的旧内容
  // 增加两个参数
  // - starg.okfilename[]文件内容格式: <filename>文件名</filename><mtime>文件时间</mtime>
  // - starg.checktime 检查文件时间
  //////////////////////////////////////////////////////////////////////////////////////////////

  if (starg.ptype == 1) {
    loadokfile();
    compmap();
    writetookfile();
  } else {
    vfromnlist.swap(vdownload);
  }
  // 可以更新心跳
  pactive.uptatime();


  //////////////////////////////////////////////////////////////////////////////////////////////
  // TODO4:下载文件, 遍历vdownload容器使用ftp_client.get()下载
  //////////////////////////////////////////////////////////////////////////////////////////////
  string str_remote_file_name;
  string str_local_file_name;

  for (auto& i : vdownload) {
    // 拼接server/local全路径的文件名
    sformat(str_remote_file_name, "%s/%s", starg.remotepath, i.file_name_.c_str());
    // sformat(str_remote_file_name, "%s", i.file_name_.c_str());
    sformat(str_local_file_name, "%s/%s", starg.localpath, i.file_name_.c_str());
    log_file.write("Getting %s...\n", str_remote_file_name.c_str());

    if (ftp_client.get(str_remote_file_name, str_local_file_name, starg.checkmtime) == false) {
      log_file << "ftp_client.get files failed.\n" << ftp_client.response() << "\n";
      return -1;
    }
    // << 追加日志
    log_file << "ftp_client.get files success.\n";
    // 下载文件之后更新心跳
    pactive.uptatime();


    //////////////////////////////////////////////////////////////////////////////////////////////
    // TODO5:下载成功后，按照参数要求处理ftp服务器上的文件
    //////////////////////////////////////////////////////////////////////////////////////////////

    // ptype=1 增量下载, 将下载成功的文件追加到okfilename文件中
    if (starg.ptype == 1) {
      appendtookfile(i);
    }

    // ptype=2 删除ftp服务端文件, 使用ftp_client.ftpdelete() method
    if (starg.ptype == 2) {
      if (ftp_client.ftpdelete(str_remote_file_name) == false) {
        log_file.write(
          "ftp_client.ftpdelete(%s) failed.\n%s\n",
          str_remote_file_name.c_str(),
          ftp_client.response()
        );
        return -1;
      }
    }

    // ptype=3 ftp服务端相应文件移至备份
    if (starg.ptype == 3) {
      string str_remote_file_name_bak = sformat("%s/%s", starg.remotepathbak, i.file_name_.c_str());
      if (ftp_client.ftprename(str_remote_file_name, str_remote_file_name_bak) == false) {
        log_file.write(
          "ftp_client.ftprename(%s, %s) failed.\n%s\n",
          str_remote_file_name.c_str(),
          str_remote_file_name_bak.c_str(),
          ftp_client.response()
        );
        return -1;
      }
    }
  }

  return 0;
}


bool appendtookfile(const struct st_file_info& stfileinfo) {
  cofile ofile;

  // 第二个参数为false不生成临时文件
  if (ofile.open(starg.okfilename, false, ios::app) == false) {
    log_file.write("ofile.open(%s) failed.\n", starg.okfilename);
    return false;
  }
  ofile.writeline(
    "<filename>%s</filename><mtime>%s</mtime>\n",
    stfileinfo.file_name_.c_str(),
    stfileinfo.file_time_.c_str()
  );

  return true;
}


bool writetookfile() {
  cofile ofile;
  if (ofile.open(starg.okfilename) == false) {
    log_file.write("ofile.open(%s) failed.\n", starg.okfilename);
    return false;
  }

  for (auto& i : vtook) {
    ofile.writeline(
      "<filename>%s</filename><mtime>%s</mtime>\n", i.file_name_.c_str(), i.file_time_.c_str()
    );
  }
  ofile.closeandrename();

  return true;
}


bool compmap() {
  vtook.clear();
  vdownload.clear();

  for (auto& i : vfromnlist) {
    auto it = mfromok.find(i.file_name_);
    if (it != mfromok.end()) {
      // 如果文件名存在，比较时间
      if (starg.checkmtime == true) {
        if (i.file_time_ == it->second) {
          vtook.push_back(i);
        } else {
          vdownload.push_back(i);
        }
      } else {
        vtook.push_back(i);
      }
    } else {
      // 如果文件名不存在，说明是新文件，需要下载
      vdownload.push_back(i);
    }
  }

  return true;
}


bool loadokfile() {
  if (starg.ptype != 1) {
    return true;
  }

  mfromok.clear();
  cifile ifile;
  if (ifile.open(starg.okfilename) == false) {
    return true;
    // 如果okfilename文件不存在，说明没有下载成功的文件，此时mfromok容器就是空的，程序继续执行即可。
  }

  string str_buffer;
  struct st_file_info stfileinfo;
  while (true) {
    stfileinfo.clear();

    if (ifile.readline(str_buffer) == false) {
      break;
    }

    getxmlbuffer(str_buffer, "filename", stfileinfo.file_name_);
    getxmlbuffer(str_buffer, "mtime", stfileinfo.file_time_);

    mfromok[stfileinfo.file_name_] = stfileinfo.file_time_;
  }

  return true;
}


/**
 * @brief {将最新的.nlist临时文件清单根据matchname规则挑选放入vfromnlist容器中}
 * @return true
 * @return false
 * @author Alexr Mak (aliasgmail@duck.com)
 * @date 2026-05-09
 */
bool load_list_file() {
  vfromnlist.clear();

  cifile ifile;
  if (ifile.open(sformat("/tmp/nlist/ftpgetfiles_%d.nlist", getpid())) == false) {
    log_file.write(
      "ifile.open(%s) failed.\n", sformat("/tmp/nlist/ftpgetfiles_%d.nlist", getpid()).c_str()
    );
    return false;
  }

  string str_file_name;

  while (true) {
    if (ifile.readline(str_file_name) == false) {
      break;
    }
    if (matchstr(str_file_name, starg.matchname) == false) {
      continue;
    }
    if (starg.ptype == 1 && starg.checkmtime == true) {
      // ftp.mtime()到ftp服务器上获取文件的时间，存储在ftp_client.m_mtime成员变量中
      if (ftp_client.mtime(str_file_name) == false) {
        log_file.write(
          "ftp_client.mtime(%s) failed.\n%s\n", str_file_name.c_str(), ftp_client.response()
        );
        return false;
      }
    }
    // 原地构造struct st_file_info对象(看结构体定义)，并添加到vfromnlist容器中
    vfromnlist.emplace_back(str_file_name, ftp_client.m_mtime);
  }

  // 关闭并删除.nlist临时文件
  ifile.closeandremove();

  // for (auto& aa : vfromnlist) {
  //   log_file.write("filename=%s,mtime=%s\n", aa.file_name_.c_str(), aa.file_time_.c_str());
  // }
  return true;
}


bool xml_to_arg(const char* strxmlbuffer) {
  memset(&starg, 0, sizeof(st_arg));

  getxmlbuffer(strxmlbuffer, "host", starg.host, 30);
  if (strlen(starg.host) == 0) {
    log_file.write("host is null.\n");
    return false;
  }

  getxmlbuffer(strxmlbuffer, "mode", starg.mode);
  if (starg.mode != 2) {
    starg.mode = 1;
  }

  getxmlbuffer(strxmlbuffer, "username", starg.username, 30);
  if (strlen(starg.username) == 0) {
    log_file.write("username is null.\n");
    return false;
  }

  getxmlbuffer(strxmlbuffer, "password", starg.password, 30);
  if (strlen(starg.password) == 0) {
    log_file.write("password is null.\n");
    return false;
  }

  getxmlbuffer(strxmlbuffer, "remotepath", starg.remotepath, 255);
  if (strlen(starg.remotepath) == 0) {
    log_file.write("remotepath is null.\n");
    return false;
  }

  getxmlbuffer(strxmlbuffer, "localpath", starg.localpath, 255);
  if (strlen(starg.localpath) == 0) {
    log_file.write("localpath is null.\n");
    return false;
  }

  getxmlbuffer(strxmlbuffer, "matchname", starg.matchname, 255);
  if (strlen(starg.matchname) == 0) {
    log_file.write("matchname is null.\n");
    return false;
  }


  // 根据ptype解析相应的参数
  getxmlbuffer(strxmlbuffer, "ptype", starg.ptype);
  if ((starg.ptype != 1) && (starg.ptype != 2) && (starg.ptype != 3)) {
    log_file.write("ptype is error.\n");
    return false;
  }
  // ptype=3, 将ftp文件备份到remotepathbak
  if (starg.ptype == 3) {
    getxmlbuffer(strxmlbuffer, "remotepathbak", starg.remotepathbak, 255);
    if (strlen(starg.remotepathbak) == 0) {
      log_file.write("remotepathbak is null.\n");
      return false;
    }
  }
  // ptype=1, 增量下载, 将下载成功的文件追加到okfilename文件中, 检查文件时间checkmtime
  if (starg.ptype == 1) {
    getxmlbuffer(strxmlbuffer, "okfilename", starg.okfilename, 255);
    if (strlen(starg.okfilename) == 0) {
      log_file.write("okfilename is null.\n");
      return false;
    }

    getxmlbuffer(strxmlbuffer, "checkmtime", starg.checkmtime);
    // memset(starg)会将starg.checktime初始化为0,因此缺省值为false; true要检查xml参数标签
  }


  // 进程心跳超时时间与进程名
  getxmlbuffer(strxmlbuffer, "timeout", starg.timeout);
  if (starg.timeout == 0) {
    log_file.write("timeout is null.\n");
    return false;
  }
  getxmlbuffer(strxmlbuffer, "pname", starg.pname, 50);
  // 心跳机制中的进程名可选
  // if (strlen(starg.pname) == 0) {
  //   log_file.write("pname is null.\n");
  //   return false;
  // }

  return true;
}


void app_help() {
  printf("\n");
  printf("Using: this_program logifle_name xml_buffer\n\n");
  printf(
    "Sample:/project/tools/bin/procctl 30 /project/tools/bin/ftpgetfiles "
    "/log/idc/ftpgetfiles_surfdata.log "
    "\"<host>127.0.0.1:21</host><mode>1</mode>"
    "<username>ftp_remote</username><password>225166</password>"
    "<remotepath>/idcdata/ftp_server</remotepath><localpath>/idcdata/ftp_local</localpath>"
    "<matchname>*.json</matchname>"
    "<ptype>1</ptype><localpathbak>/tmp/idc/surfdatabak</localpathbak>"
    "<okfilename>/idcdata/ftplist/ftpgetfiles_test.xml</okfilename>"
    "<checkmtime>true</checkmtime>"
    "<timeout>30</timeout><pname>ftpgetfiles_test</pname>\"\n\n\n"
  );

  printf("本程序是通用的功能模块，用于把远程ftp服务端的文件下载到本地目录。\n");
  printf("logfile_name是本程序运行的日志文件。\n");
  printf("xml_buffer为文件下载的参数，如下：\n");
  printf("<host>192.168.150.128:21</host> 远程服务端的IP和端口。\n");
  printf("<mode>1</mode> 传输模式，1-被动模式，2-主动模式，缺省采用被动模式。\n");
  printf("<username>alex</username> 远程服务端ftp的用户名。\n");
  printf("<password>225166</password> 远程服务端ftp的密码。\n");
  printf("<remotepath>/idcdata/ftp_server</remotepath> 远程服务端存放文件的目录。\n");
  printf("<localpath>/idcdata/ftp_local</localpath> 本地文件存放的目录。\n");
  printf(
    "<matchname>SURF_ZH*.XML,SURF_ZH*.CSV</matchname> 待下载文件匹配的规则。"
    "不匹配的文件不会被下载，本字段尽可能设置精确，不建议用*匹配全部的文件。\n"
  );
  printf(
    "<ptype>1</ptype> 文件下载成功后，远程服务端文件的处理方式："
    "1-什么也不做；2-删除；3-备份，如果为3，还要指定备份的目录。\n"
  );
  printf(
    "<remotepathbak>/tmp/idc/surfdatabak</remotepathbak> 文件下载成功后，服务端文件的备份目录，"
    "此参数只有当ptype=3时才有效。\n"
  );
  printf(
    "<okfilename>/idcdata/ftplist/ftpgetfiles_test.xml</okfilename> 已下载成功文件名清单，"
    "此参数只有当ptype=1时才有效。\n"
  );
  printf(
    "<checkmtime>true</checkmtime> 是否需要检查服务端文件的时间，true-需要，false-不需要，"
    "此参数只有当ptype=1时才有效，缺省为false。\n"
  );
  printf("<timeout>30</timeout> 下载文件超时时间，单位：秒，视文件大小和网络带宽而定。\n");
  printf(
    "<pname>ftpgetfiles_test</pname> "
    "进程名，尽可能采用易懂的、与其它进程不同的名称，方便故障排查。\n\n\n"
  );
}


void app_exit(const int sig) {
  printf("程序退出，sig=%d\n", sig);
  exit(0);
}