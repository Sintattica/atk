<?php
$config_atkroot = "../../";
include_once $config_atkroot.'./atk.inc';

atkimport('atk.utils.atkyaml');

$table = @$_SERVER['argv'][1];
if ($table == null) die("Please specify a table name!\n");

$table = strtolower($table);

$db = atkGetDb();
$db->query("SELECT * FROM $table");

ob_end_clean();

for ($i = 1; $db->next_record(); $i++)
{
  $record = $db->m_record;
  $yaml = atkYAML::dump(array("{$table}_{$i}" => $db->m_record));
  $yaml = substr($yaml, strpos($yaml, "\n") + 1);
  echo $yaml;
}