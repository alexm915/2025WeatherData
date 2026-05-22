#include "_public.h"
#include <csignal>
#include <cstring>
#include <sys/socket.h>
using namespace idc;

struct st_arg {
  int clienttype;       // 客户端类型，1-上传文件；2-下载文件，本程序固定填1。
  char clientpath[256]; // 本地文件存放的根目录。 /data /data/aaa /data/bbb
  int ptype;            // 文件上传成功后本地文件的处理方式：1-删除文件；2-移动到备份目录。
  bool andchild;        // 是否上传clientpath目录下各级子目录的文件，true-是；false-否。
  char matchname[256];  // 待上传文件名的匹配规则，如"*.TXT,*.XML"。
  char srvpath[256];    // 服务端文件存放的根目录。/data1 /data1/aaa /data1/bbb
  char srvpathbak[256]; // 服务端文件存放的根目录。/data1 /data1/aaa /data1/bbb
  int timetvl;          // 扫描本地目录文件的时间间隔（执行文件上传任务的时间间隔），单位：秒。
  int timeout;          // 进程心跳的超时时间。
  char pname[51];       // 进程名，建议用"tcpputfiles_后缀"的方式。
} starg;


cpactive pactive;
clogfile logfile;
ctcpserver tcpserver;
string str_sendbuf;
string str_recvbuf;

void parent_exit(int sig);
void child_exit(int sig);
bool active_test();

bool clientlogin();
bool ackmessage(const string& str_recvbuf);
// upload mode
void recvfilesmain();
bool recvfile(const string& filename, const string& mtime, const int filesize);
// download mode
void sendfilesmain();
bool _tcpputfiles(bool& becontinue);
bool sendfile(const string& filename, const int filesize);


int main(int argc, char* argv[]) {
  if (argc != 3) {
    printf("Using:./fileserver port logfile\n");
    printf("Example1:./fileserver 5005 /log/idc/fileserver.log\n");
    printf(
      "Example2:/project/tools/bin/procctl 10 /project/tools/bin/fileserver 5005 "
      "/log/idc/fileserver.log\n\n\n"
    );
    return -1;
  }

  // closeioandsignal(false);
  signal(SIGINT, parent_exit);
  signal(SIGTERM, parent_exit);
  if (logfile.open(argv[2]) == false) {
    printf("logfile.open(%s) failed.\n", argv[2]);
    return -1;
  }

  ////////////////////////////////////////////////////////////////////////////////////////
  // TODO: 1-init server
  ////////////////////////////////////////////////////////////////////////////////////////

  if (tcpserver.initserver(atoi(argv[1])) == false) {
    logfile.write("tcpserver.initserver(%s) failed.\n", argv[1]);
    return -1;
  }


  ////////////////////////////////////////////////////////////////////////////////////////
  // TODO: 2-主流程
  // 1. 父进程close client fd, 只用来处理连接请求;
  // 2. 子进程close listen fd, 只用来处理文件上传下载请求;
  ////////////////////////////////////////////////////////////////////////////////////////

  while (true) {
    if (tcpserver.accept() == false) {
      logfile.write("tcpserver.accept() failed.\n");
      parent_exit(-1);
    }
    logfile.write("client (%s) connected.\n", tcpserver.getip());

    // parent back to listen accept
    if (fork() > 0) {
      tcpserver.closeclient();
      continue;
    }
    // child handle main task
    signal(SIGINT, child_exit);
    signal(SIGTERM, child_exit);
    tcpserver.closelisten();


    ////////////////////////////////////////////////////////////////////////////////////////
    // TODO: 2.1-处理client的登录参数报文, 使用clientlogin()实现
    ////////////////////////////////////////////////////////////////////////////////////////
    if (clientlogin() == false) {
      child_exit(-1);
    }
    // 在子进程处添加心跳，每个子进程都有自己的心跳，放在登录后才能取到心跳参数
    pactive.addpinfo(starg.timeout, starg.pname);


    ////////////////////////////////////////////////////////////////////////////////////////
    // TODO: 2.2-根据clienttype判断是upload / download
    // 1. clienttype=1为upload mode, 用recvfilesmain()实现, recvfile()接受文件内容
    // 2. clienttype=2为download mode, 用sendfilesmain()实现, _tcpputfiles()执行一次下载
    ////////////////////////////////////////////////////////////////////////////////////////
    if (starg.clienttype == 1) {
      recvfilesmain();
    }
    if (starg.clienttype == 2) {
      sendfilesmain();
    }

    // 非1,2的非法连接，退出
    child_exit(0);
  }


  return 0;
}


