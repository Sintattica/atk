CREATE TABLE db_sequence (
  seq_name varchar(100) NOT NULL default '',
  nextid int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (seq_name)
);

