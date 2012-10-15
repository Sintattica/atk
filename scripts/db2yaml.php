<?php
$config_atkroot = "../../";
include_once $config_atkroot.'./atk.inc';

$dir = @$_SERVER['argv'][1];
if ($dir == null) die("Please specify a directory name where you want to store the YAML files!\n");

$db = atkGetDb();
foreach ($db->table_names() as $table)
{
  $line = "Exporting table {$table['table_name']} to {$dir}/{$table['table_name']}.yml";
  echo $line;
  `php ./table2yaml.php {$table['table_name']} > {$dir}/{$table['table_name']}.yml`;
  echo str_repeat(' ', 90 + strlen($dir) - strlen($line))." [DONE]\n";
}