bool sendfile(const string& filename, const int filesize) {
  int onread = 0;
  char buffer[1000];
  int totalbytes = 0;
  cifile ifile;

  if (ifile.open(filename, ios::in | ios::binary) == false) {
    return false;
  }

  while (true) {
    memset(buffer, 0, sizeof(buffer));

    if (filesize - totalbytes > 1000) {
      onread = 1000;
    } else {
      onread = filesize - totalbytes;
    }

    ifile.read(buffer, onread);
    if (tcpserver.write(buffer, onread) == false) {
      return false;
    }
    totalbytes += onread;

    if (totalbytes == filesize) {
      break;
    }
  }

  return true;
}


bool ackmessage(const string& str_recvbuf) {
  string filename;
  string result;
  getxmlbuffer(str_recvbuf, "filename", filename);
  getxmlbuffer(str_recvbuf, "result", result);

  // 直接return true, 下次重传
  if (result != "ok") {
    return true;
  }

  // ptype=1,删除文件；ptype=2,移动到备份目录
  if (starg.ptype == 1) {
    if (remove(filename.c_str()) != 0) {
      logfile.write("remove(%s) failed.\n", filename.c_str());
      return false;
    }
  }
  if (starg.ptype == 2) {
    // 生成备份文件全路径   /tmp/client/2.xml => /tmp/clientbak/2.xml
    string bakfilename = filename;
    replacestr(bakfilename, starg.clientpath, starg.srvpathbak, false);
    if (renamefile(filename, bakfilename) == false) {
      logfile.write("renamefile(%s, %s) failed.\n", filename.c_str(), bakfilename.c_str());
      return false;
    }
  }

  return true;
}


bool active_test() {
  str_sendbuf = "<activetest>ok</activetest>";

  if (tcpserver.write(str_sendbuf.c_str()) == false) {
    return false;
  }

  if (tcpserver.read(str_recvbuf, 10) == false) {
    return false;
  }

  return true;
}


bool _tcpputfiles(bool& becontinue) {
  becontinue = false;

  // 打开服务端目录(starg.srvpath), 找满足matchname的文件
  cdir dir;
  if (dir.opendir(starg.srvpath, starg.matchname, 10000, starg.andchild) == false) {
    logfile.write("dir.opendir(%s) failed.\n", starg.srvpath);
    return false;
  }

  int delay = 0;

  while (true) {
    if (dir.readdir() == false) {
      break;
    }
    becontinue = true;
    // 每个文件组装发送,文件描述信息报文：<filename><mtime><size>
    sformat(
      str_sendbuf,
      "<filename>%s</filename><mtime>%s</mtime><size>%d</size>",
      dir.m_ffilename.c_str(),
      dir.m_mtime.c_str(),
      dir.m_filesize
    );
    if (tcpserver.write(str_sendbuf.c_str()) == false) {
      logfile.write("tcpserver.write() failed.\n");
      return false;
    }
    // 调用sendfile()发送文件内容
    logfile.write("send %s(%d) ...", dir.m_ffilename.c_str(), dir.m_filesize);
    if (sendfile(dir.m_ffilename, dir.m_filesize) == true) {
      logfile.write("ok.\n");
      delay++;
    } else {
      logfile.write("failed.\n");
      tcpserver.closeclient();
      return false;
    }

    // 每上传一个文件，更新一次心跳
    pactive.uptatime();

    // 接收客户端确认报文
    while (delay > 0) {
      if (tcpserver.read(str_recvbuf, -1) == false) {
        break;
      }
      delay--;
      // 调用ackmessage()结果
      ackmessage(str_recvbuf);
    }
  }

  // 继续接受server response
  while (delay > 0) {
    if (tcpserver.read(str_recvbuf, 10) == false) {
      break;
    }

    delay--;
    ackmessage(str_recvbuf);
  }


  return true;
}


void sendfilesmain() {
  pactive.addpinfo(starg.timeout, starg.pname);

  // 如果调用_tcpputfiles()发送了文件，bcontinue为true，否则为false。
  bool bcontinue = true;

  while (true) {
    // 调用_tcpputfiles执行一次文件下载的任务。
    if (_tcpputfiles(bcontinue) == false) {
      logfile.write("_tcpputfiles() failed.\n");
      return;
    }
    // 本轮没有文件要发送，要：等待timetvl秒 + 发送心跳检测连接状态
    if (bcontinue == false) {
      sleep(starg.timetvl);
      if (active_test() == false)
        break;
    }

    pactive.uptatime();
  }
}


