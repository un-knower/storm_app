

-- ----------------------------
-- Table structure for day_method_waring
-- ----------------------------
DROP TABLE IF EXISTS `day_method_waring`;
CREATE TABLE `day_method_waring` (
  `id` smallint(8) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(255) NOT NULL COMMENT '具体每个步骤',
  `qa` varchar(255) NOT NULL DEFAULT '' COMMENT '01 名人名言 2 疲劳 3 绝望 4 害怕 5自责',
  `author` varchar(255) NOT NULL DEFAULT '' COMMENT '作者',
  `priority` mediumint(9) NOT NULL DEFAULT '0' COMMENT '数据大说明很重要',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;



select * from geyan;
DROP TABLE IF EXISTS geyan;
CREATE TABLE `geyan` (
  `id` smallint(8) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(255) NOT NULL COMMENT '具体每个步骤',
  `author` varchar(255) NOT NULL DEFAULT '' COMMENT '作者',
  `feel` varchar(255) NOT NULL DEFAULT '' COMMENT '01 名人名言 2 疲劳 3 绝望 4 害怕 5自责',
  `task` varchar(255) NOT NULL DEFAULT '' COMMENT '01学习 02 工作 03',
  `priority` mediumint(9) NOT NULL DEFAULT '0' COMMENT 'nice',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;