<?php

/**
 * @internal Setup the system
 */
  $config_atkroot = "./";
  include_once("atk.inc");
  atksession();

  $time = getNode("funding.time");
  $output = &atkOutput::getInstance();

  global $g_db, $selectedprogram, $empnumber, $g_debug_msg;
  $empnumber = escapeSQL($empnumber);
  if ($selectedprogram) $organization = getOrganizationByProgram($selectedprogram);
  set_time_limit(0);

  $query = "SELECT time.id,time.date,time.employee,time.endtime,time.begintime FROM time";
  $where =" WHERE ";
  if ($organization)
  {
    $query.=" LEFT JOIN employee_project_role epr ON time.employee = epr.id
                LEFT JOIN employee ON employee.id = epr.employee";
    $where.= "employee.organization = '$organization'";
  }
  if ($empnumber) $where.= " AND employee.id = '$empnumber' ";
  $results = $g_db->getrows($query.($where==" WHERE "?"":$where));

  $securebridge = array();
  $costsupdated=0;
  $resultcount = count($results);

  foreach ($results as $key=>$record)
  {
    echo "Bezig met tijdregistratie $key van $resultcount \n";
    $timeid = $record['id'];
    $time->transformRecordHoursToArray($record);

    $totalHours = $record['endtime']['hours'] - $record['begintime']['hours'] - ($record['begintime']['minutes'] / 60)
    + ($record['endtime']['minutes'] / 60);

    // beware for nightshifts:
    if ($totalHours < 0)
    $totalHours += 24;

    $g_db->query("UPDATE " . $time->m_table . " SET hours='" . $totalHours . "' WHERE id='" . $timeid . "';");

    $wage = $time->getPersonWage($record['employee'], $record['date']);

    if (!$wage) $wage = 0;
    else $costsupdated++;

    $costs = $totalHours * $wage;
    $g_db->query("UPDATE " . $time->m_table . " SET costs='" . $costs . "' WHERE id='" . $timeid . "';");
  }
  $time->clearCache();
  atkdebug("<h1>records updated with actual costs (not 0): $updatedcosts</h1>");
  $output->outputFlush();
  mail('boy@ibuildings.nl','Update kosten en uren gedaan!',implode("\n",$g_debug_msg));

?>