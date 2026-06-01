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

  stmt.prepare(
    "update girls set name=:1, weight=:2, btime=to_date(:3,'yyyy-mm-dd hh24:mi:ss') where id=:4"
  );

  stmt.bindin(1, stgirl.name, 30);
  stmt.bindin(2, stgirl.weight);
  stmt.bindin(3, stgirl.btime, 19);
  stmt.bindin(4, stgirl.id);

  memset(&stgirl, 0, sizeof(struct st_girl));
  stgirl.id = 10;
  stgirl.weight = 299;
  // sprintf(stgirl.name, "Bob");
  strcpy(stgirl.name, "Sumyu");
  strcpy(stgirl.btime, "2039-10-31 10:22:54");

  if (stmt.execute() != 0) {
    printf("stmt.execute() failed.\n%s\n%s\n", stmt.sql(), stmt.message());
  }

  printf("Excute SQL Success.\n");
  conn.commit();

  return 0;
}