#ifndef IDCAPP_H
#define IDCAPP_H

#include "_ooci.h"
#include "_public.h"
using namespace idc;

class CZHOBTMIND {
private:
  struct st_zhobtmind {
    char obtid[6];      // 站点代码。
    char ddatetime[21]; // 数据时间，精确到分钟。
    char t[11];         // 温度，单位：0.1摄氏度。
    char p[11];         // 气压，单位：0.1百帕。
    char u[11];         // 相对湿度，0-100之间的值。
    char wd[11];        // 风向，0-360之间的值。
    char wf[11];        // 风速：单位0.1m/s。
    char r[11];         // 降雨量：0.1mm。
    char vis[11];       // 能见度：0.1米。
  };
  clogfile& m_logfile;
  connection& m_conn;
  sqlstatement m_stmt;

  string m_buffer;
  struct st_zhobtmind m_stzhobtmind;

public:
  CZHOBTMIND(connection& conn, clogfile& logfile) : m_conn(conn), m_logfile(logfile) {
  }
  ~CZHOBTMIND() {
  }

  bool splitbuffer(const string& strbuffer, const bool bisxml);
  bool inserttable();
};


#endif