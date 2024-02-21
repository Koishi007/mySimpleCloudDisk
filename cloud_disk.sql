CREATE DATABASE  IF NOT EXISTS `cloud_disk` /*!40100 DEFAULT CHARACTER SET utf8 */ /*!80016 DEFAULT ENCRYPTION='N' */;
USE `cloud_disk`;
-- MySQL dump 10.13  Distrib 8.0.34, for Win64 (x86_64)
--
-- Host: localhost    Database: cloud_disk
-- ------------------------------------------------------
-- Server version	8.0.16

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!50503 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `add_friend_info`
--

DROP TABLE IF EXISTS `add_friend_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `add_friend_info` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `sender` varchar(128) NOT NULL,
  `receiver` varchar(128) NOT NULL,
  `status` tinyint(4) NOT NULL DEFAULT '0',
  `accepted` tinyint(4) NOT NULL DEFAULT '0',
  `time` datetime NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `id_UNIQUE` (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=6 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `add_friend_info`
--

LOCK TABLES `add_friend_info` WRITE;
/*!40000 ALTER TABLE `add_friend_info` DISABLE KEYS */;
INSERT INTO `add_friend_info` VALUES (1,'koishi007','abc2222',1,1,'2023-11-02 23:41:54'),(2,'abc123','koishi007',1,0,'2023-11-03 00:46:19'),(3,'abc123','abc2222',1,1,'2023-11-03 00:54:41'),(4,'abc2222','koishi222',1,1,'2023-11-04 00:51:08'),(5,'koishi007','kemo1',0,0,'2023-11-05 20:43:12');
/*!40000 ALTER TABLE `add_friend_info` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `discuss_post`
--

DROP TABLE IF EXISTS `discuss_post`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `discuss_post` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `time` datetime NOT NULL,
  `sender` varchar(128) NOT NULL,
  `receiver` varchar(128) NOT NULL,
  `msg` text,
  PRIMARY KEY (`id`,`time`),
  UNIQUE KEY `id_UNIQUE` (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=24 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `discuss_post`
--

LOCK TABLES `discuss_post` WRITE;
/*!40000 ALTER TABLE `discuss_post` DISABLE KEYS */;
INSERT INTO `discuss_post` VALUES (1,'2023-10-30 23:44:18','koishi007','koishi007','你好啊，恋恋'),(2,'2023-10-30 23:47:23','koishi007','abc123','你好啊，abc123'),(3,'2023-10-30 23:48:30','koishi007','abc123','你好啊，abc123'),(4,'2023-10-31 00:10:02','koishi007','','koishi哈哈'),(5,'2023-10-31 00:11:52','koishi007','','nihaoa'),(6,'2023-10-31 00:13:45','koishi007','koishi007','kkkk'),(7,'2023-10-31 00:15:15','koishi007','koishi007','你好'),(8,'2023-10-31 00:16:57','koishi007','koishi007','nininin'),(9,'2023-10-31 00:18:40','koishi007','','ninnin'),(10,'2023-11-04 00:51:29','abc2222','koishi222','nihao '),(11,'2023-11-07 00:35:05','koishi007','koishi222','你好'),(12,'2023-11-07 00:38:51','koishi007','abc123','我在和自己聊天呢你你你你你你你你你你你你你你你你你你你你你你你你你你你你你你你你你你你你你你你你你你'),(13,'2023-11-07 23:41:29','abc123','koishi007','你好'),(14,'2023-11-08 00:08:20','koishi007','abc123','你好啊'),(15,'2023-11-08 00:08:27','abc123','koishi007','你好啊'),(16,'2023-11-08 00:08:41','koishi007','abc123','你是abc123?'),(17,'2023-11-08 00:08:52','abc123','koishi007','是啊，你是koishi007?'),(18,'2023-11-08 00:09:08','koishi007','abc123','是是是是是是是是是是是是是是是是是是是是是是是是是是是是是是是是是是是是是的'),(19,'2023-11-08 00:10:28','abc123','koishi007','我先下线了'),(20,'2023-11-08 00:10:37','koishi007','abc123','好'),(21,'2023-11-08 00:13:04','koishi007','abc123','你好？'),(22,'2023-11-12 22:47:10','koishi007','abc123','你好啊'),(23,'2023-12-03 21:18:06','koishi007','abc123','nihao ');
/*!40000 ALTER TABLE `discuss_post` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `file_share`
--

DROP TABLE IF EXISTS `file_share`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `file_share` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `path` varchar(256) NOT NULL,
  `file_name` varchar(128) NOT NULL,
  `sender` varchar(128) NOT NULL,
  `receiver` varchar(128) NOT NULL,
  `status` tinyint(4) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  UNIQUE KEY `id_UNIQUE` (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=5 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `file_share`
--

LOCK TABLES `file_share` WRITE;
/*!40000 ALTER TABLE `file_share` DISABLE KEYS */;
INSERT INTO `file_share` VALUES (1,'C:/Users/tqzth/source/qt/TcpClient/userData/koishi007','1.0.1.zip','koishi007','abc123',0),(2,'C:/Users/tqzth/source/qt/TcpClient/userData/koishi007','「代码随想录」贪心算法专题精讲（v2.0）.pdf','koishi007','abc123',0),(3,'C:/Users/tqzth/source/qt/TcpClient/userData/koishi007/新建文件夹(4)','《TCPIP网络编程(尹圣雨)》PDF+源代码.7z','koishi007','abc123',0),(4,'C:/Users/tqzth/source/qt/TcpClient/userData/koishi007','「代码随想录」贪心算法专题精讲（v2.0）.pdf','koishi007','abc123',0);
/*!40000 ALTER TABLE `file_share` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `friendinfo`
--

DROP TABLE IF EXISTS `friendinfo`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `friendinfo` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `User1` varchar(128) NOT NULL,
  `User2` varchar(128) NOT NULL,
  PRIMARY KEY (`id`,`User2`,`User1`),
  UNIQUE KEY `id_UNIQUE` (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=12 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `friendinfo`
--

LOCK TABLES `friendinfo` WRITE;
/*!40000 ALTER TABLE `friendinfo` DISABLE KEYS */;
INSERT INTO `friendinfo` VALUES (1,'abc123','koishi007'),(2,'koishi222','koishi007'),(6,'koishi007','abc2222'),(8,'abc123','abc2222'),(11,'abc2222','koishi222');
/*!40000 ALTER TABLE `friendinfo` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `userinfo`
--

DROP TABLE IF EXISTS `userinfo`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `userinfo` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `user_name` varchar(128) NOT NULL,
  `pwd` varchar(128) NOT NULL,
  `online` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  UNIQUE KEY `user_name` (`user_name`)
) ENGINE=InnoDB AUTO_INCREMENT=19 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `userinfo`
--

LOCK TABLES `userinfo` WRITE;
/*!40000 ALTER TABLE `userinfo` DISABLE KEYS */;
INSERT INTO `userinfo` VALUES (1,'koishi','123456',0),(2,'abc123','123456',0),(3,'ddd123','123456',0),(4,'koishi007','123456',0),(9,'kemoji','123456',0),(10,'koishi008','123456',0),(11,'koishi009','123456',0),(12,'koishi010','123456',0),(13,'kemojiK','123456',0),(14,'kemo','123456',0),(15,'kemo1','123456',0),(16,'abc2222','123456',0),(17,'koishi222','123456',0),(18,'koishi1111','123456',0);
/*!40000 ALTER TABLE `userinfo` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2023-12-17 23:23:56
