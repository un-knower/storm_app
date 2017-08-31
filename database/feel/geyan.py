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

#插入一条数据
cur.execute("insert into geyan(name,author,feel,task) values('瞎看什么','troy','happy','work');")

cur.close()
conn.commit()
conn.close()
