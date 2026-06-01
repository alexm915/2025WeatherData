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
    "insert into girls(id,name,weight,btime,memo) values(:1,:2,:3,to_date(:4,'yyyy-mm-dd "
    "hh24:mi:ss'),:5)"
  );

  stmt.bindin(1, stgirl.id);
  stmt.bindin(2, stgirl.name, 30);
  stmt.bindin(3, stgirl.weight);
  stmt.bindin(4, stgirl.btime, 19);
  stmt.bindin(5, stgirl.memo, 300);

  for (int i = 10; i < 15; i++) {
    memset(&stgirl, 0, sizeof(struct st_girl));
    stgirl.id = i;
    sprintf(stgirl.name, "西施%05dgirl", i);
    stgirl.weight = 45.35 + i;
    sprintf(stgirl.btime, "2021-08-05 10:33:%02d", i);
    sprintf(stgirl.memo, "这是第%05d个超级女生的备注。", i);

    if (stmt.execute() != 0) {
      printf("stmt.execute() failed.\n%s\n%s\n", stmt.sql(), stmt.message());
    }
    printf("成功插入了%ld条记录。\n", stmt.rpc());
  }

  printf("Excute SQL Success.\n");
  conn.commit();

  return 0;
}