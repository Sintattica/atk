-- 
-- Table structure for table `bc_business_cards`
-- 

CREATE TABLE `bc_business_cards` (
  `id` int(15) NOT NULL,
  `name` varchar(255) collate utf8_unicode_ci NOT NULL,
  `business_card_pdf_path` varchar(255) collate utf8_unicode_ci NOT NULL,
  `company_id` int(15) NOT NULL,
  `obs_template_code` varchar(255) collate utf8_unicode_ci default NULL,
  PRIMARY KEY  (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- 
-- Dumping data for table `bc_business_cards`
-- 

INSERT INTO `bc_business_cards` (`id`, `name`, `business_card_pdf_path`, `company_id`, `obs_template_code`) VALUES 
(114, '534wetrtwe', 'ibs_template3.pdf', 3, NULL);

-- --------------------------------------------------------

-- 
-- Table structure for table `bc_fields`
-- 

CREATE TABLE `bc_fields` (
  `id` int(15) NOT NULL,
  `business_card_id` int(15) NOT NULL,
  `page_nr` int(3) NOT NULL,
  `field_nr` int(3) NOT NULL,
  `field_blockname` varchar(255) collate utf8_unicode_ci NOT NULL,
  `field_multiline` tinyint(1) NOT NULL default '0',
  `field_desc` varchar(255) collate utf8_unicode_ci default NULL,
  `obligatory` tinyint(1) NOT NULL default '0',
  `additional_information` varchar(255) collate utf8_unicode_ci default NULL,
  PRIMARY KEY  (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
