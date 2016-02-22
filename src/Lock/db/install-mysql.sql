# Use this installation script only for the atkDbLock type!

CREATE TABLE db_lock
(
  lock_id          BIGINT NOT NULL,
  lock_table       VARCHAR(100) NOT NULL,
  lock_record      VARCHAR(255) NOT NULL,
  lock_stamp       DATETIME NOT NULL,
  lock_lease       DATETIME NOT NULL,
  lock_lease_count BIGINT NOT NULL,
  user_id          VARCHAR(50) NOT NULL,
  user_ip          VARCHAR(50) NOT NULL,
  session_id       VARCHAR(32) NOT NULL,
  PRIMARY KEY (lock_id, lock_table, lock_record)
);