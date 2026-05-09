/**
 * @file ftpputfiles.cpp
 * @author Alex Mak (aliasgmail@duck.com)
 * @brief {ftp文件上传模块: 把本地文件上传到远程ftp服务端}
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
  char localpathbak[256];
  char okfilename[256]; // 已上传成功文件的信息，文件名+文件时间
  bool checkmtime;      // 是否需要检查服务端文件时间
  int timeout;          // 进程心跳超时时间
  char pname[51];       // 进程名，ftpputfiles_
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
map<string, string> mfromok;        // container1:上传过的文件,从okfilename加载
list<struct st_file_info> vfromdir; // container2:客户端目录中的文件,从本地目录扫描获得
list<struct st_file_info> vtook;    // container3:没更改过的文件
list<struct st_file_info> vupload;  // container4:本次需要上传


bool loadokfile();
bool load_local_file();
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
  // TODO2:连接FTP服务器, 加载starg.localpath目录下文件到vfromdir容器
  // 由ftp_client.login()和编写load_local_file()实现
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


  // starg.localpath目录下文件加载到vfromdir容器中
  if (load_local_file() == false) {
    log_file.write("load_local_file() failed.\n");
    return -1;
  }
  pactive.uptatime();

  //////////////////////////////////////////////////////////////////////////////////////////////
  // TODO3:上传方式, 哪些文件需要上传, 增量or全量？
  // type=1增量上传, 处理四个容器,编写loadokfile(), compmap()和writetookfile()实现
  // - loadokfile(): 加载okfilename到mfromok容器
  // - compmap():比较vfromdir,mfromok得到vtook和vupload
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
    vfromdir.swap(vupload);
  }
  // 可以更新心跳
  pactive.uptatime();


  //////////////////////////////////////////////////////////////////////////////////////////////
  // TODO4:上传文件, 遍历vupload容器使用ftp_client.put()上传
  //////////////////////////////////////////////////////////////////////////////////////////////

  string str_remote_file_name;
  string str_local_file_name;
  for (auto& i : vupload) {
    sformat(str_remote_file_name, "%s/%s", starg.remotepath, i.file_name_.c_str());
    sformat(str_local_file_name, "%s/%s", starg.localpath, i.file_name_.c_str());

    log_file.write("put %s ...", str_local_file_name.c_str());

    // 第三个参数true确保文件上传成功，对方不可抵赖
    if (ftp_client.put(str_local_file_name, str_remote_file_name, true) == false) {
      log_file << "failed.\n" << ftp_client.response() << "\n";
      return -1;
    }
    log_file.write("success.\n");

    // 上传成功后更新心跳
    pactive.uptatime();


    //////////////////////////////////////////////////////////////////////////////////////////////
    // TODO5:上传成功后，按照参数要求处理client本地目录上的文件
    //////////////////////////////////////////////////////////////////////////////////////////////

    // ptype=1 增量上传, 将上传成功的文件追加到okfilename文件中
    if (starg.ptype == 1) {
      appendtookfile(i);
    }
    // ptype=2 删除client本地文件, 使用remove()
    if (starg.ptype == 2) {
      if (remove(str_local_file_name.c_str()) != 0) {
        log_file.write("remove(%s) failed.\n", str_local_file_name.c_str());
        return -1;
      }
    }
    // ptype=3 client本地将相应文件移至备份
    if (starg.ptype == 3) {
      string str_local_file_name_bak = sformat("%s/%s", starg.localpathbak, i.file_name_.c_str());
      if (renamefile(str_local_file_name, str_local_file_name_bak) == false) {
        log_file.write(
          "renamefile(%s, %s) failed.\n",
          str_local_file_name.c_str(),
          str_local_file_name_bak.c_str()
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
  vupload.clear();

  for (auto& i : vfromdir) {
    auto it = mfromok.find(i.file_name_);
    if (it != mfromok.end()) {
      // 如果文件名存在，比较时间
      if (i.file_time_ == it->second) {
        vtook.push_back(i);
      } else {
        vupload.push_back(i);
      }
    } else {
      // 如果文件名不存在，说明是新文件，需要上传
      vupload.push_back(i);
    }
  }

  return true;
}


bool loadokfile() {
  mfromok.clear();
  cifile ifile;
  // 第一次上传starg.okfilename不存在，return true
  if (ifile.open(starg.okfilename) == false) {
    return true;
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


bool load_local_file() {
  vfromdir.clear();

  cdir dir;
  // 不包括子目录
  if (dir.opendir(starg.localpath, starg.matchname) == false) {
    log_file.write("dir.opendir(%s) failed.\n", starg.localpath);
    return false;
  }

  while (true) {
    if (dir.readdir() == false) {
      break;
    }
    vfromdir.emplace_back(dir.m_filename, dir.m_mtime);
  }

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
  // ptype=3, 将ftp文件备份到localpathbak
  if (starg.ptype == 3) {
    getxmlbuffer(strxmlbuffer, "localpathbak", starg.localpathbak, 255);
    if (strlen(starg.localpathbak) == 0) {
      log_file.write("localpathbak is null.\n");
      return false;
    }
  }
  // ptype=1, 增量上传, 将上传成功的文件追加到okfilename文件中, 检查文件时间checkmtime
  if (starg.ptype == 1) {
    getxmlbuffer(strxmlbuffer, "okfilename", starg.okfilename, 255);
    if (strlen(starg.okfilename) == 0) {
      log_file.write("okfilename is null.\n");
      return false;
    }
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
    "Sample:/project/tools/bin/procctl 30 /project/tools/bin/ftpputfiles "
    "/log/idc/ftpputfiles_surfdata.log "
    "\"<host>127.0.0.1:21</host><mode>1</mode>"
    "<username>ftp_remote</username><password>225166</password>"
    "<remotepath>/idcdata/ftp_server</remotepath><localpath>/idcdata/ftp_local</localpath>"
    "<matchname>*.json</matchname>"
    "<ptype>1</ptype><localpathbak>/tmp/idc/surfdatabak</localpathbak>"
    "<okfilename>/idcdata/ftplist/ftpputtfiles_test.xml</okfilename>"
    "<timeout>80</timeout><pname>ftpputfiles_test</pname>\"\n\n\n"
  );

  printf("本程序是通用的功能模块，用于把本地目录中的文件上传到远程的ftp服务器。\n");
  printf("logfilename是本程序运行的日志文件。\n");
  printf("xmlbuffer为文件上传的参数，如下：\n");
  printf("<host>127.0.0.1:21</host> 远程服务端的IP和端口。\n");
  printf("<mode>1</mode> 传输模式，1-被动模式，2-主动模式，缺省采用被动模式。\n");
  printf("<username>wucz</username> 远程服务端ftp的用户名。\n");
  printf("<password>wuczpwd</password> 远程服务端ftp的密码。\n");
  printf("<remotepath>/tmp/ftpputest</remotepath> 远程服务端存放文件的目录。\n");
  printf("<localpath>/tmp/idc/surfdata</localpath> 本地文件存放的目录。\n");
  printf(
    "<matchname>SURF_ZH*.JSON</matchname> 待上传文件匹配的规则。"
    "不匹配的文件不会被上传，本字段尽可能设置精确，不建议用*匹配全部的文件。\n"
  );
  printf(
    "<ptype>1</ptype> "
    "文件上传成功后，本地文件的处理方式：1-什么也不做；2-删除；3-"
    "备份，如果为3，还要指定备份的目录。\n"
  );
  printf(
    "<localpathbak>/tmp/idc/surfdatabak</localpathbak> "
    "文件上传成功后，本地文件的备份目录，此参数只有当ptype=3时才有效。\n"
  );
  printf(
    "<okfilename>/idcdata/ftplist/ftpputfiles_surfdata.xml</okfilename> "
    "已上传成功文件名清单，此参数只有当ptype=1时才有效。\n"
  );
  printf("<timeout>80</timeout> 上传文件超时时间，单位：秒，视文件大小和网络带宽而定。\n");
  printf(
    "<pname>ftpputfiles_surfdata</pname> "
    "进程名，尽可能采用易懂的、与其它进程不同的名称，方便故障排查。\n\n\n"
  );
}


void app_exit(const int sig) {
  printf("程序退出，sig=%d\n", sig);
  exit(0);
}