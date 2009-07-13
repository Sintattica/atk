<?php

class test_sql_whereclause_blacklist_checker extends atkTestCase
{
  private function isValid($whereclause)
  {
    $validator = new sql_whereclause_blacklist_checker($whereclause);
    return $validator->isSafe();
  }

  public function test_filter_valid()
  {
    $where_clauses = array(
      "News.id='1'",
      "contract.id='16'",
      "project_person.projectid='1017' AND project_person.personid='366' AND project_person.role='1'",
      "(x.x=1) OR (y.y=2)",
      "(z.z='50' OR yada.yada='text') AND hours.time='70'",
      "hours.time=' -- '",
      "hours.time=' /* */ '",
    );

    foreach ($where_clauses as $clause)
    {
      $this->assertTrue($this->isValid($clause), "Checking wether $clause is a valid WHERE clause");
    }
  }

  public function test_filter_invalid()
  {
    $where_clauses = array(
      "1/*",
      "1 --",
      "1#",
      "1)UNION SELECT user.password FROM user",
      "1)UNION(SELECT",
      "1; DROP TABLE hours; --",
      "'\"' --",
      "'\\'' --",
      "\"''hello''\" #",
    );

    foreach ($where_clauses as $clause)
    {
      $this->assertIdentical($this->isValid($clause), false, "Checking wether $clause is not a valid WHERE clause");
    }
  }
}