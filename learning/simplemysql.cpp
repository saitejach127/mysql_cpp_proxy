#include <stdlib.h>
#include <iostream>
#include <mysql_connection.h>
#include <driver.h>
#include <exception.h>
#include <resultset.h>
#include <statement.h>

using namespace sql;
int main(void){
  sql::Driver *driver;
  sql::Connection *con;

  driver = get_driver_instance();
  con = driver->connect("tcp://127.0.0.1:3306","root","ML#tensorflow@122");

  return 0;
}