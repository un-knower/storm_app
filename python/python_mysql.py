#!/usr/bin/python
#coding=utf-8
import  pymysql
import  pymysql.cursors
conn= pymysql.connect(
        host='bdm293207686.my3w.com',
        port = 3306,
        user='bdm293207686',
        passwd='qwer1234',
        db ='bdm293207686_db',
        charset="utf8",
        )
cur = conn.cursor()
#创建数据表
#cur.execute("create table student(id int ,name varchar(20),class varchar(30),age varchar(10))")

#插入一条数据
cur.execute("insert into day_feel(create_time,tag) VALUES(NOW(),'怪罪过去现在不行的')")
#修改查询条件的数据
#cur.execute("update student set class='3 year 1 class' where name = 'Tom'")

#删除查询条件的数据
#cur.execute("delete from student where age='9'")

cur.close()
conn.commit()
conn.close()
