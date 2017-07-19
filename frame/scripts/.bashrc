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
export PATH=/opt/scala/sbt/:$PATH

###########################
# strom_kafka-flume
#####################
export STORM_HOME=/usr/local/storm1.1
export KAFKA_HOME=/usr/local/kafka_2.12-0.10.2.1
export ZOOKEEPER_HOME=/usr/local/zookeeper-3.4.10
export MAVEN_HOME=/usr/local/apache-maven-3.5.0
export FLUME_HOME=/usr/local/apache-flume-1.7.0-bin
export PATH=$PATH:$STORM_HOME/bin:$KAFKA_HOME/bin:$KAFKA_HOME/bin:$MAVEN_HOME/bin:$FLUME_HOME/bin:/home/gadmin/ob_rel/redis
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

