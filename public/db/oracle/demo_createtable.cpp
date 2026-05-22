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

  sqlstatement stmt;
  stmt.connect(&conn);
  stmt.prepare(
    "create table girls(id number(10),name varchar2(30),weight number(8,2),btime date,memo "
    "varchar2(300),pic blob,primary key(id))"
  );

  if (stmt.execute() != 0) {
    printf("stmt.execute() failed.\n%s\n%s\n", stmt.sql(), stmt.message());
    return -1;
  }

  printf("create table ok.\n");

  return 0;
}