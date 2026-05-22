#include "_public.h"
using namespace idc;

struct st_arg {
  int clienttype;          // 客户端类型，1-上传文件；2-下载文件，本程序固定填1。
  char ip[31];             // 服务端的IP地址。
  int port;                // 服务端的端口。
  char clientpath[256];    // 本地文件存放的根目录。 /data /data/aaa /data/bbb
  int ptype;               // 文件上传成功后本地文件的处理方式：1-删除文件；2-移动到备份目录。
  char clientpathbak[256]; // 文件成功上传后，本地文件备份的根目录，当ptype==2时有效。
  bool andchild;           // 是否上传clientpath目录下各级子目录的文件，true-是；false-否。
  char matchname[256];     // 待上传文件名的匹配规则，如"*.TXT,*.XML"。
  char srvpath[256];       // 服务端文件存放的根目录。/data1 /data1/aaa /data1/bbb
  int timetvl;             // 扫描本地目录文件的时间间隔（执行文件上传任务的时间间隔），单位：秒。
  int timeout;             // 进程心跳的超时时间。
  char pname[51];          // 进程名，建议用"tcpputfiles_后缀"的方式。
} starg;

void app_exit(int sig);
void app_help();
bool _xmltoarg(const char* strxmlbuffer);

cpactive pactive;
clogfile logfile;
ctcpclient tcpclient;
string str_sendbuf;
string str_recvbuf;

bool login(const char* argv);
bool _tcpputfiles(bool& becontinue);
bool sendfile(const string& filename, const int filesize);
bool ackmessage(const string& str_recvbuf);
bool active_test();

int main(int argc, char* argv[]) {
  if (argc != 3) {
    app_help();
    return -1;
  }
  // closeioandsignal(false);
  signal(SIGINT, app_exit);
  signal(SIGTERM, app_exit);

  if (logfile.open(argv[1]) == false) {
    printf("logfile.open(%s) failed.\n", argv[1]);
    return -1;
  }
  if (_xmltoarg(argv[2]) == false) {
    logfile.write("_xmltoarg(%s) failed.\n", argv[2]);
    return -1;
  }


  // 添加心跳
  pactive.addpinfo(starg.timeout, starg.pname);


  // send connect request to server
  if (tcpclient.connect(starg.ip, starg.port) == false) {
    logfile.write("tcpclient.connect(%s, %d) failed.\n", starg.ip, starg.port);
    app_exit(-1);
  }
  // TODO:向服务端发送登录报文，把客户端程序的参数传递给服务端。
  if (login(argv[2]) == false) {
    logfile.write("login() failed.\n");
    app_exit(-1);
  }

  // 如果调用了_tcpputfiles()发送了文件按, becontinue为true, 否则为false
  // 初始化为true
  bool becontinue = true;


  // process main loop
  while (true) {

    if (_tcpputfiles(becontinue) == false) {
      logfile.write("_tcpputfiles() failed.\n");
      app_exit(-1);
    }

    // 优化:如果刚才执行文件上传任务的时候上传了文件，在上传的过程中，可能有新的文件陆续已生成，
    // 那么，为保证文件被尽快上传，进程不体眠。
    // （只有在刚才执行文件上传任务的时候没有上传文件的情况下才休眠）
    if (becontinue == false) {
      sleep(starg.timetvl);

      // send heartbeat message
      if (active_test() == false) {
        break;
      }
    }

    // 更新心跳
    pactive.uptatime();
  }


  app_exit(0);
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
    if (tcpclient.write(buffer, onread) == false) {
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
    replacestr(bakfilename, starg.clientpath, starg.clientpathbak, false);
    if (renamefile(filename, bakfilename) == false) {
      logfile.write("renamefile(%s, %s) failed.\n", filename.c_str(), bakfilename.c_str());
      return false;
    }
  }

  return true;
}


