# .bashrc

# User specific aliases and functions

alias rm='rm -i'
alias cp='cp -i'
alias mv='mv -i'

# Source global definitions
if [ -f /etc/bashrc ]; then
	. /etc/bashrc
fi

##############
# java
##############
export JAVA_HOME=/usr/local/jdk1.8.0_111
export JRE_HOME=$JAVA_HOME/jre
export CLASSPATH=.:$JAVA_HOME/lib:$JRE_HOME/lib:$CLASSPATH
export PATH=$JAVA_HOME/bin:$JRE_HOME/bin:$PATH
export M2_HOME=/usr/local/apache-maven-3.5.0
export PATH=/opt/scala/sbt/:$M2_HOME/bin:/home/gadmin/ob_rel/redis:$PATH

###########################
# strom_kafka-flume
#####################
export STORM_HOME=/usr/local/storm1.1
export KAFKA_HOME=/usr/local/kafka_2.12-0.10.2.1
export ZOOKEEPER_HOME=/usr/local/zookeeper-3.4.10
export MAVEN_HOME=/usr/local/apache-maven-3.5.0
export FLUME_HOME=/usr/local/apache-flume-1.7.0-bin
export REDIS_HOME=/usr/local/redis-3.2.9
export ES_HOME=/usr/local/elasticsearch-5.5.1
export PATH=$PATH:$STORM_HOME/bin:$KAFKA_HOME/bin:$KAFKA_HOME/bin:$MAVEN_HOME/bin:$FLUME_HOME/bin:$ES_HOME/bin
###########################
# alias
#####################
alias c="source ~/.bashrc"
alias cdstorm="cd /usr/local/storm1.1"
alias cdzk="cd /usr/local/zookeeper-3.4.10"
alias cdsrs="cd /usr/local/srs/objs"
alias cdl="cd /usr/local"
alias cdk="cd /usr/local/kafka_2.12-0.10.2.1"
alias cds="cd /usr/local/storm1.1"
alias mdbclient="/home/gadmin/ob_rel/redis/redis-cli -h 10.112.179.21 -p 6379"
alias cdlog="cd /usr/local/storm1.1/workplace/workers"
alias cdcode="cd /home/gadmin/work"
alias cdsrs="cd /usr/local/srs/objs"
alias cdm="cd /home/gadmin/work/meetctl/src/dns"
alias cdredis="cd /usr/local/redis-3.2.9"
export LANG=en_US.utf-8
alias cdj="cd /root/mygo/src/github.com/evolsnow/robot"
alias ll="ls -ltra"

#############
#go
##############
#根目录
export GOROOT=/usr/local/go
#bin目录
export GOBIN=$GOROOT/bin
#工作目录
export GOPATH=/root/mygo
export PATH=$PATH:$GOPATH:$GOBIN:$GOPATH

#############
#python
##############


