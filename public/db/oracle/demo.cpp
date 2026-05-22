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

  int minid = 10;
  int maxid = 11;

  stmt.prepare(
    "select id,name,weight,to_char(btime,'yyyy-mm-dd hh24:mi:ss'),memo from girls where id>=:1 and "
    "id<=:2"
  );

  stmt.bindin(1, minid);
  stmt.bindin(2, maxid);

  stmt.bindout(1, stgirl.id);
  stmt.bindout(2, stgirl.name, 30);
  stmt.bindout(3, stgirl.weight);
  stmt.bindout(4, stgirl.btime, 19);
  stmt.bindout(5, stgirl.memo, 300);

  if (stmt.execute() != 0) {
    printf("stmt.execute() failed.\n%s\n%s\n", stmt.sql(), stmt.message());
  }

  while (true) {
    memset(&stgirl, 0, sizeof(struct st_girl));
    if (stmt.next() != 0) {
      break;
    }
    printf(
      "id=%ld,name=%s,weight=%.02f,btime=%s,memo=%s\n",
      stgirl.id,
      stgirl.name,
      stgirl.weight,
      stgirl.btime,
      stgirl.memo
    );
  }

  printf("Excute SQL Success.\n");
  printf("查询了%ld条记录。\n", stmt.rpc());

  return 0;
}