// 文件上传main func, 执行文件一次上传的任务
bool _tcpputfiles(bool& becontinue) {
  becontinue = false;

  cdir dir;
  // open clientpath directory
  if (dir.opendir(starg.clientpath, starg.matchname, 10000, starg.andchild) == false) {
    logfile.write("dir.opendir(%s) failed.\n", starg.clientpath);
    return false;
  }

  // 未收到server确认报文的数量，发送一个文件+1,收到一个回应-1
  int delay = 0;

  // read all files
  while (dir.readdir()) {
    becontinue = true;

    // TODO:send file info to server
    sformat(
      str_sendbuf,
      "<filename>%s</filename><mtime>%s</mtime><size>%d</size>",
      dir.m_ffilename.c_str(),
      dir.m_mtime.c_str(),
      dir.m_filesize
    );
    // xxxxxxxxx logfile.write("send: %s\n", str_sendbuf.c_str());
    if (tcpclient.write(str_sendbuf.c_str()) == false) {
      logfile.write("tcpclient.write() failed.\n");
      return false;
    }

    // TODO:send file content, with sendfile() function
    logfile.write("send %s(%d) ...", dir.m_ffilename.c_str(), dir.m_filesize);
    if (sendfile(dir.m_ffilename, dir.m_filesize) == true) {
      logfile.write("ok.\n");
      delay++;
    } else {
      logfile.write("failed.\n");
      tcpclient.close();
      return false;
    }


    // 每上传一个文件，更新一次心跳
    pactive.uptatime();


    // recv server response
    while (delay > 0) {
      if (tcpclient.read(str_recvbuf, -1) == false) {
        break;
      }
      // xxxxxxxxx logfile.write("recv: %s\n", str_recvbuf.c_str());

      // TODO: 根据服务端的回应处理本地文件,删除文件或移动文件到备份目录。
      ackmessage(str_recvbuf);
    }
  }

  // 继续接受server response
  while (delay > 0) {
    if (tcpclient.read(str_recvbuf, 10) == false) {
      break;
    }
    // xxxxxxxxx logfile.write("recv: %s\n", str_recvbuf.c_str());

    delay--;
    ackmessage(str_recvbuf);
  }

  return true;
}


// 以tcp报文的形式将client的参数传给server
bool login(const char* argv) {
  sformat(str_sendbuf, "%s<clienttype>1</clienttype>", argv);
  // xxxxxxxxxx logfile.write("send: %s\n", str_sendbuf.c_str());
  if (tcpclient.write(str_sendbuf.c_str()) == false) {
    return false;
  }

  if (tcpclient.read(str_recvbuf, 10) == false) {
    return false;
  }
  // xxxxxxxxxx logfile.write("recv: %s\n", str_recvbuf.c_str());

  logfile.write("login(%s:%d) success.\n", starg.ip, starg.port);
  return true;
}

bool active_test() {
  str_sendbuf = "<activetest>ok</activetest>";
  // xxxxxxxxxxxx logfile.write("send: %s\n", str_sendbuf.c_str());
  if (tcpclient.write(str_sendbuf.c_str()) == false) {
    return false;
  }
  // xxxxxxxxxxxx logfile.write("send: %s\n", str_sendbuf.c_str());

  if (tcpclient.read(str_recvbuf, 10) == false) {
    return false;
  }
  logfile.write("recv: %s\n", str_recvbuf.c_str());
  // 心跳机制的代码可简单化处理，只需要收到对端的回应,检测到网络可用就行，不必判断回应的内容。

  return true;
}


