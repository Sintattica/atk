CREATE TABLE db_sequence (
  seq_name varchar(100) NOT NULL default '',
  nextid int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (seq_name)
);

CREATE TABLE atk_searchcriteria 
(
  nodetype VARCHAR(100) NOT NULL,
  name VARCHAR(100) NOT NULL,
  criteria TEXT NOT NULL,
  handlertype VARCHAR(100) NOT NULL,
  PRIMARY KEY (nodetype, name)
);