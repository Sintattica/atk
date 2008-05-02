CREATE TABLE `user` (
/* obligatory */
  `user_id` int(11) NOT NULL DEFAULT 0,
  `username` varchar(25) NOT NULL DEFAULT '',
  `password` varchar(50) NOT NULL DEFAULT '',
  `firstname` varchar(50) NOT NULL DEFAULT '',
  `lastname` varchar(50) NOT NULL DEFAULT '',
  `email` varchar(50) NOT NULL DEFAULT '',
  `type_id` int(11) NOT NULL DEFAULT 0,
  `group_id` int(11) DEFAULT NULL,
/* non-obligatory */
  `title` varchar(25),
  `initials` varchar(10),
  `phonenumber` varchar(15),
  `cellular` varchar(15),
  `faxnumber` varchar(15),
  `jobtitle` varchar(50),
  `remark` varchar(100),
  `birthdate` date,
  `status_id` int(11) NOT NULL DEFAULT 0,
  `language_id` int(11),
/* mprint employee */
  `address` varchar(50),
  `zipcode` varchar(10),
  `city` varchar(50),
  `state` varchar(50),
  `country` varchar(50),
  `functionlevel_id` int(11),
  `department_id` int(11),
PRIMARY KEY (`user_id`),
KEY `username` (`username`)
);

CREATE TABLE `status` (
  `status_id` int(11) NOT NULL DEFAULT 0,
  `name` varchar(25) NOT NULL DEFAULT '',
  `description` varchar(100),
PRIMARY KEY (`status_id`),
KEY `name` (`name`)
);
INSERT INTO `status` VALUES (1, 'active', 'Active user account');
INSERT INTO `status` VALUES (2, 'inactive', 'Non-active user account');

CREATE TABLE `usertype` (
  `usertype_id` int(11) NOT NULL DEFAULT 0,
  `name` varchar(50) NOT NULL DEFAULT '',
PRIMARY KEY (`usertype_id`),
KEY `name` (`name`)
);
INSERT INTO `usertype` VALUES (1, 'Employee');
INSERT INTO `usertype` VALUES (2, 'Customer');
INSERT INTO `usertype` VALUES (3, 'Supplier');

CREATE TABLE `group` (
  `group_id` int(11) NOT NULL DEFAULT 0,
  `name` varchar(25) NOT NULL DEFAULT '',
  `description` varchar(100),
PRIMARY KEY (`group_id`),
KEY `name` (`name`)
);

CREATE TABLE `access` (
  `node` varchar(100) NOT NULL DEFAULT '',
  `action` varchar(25) NOT NULL DEFAULT '',
  `group_id` int(11) NOT NULL DEFAULT 0,
PRIMARY KEY (`node`,`action`,`group_id`)
);

CREATE TABLE `language` (
  `language_id` int(11) NOT NULL DEFAULT 0,
  `name` varchar(50) NOT NULL DEFAULT '',
  `code` varchar(2) NOT NULL DEFAULT '',
PRIMARY KEY (`language_id`),
KEY `name` (`name`)
);
INSERT INTO `language` VALUES (1, 'English', 'en');
INSERT INTO `language` VALUES (2, 'Nederlands', 'nl');


CREATE TABLE `functionlevel` (
  `functionlevel_id` int(11) NOT NULL DEFAULT 0,
  `name` varchar(50) NOT NULL DEFAULT '',
  `description` varchar(100),
PRIMARY KEY (`functionlevel_id`),
KEY `name` (`name`)
);
INSERT INTO `functionlevel` VALUES (1, 'Manager Purchase & Logistics', '');
INSERT INTO `functionlevel` VALUES (2, 'Purchaser', '');
INSERT INTO `functionlevel` VALUES (3, 'Logistics coordinator', '');
INSERT INTO `functionlevel` VALUES (4, 'Business Unit Manager', '');
INSERT INTO `functionlevel` VALUES (5, 'Project Manager (Account executive)', '');
INSERT INTO `functionlevel` VALUES (6, 'Sales manager', '');
INSERT INTO `functionlevel` VALUES (7, 'Account manager', '');
INSERT INTO `functionlevel` VALUES (8, 'Print manager (Monitoring web orders)', '');
INSERT INTO `functionlevel` VALUES (9, 'Manager Finance', '');
INSERT INTO `functionlevel` VALUES (10, 'Finance assistant', '');
INSERT INTO `functionlevel` VALUES (11, 'Office manager', '');
INSERT INTO `functionlevel` VALUES (12, 'Office assistant', '');
INSERT INTO `functionlevel` VALUES (13, 'Management', '');

CREATE TABLE `department` (
  `department_id` int(11) NOT NULL DEFAULT 0,
  `name` varchar(50) NOT NULL DEFAULT '',
  `description` varchar(100),
PRIMARY KEY (`department_id`),
KEY `name` (`name`)
);
INSERT INTO `department` VALUES (1, 'Purchase & Logistics', '');
INSERT INTO `department` VALUES (2, 'BU Anke', '');
INSERT INTO `department` VALUES (3, 'BU Thijs', '');
INSERT INTO `department` VALUES (4, 'BU ?', '');
INSERT INTO `department` VALUES (5, 'Sales', '');
INSERT INTO `department` VALUES (6, 'Finance', '');
INSERT INTO `department` VALUES (7, 'Office', '');
INSERT INTO `department` VALUES (8, 'Management', '');