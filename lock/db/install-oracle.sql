-- Use this installation script only for the atkDbLock type!

CREATE TABLE db_lock
(
  lock_id          NUMBER(10) NOT NULL,
  lock_table       VARCHAR2(100) NOT NULL,
  lock_record      VARCHAR2(255) NOT NULL,
  lock_stamp       DATE NOT NULL,
  lock_lease       DATE NOT NULL,
  lock_lease_count NUMBER(10) NOT NULL,
  user_id          VARCHAR2(50) NOT NULL,
  user_ip          VARCHAR2(50) NOT NULL,
  session_id       VARCHAR2(32) NOT NULL,
  CONSTRAINT pk_dblock PRIMARY KEY (lock_id, lock_table, lock_record)
);