bool recvfile(const string& filename, const string& mtime, const int filesize) {
  int totalbytes = 0;
  int onread = 0;
  char buffer[1000];
  cofile ofile;

  // 打开目标文件 (二进制写模式)
  if (ofile.open(filename, ios::out | ios::binary) == false) {
    return false;
  }

  // 循环接收数据块（每次最多1000字节）
  while (true) {
    memset(buffer, 0, sizeof(buffer));
    if (filesize - totalbytes > 1000) {
      onread = 1000;
    } else {
      onread = filesize - totalbytes;
    }
    if (tcpserver.read(buffer, onread) == false) {
      return false;
    }
    // 写入文件
    ofile.write(buffer, onread);
    totalbytes += onread;
    // 验证文件大小完整后，关闭并重命名文件
    if (totalbytes == filesize) {
      break;
    }
  }

  ofile.closeandrename();
  // 与client文件时间保持一致
  setmtime(filename, mtime);

  return true;
}


bool clientlogin() {
  // 接收报文
  if (tcpserver.read(str_recvbuf, 10) == false) {
    logfile.write("tcpserver.read() failed.\n");
    return false;
  }
  // 解析参数报文到starg结构体
  memset(&starg, 0, sizeof(struct st_arg));
  // 用于upload, download的参数
  getxmlbuffer(str_recvbuf, "clienttype", starg.clienttype);
  getxmlbuffer(str_recvbuf, "clientpath", starg.clientpath);
  getxmlbuffer(str_recvbuf, "srvpath", starg.srvpath);
  getxmlbuffer(str_recvbuf, "srvpathbak", starg.srvpathbak);
  getxmlbuffer(str_recvbuf, "andchild", starg.andchild);
  getxmlbuffer(str_recvbuf, "ptype", starg.ptype);
  getxmlbuffer(str_recvbuf, "matchname", starg.matchname);
  // 用于心跳的参数
  getxmlbuffer(str_recvbuf, "timetvl", starg.timetvl);
  getxmlbuffer(str_recvbuf, "timeout", starg.timeout);
  getxmlbuffer(str_recvbuf, "pname", starg.pname);


  // 判断clienttype为1/2, 其他为非法请求
  if (starg.clienttype != 1 && starg.clienttype != 2) {
    str_sendbuf = "failed";
  } else {
    str_sendbuf = "ok";
  }

  // 回复确认报文
  if (tcpserver.write(str_sendbuf) == false) {
    logfile.write("tcpserver.write() failed.\n");
    return false;
  }

  logfile.write("%slogin %s\n%s\n", tcpserver.getip(), str_sendbuf.c_str(), str_recvbuf.c_str());


  return true;
}


void recvfilesmain() {
  while (true) {
    // 更新心跳
    pactive.uptatime();

    // 等待客户端报文（超时时间：timetvl+10秒）
    if (tcpserver.read(str_recvbuf, starg.timetvl + 10) == false) {
      logfile.write("tcpserver.read() failed.\n");
      return;
    }

    // 处理心跳报文："<activetest>ok</activetest>", 回复"ok"
    if (str_recvbuf == "<activetest>ok</activetest>") {
      str_sendbuf = "ok";

      if (tcpserver.write(str_sendbuf) == false) {
        logfile.write("tcpserver.write() failed.\n");
        return;
      }
    }

    // 处理文件上传请求报文：包含<filename><mtime><size>
    if (str_recvbuf.find("<filename>") != string::npos) {
      string clientfilename;
      string mtime;
      int filesize = 0;
      getxmlbuffer(str_recvbuf, "filename", clientfilename);
      getxmlbuffer(str_recvbuf, "mtime", mtime);
      getxmlbuffer(str_recvbuf, "size", filesize);
      // 路径转换: 将clientpath转成srvpath
      string serverfilename;
      serverfilename = clientfilename;
      replacestr(serverfilename, starg.clientpath, starg.srvpath, false);
      // 接收文件内容：使用revfile()实现, 并回应
      logfile.write("recv file %s(%d) ...", serverfilename.c_str(), filesize);
      if (recvfile(serverfilename, mtime, filesize) == true) {
        logfile.write("ok.\n");
        sformat(str_sendbuf, "<filename>%s</filename><result>ok</result>", clientfilename.c_str());
      } else {
        logfile.write("failed.\n");
        sformat(
          str_sendbuf, "<filename>%s</filename><result>failed</result>", clientfilename.c_str()
        );
      }
      // 返回成功/失败确认报文给客户端
      if (tcpserver.write(str_sendbuf) == false) {
        logfile.write("tcpserver.write() failed.\n");
        return;
      }
    }
  }
}

void parent_exit(int sig) {
  signal(SIGINT, SIG_IGN);
  signal(SIGTERM, SIG_IGN);

  logfile.write("parent process, sig=%d.\n", sig);
  kill(0, SIGTERM);
  tcpserver.closelisten();

  exit(0);
}

void child_exit(int sig) {
  signal(SIGINT, SIG_IGN);
  signal(SIGTERM, SIG_IGN);

  logfile.write("child process, sig=%d.\n", sig);
  tcpserver.closeclient();

  exit(0);
}