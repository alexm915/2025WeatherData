#include "_ooci.h"
using namespace idc;

int main(int argc, char* argv[]) {
  connection conn;
  if (
    conn.connecttodb(
      "alex/Alex225166@//192.168.145.128:1521/XEPDB1", "Simplified Chinese_China.AL32UTF8"
    ) != 0
  ) {
    printf("connect database failed.\n%s\n", conn.message());
    return -1;
  }
  printf("connect database ok.\n");

  sqlstatement stmt(&conn);


  struct st_girl {
    long id;
    char name[31];
    double weight;
    char btime[20];
    char memo[301];
  } stgirl;

  int minid = 12;
  int maxid = 14;

  stmt.prepare("delete from girls where id>=:1 and id<=:2");

  stmt.bindin(1, minid);
  stmt.bindin(2, maxid);


  if (stmt.execute() != 0) {
    printf("stmt.execute() failed.\n%s\n%s\n", stmt.sql(), stmt.message());
  }

  printf("Excute SQL Success.\n");
  conn.commit();

  return 0;
}