bool _xmltoarg(const char* strxmlbuffer) {
  memset(&starg, 0, sizeof(starg));

  getxmlbuffer(strxmlbuffer, "ip", starg.ip);
  if (strlen(starg.ip) == 0) {
    logfile.write("ip is null.\n");
    return false;
  }

  getxmlbuffer(strxmlbuffer, "port", starg.port);
  if (starg.port == 0) {
    logfile.write("port is null.\n");
    return false;
  }

  getxmlbuffer(strxmlbuffer, "ptype", starg.ptype);
  if ((starg.ptype != 1) && (starg.ptype != 2)) {
    logfile.write("ptype not in (1,2).\n");
    return false;
  }

  getxmlbuffer(strxmlbuffer, "clientpath", starg.clientpath);
  if (strlen(starg.clientpath) == 0) {
    logfile.write("clientpath is null.\n");
    return false;
  }

  getxmlbuffer(strxmlbuffer, "clientpathbak", starg.clientpathbak);
  if ((starg.ptype == 2) && (strlen(starg.clientpathbak) == 0)) {
    logfile.write("clientpathbak is null.\n");
    return false;
  }

  getxmlbuffer(strxmlbuffer, "andchild", starg.andchild);

  getxmlbuffer(strxmlbuffer, "matchname", starg.matchname);
  if (strlen(starg.matchname) == 0) {
    logfile.write("matchname is null.\n");
    return false;
  }

  getxmlbuffer(strxmlbuffer, "srvpath", starg.srvpath);
  if (strlen(starg.srvpath) == 0) {
    logfile.write("srvpath is null.\n");
    return false;
  }

  getxmlbuffer(strxmlbuffer, "timetvl", starg.timetvl);
  if (starg.timetvl == 0) {
    logfile.write("timetvl is null.\n");
    return false;
  }
  // 扫描本地目录文件的时间间隔（执行上传任务的时间间隔），单位：秒。
  // starg.timetvl没有必要超过30秒。
  if (starg.timetvl > 30) {
    starg.timetvl = 30;
  }

  // 进程心跳的超时时间，一定要大于starg.timetvl。
  getxmlbuffer(strxmlbuffer, "timeout", starg.timeout);
  if (starg.timeout == 0) {
    logfile.write("timeout is null.\n");
    return false;
  }
  if (starg.timeout <= starg.timetvl) {
    logfile.write("starg.timeout(%d) <= starg.timetvl(%d).\n", starg.timeout, starg.timetvl);
    return false;
  }

  getxmlbuffer(strxmlbuffer, "pname", starg.pname, 50);
  // if (strlen(starg.pname)==0) { logfile.write("pname is null.\n"); return false; }

  return true;
}


void app_help() {
  printf("\n");
  printf("Using:/project/tools/bin/tcpputfiles logfilename xmlbuffer\n\n");

  printf(
    "Sample:/project/tools/bin/procctl 20 /project/tools/bin/tcpputfiles "
    "/log/idc/tcpputfiles_surfdata.log "
    "\"<ip>192.168.150.128</ip><port>5005</port>"
    "<clientpath>/tmp/client</clientpath><ptype>1</ptype>"
    "<srvpath>/tmp/server</srvpath>"
    "<andchild>true</andchild><matchname>*.xml,*.txt</matchname><timetvl>10</timetvl>"
    "<timeout>50</timeout><pname>tcpputfiles_surfdata</pname>\"\n\n"
  );

  printf("本程序是数据中心的公共功能模块，采用tcp协议把文件上传给服务端。\n");
  printf("logfilename   本程序运行的日志文件。\n");
  printf("xmlbuffer     本程序运行的参数，如下：\n");
  printf("ip            服务端的IP地址。\n");
  printf("port          服务端的端口。\n");
  printf("ptype         文件上传成功后的处理方式：1-删除文件；2-移动到备份目录。\n");
  printf("clientpath    本地文件存放的根目录。\n");
  printf("clientpathbak 文件成功上传后，本地文件备份的根目录，当ptype==2时有效。\n");
  printf(
    "andchild      是否上传clientpath目录下各级子目录的文件，true-是；false-否，缺省为false。\n"
  );
  printf("matchname     待上传文件名的匹配规则，如\"*.TXT,*.XML\"\n");
  printf("srvpath       服务端文件存放的根目录。\n");
  printf("timetvl       扫描本地目录文件的时间间隔，单位：秒，取值在1-30之间。\n");
  printf("timeout       本程序的超时时间，单位：秒，视文件大小和网络带宽而定，建议设置50以上。\n");
  printf("pname         进程名，尽可能采用易懂的、与其它进程不同的名称，方便故障排查。\n\n");
}


void app_exit(int sig) {
  logfile.write("程序退出，sig=%d\n", sig);
  exit(0);
}