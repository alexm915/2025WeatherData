#include "_ftp.h"
#include "_public.h"
using namespace idc;

cftpclient ftp_client;
clogfile log_file;

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

} starg;

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

vector<struct st_file_info> vfilelist;
bool load_list_file();

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

  // TODO: 1-从服务器中的目录下载文件，可指定文件名匹配规则
  // TODO: 1.1-打开日志文件，解析xml文件得到程序运行参数
  if (log_file.open(argv[1]) == false) {
    printf("打开日志文件失败，程序退出。\n");
    return -1;
  }
  if (xml_to_arg(argv[2]) == false) {
    printf("解析xml参数失败，程序退出。\n");
    return -1;
  }

  // TODO: 1.2-连接ftp服务器，进入指定目录,ftp_client.nlist()列出文件名并保存到本地文件中，然后将这些文件名放到vfilelist容器中
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

  // 将获取到的文件名加载到vfilelist容器中
  if (load_list_file() == false) {
    log_file.write("load_list_file() failed.\n");
    return -1;
  }

  // TODO: 1.3-遍历容器，使用ftp_client.get()函数下载文件
  string str_remote_file_name;
  string str_local_file_name;

  for (auto& i : vfilelist) {
    // 拼接server/local全路径的文件名
    sformat(str_remote_file_name, "%s/%s", starg.remotepath, i.file_name_.c_str());
    // sformat(str_remote_file_name, "%s", i.file_name_.c_str());
    sformat(str_local_file_name, "%s/%s", starg.localpath, i.file_name_.c_str());

    log_file.write("Getting %s...\n", str_remote_file_name.c_str());

    if (ftp_client.get(str_remote_file_name, str_local_file_name) == false) {
      log_file << "ftp_client.get files failed.\n" << ftp_client.response() << "\n";
      return -1;
    }

    // << 追加日志
    log_file << "ftp_client.get files success.\n";

    // TODO:2 -下载成功后，按照参数要求处理ftp服务器上的文件
    // ptype=1 增量下载
    if (starg.ptype == 1) {
    }

    // ptype=2 删除ftp服务端文件, ptype=3 ftp服务端相应文件移至备份
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

bool load_list_file() {
  vfilelist.clear();

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
    vfilelist.emplace_back(str_file_name, "");
  }

  ifile.closeandremove();

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

  // 下载后服务端文件的处理方式：1-什么也不做；2-删除；3-备份。
  getxmlbuffer(strxmlbuffer, "ptype", starg.ptype);
  if ((starg.ptype != 1) && (starg.ptype != 2) && (starg.ptype != 3)) {
    log_file.write("ptype is error.\n");
    return false;
  }

  // 下载后服务端文件的备份目录。
  if (starg.ptype == 3) {
    getxmlbuffer(strxmlbuffer, "remotepathbak", starg.remotepathbak, 255);
    if (strlen(starg.remotepathbak) == 0) {
      log_file.write("remotepathbak is null.\n");
      return false;
    }
  }

  return true;
}

void app_help() {
  printf("\n");
  printf("Using: this_program logifle_name xml_buffer\n\n");
  printf(
    "Example:/project/tools/bin/procctl 30"
    "/project/tools/bin/ftpgetfiles "
    "/log/idc/ftpgetfiles_surfdata.log "
    "\"<host>127.0.0.1</host><mode>1</mode>"
    "<username>ftp_remote</username><password>225166</password>"
    "<remotepath>idcdata/surfdata</remotepath>"
    "<localpath>/tmp/idc/surfdata</localpath>"
    "<matchname>SURF_ZH*.XML,SURF_ZH*.CSV</matchname>"
    "<ptype>3</ptype><remotepathbak>/tmp/idc/surfdatabak</remotepathbak>\"\n\n"
  );
  printf("本程序是通用的功能模块，用于把远程ftp服务端的文件下载到本地目录。\n");
  printf("logfilename是本程序运行的日志文件。\n");
  printf("xmlbuffer为文件下载的参数，如下：\n");
  printf("<host>192.168.150.128:21</host> 远程服务端的IP和端口。\n");
  printf("<mode>1</mode> 传输模式，1-被动模式，2-主动模式，缺省采用被动模式。\n");
  printf("<username>wucz</username> 远程服务端ftp的用户名。\n");
  printf("<password>oraccle</password> 远程服务端ftp的密码。\n");
  printf(
    "<remotepath>/tmp/idc/surfdata</remotepath> "
    "远程服务端存放文件的目录。\n"
  );
  printf("<localpath>/idcdata/surfdata</localpath> 本地文件存放的目录。\n");
  printf(
    "<matchname>SURF_ZH*.XML,SURF_ZH*.CSV</matchname> 待下载文件匹配的规则。"
    "不匹配的文件不会被下载，本字段尽可能设置精确，不建议用*"
    "匹配全部的文件。\n\n\n"
  );
  printf(
    "<ptype>1</ptype> 文件下载成功后，远程服务端文件的处理方式："
    "1-什么也不做；2-删除；3-备份，如果为3，还要指定备份的目录。\n"
  );
  printf(
    "<remotepathbak>/tmp/idc/surfdatabak</remotepathbak> "
    "文件下载成功后，服务端文件的备份目录，"
    "此参数只有当ptype=3时才有效。\n\n\n"
  );
}

void app_exit(const int sig) {
  printf("程序退出，sig=%d\n", sig);
  exit(0);
}