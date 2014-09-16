#!/bin/bash
#filename:test.sh
#author:viticm
#createtime:2013-5-31 11:12:03
#use: install web develop
cUser=`whoami`
cCommand=$1
cDownload=on
cBase=no
cWebUser=www
cInstallFile=/web/
cInstallPhpPath=/usr/local/php/
cInstallNginxPath=/usr/local/nginx/
cInstallMysqlPath=/usr/local/mysql/
cPhpMakefile=/home/Makefile
iSystemType=`getconf LONG_BIT`
Arr_OpenPortList=""
cTestDomain=www.baidu.com

##### mysql config #####
cMysqlDefaultPasswd=mysql
cMysqlInstallPath=/usr/local/mysql/
cMysqlConfUploadFile=`pwd`/my.cnf
##### mysql config #####

####### PHP #######
cDate_TimeZone="Asia/Shanghai"
cDisplay_Errors=On
cError_Reporting="E_ALL & ~E_NOTICE"
cAllow_Call_Time_Pass_Reference=On
###### PHP ########

#### PHP-FPM CONFIGS ####
pm_max_children=64 
pm_start_servers=20
pm_min_spare_servers=5
pm_max_spare_servers=35
pm_max_requests=1024
pm_user=${cWebUser}
pm_group=${cWebUser}
#### PHP-FPM CONFIGS ####

usage()
{
cat <<EOF

Usage: $0 <params>
-m or --mysql install mysql
-n or --nginx install nginx
-p or --php install php( and this can enter two params,like -p 5.3( 5.4 ) then install php what version is 5.3 )


EOF
return 0
}

# on OEL, /etc/issue states "Enterprise Linux Enterprise Linux Server"
SUPPORTED_OS='CentOS|Red Hat Enterprise Linux Server|Enterprise Linux Enterprise Linux Server|Fedora|SUSE|Debian GNU/Linux|Ubuntu|Oracle Linux Server'

if ! egrep -q "$SUPPORTED_OS" /etc/issue ; then
cat <<EOF

Unable to install: Your distribution is not suitable for installation using
Zend's DEB/RPM repositories. 

EOF
  exit 1
fi

# -v or --version
if [ "$1" = "-v" -o "$1" = "--version" ]; then
  echo "`basename $0` version 1.0.1"
  usage
  exit 0
fi

# -h or --help
if [ "$1" = "-h" -o "$1" = "--help" ]; then
  usage
  exit 0
fi

MYUID=`id -u 2> /dev/null`
if [ ! -z "$MYUID" ]; then
  if [ $MYUID != 0 ]; then
    echo "You need root privileges to run this script.";
    #exit 2
  fi
else
  echo "Could not detect UID";
  exit 2
fi

cat <<EOF
Hit ENTER to install web develop, or Ctrl+C to abort now.
EOF
# give read a parameter, as it required in dash
read answer


#Base
function InstallBasePackage()
{
  echo
  echo "************************************************************************"
  echo "* Start install Base package( vim git wget base for mysql php nginx ). *" 
  echo "************************************************************************"
#clean iptables
#  echo -e "\e[0;36;1mfirst will clean iptables,please wait.\e[0m"
#  iptables -F
#  iptables -X
#  iptables -Z
  if [[ ${cBase} == "ok" ]] ; then
    echo -e "\e[0;35;1myour base is ok,now will not install base package.\e[0m"
  else
    #it will download from network .
    echo -e "\e[0;33;1mnow will test your network if worked,please wait some time.\e[0m"
    ping -c 5 ${cTestDomain}
    if [[ $? -ne 0 ]] ; then 
      echo -e "\e[0;31;1myour network not worked,please check it.\e[0m"
      exit 1
    else
      echo -e "\e[0;35;1myour network is worked,now will download from your network.\e[0m"
    fi
  fi
  if [[ ${cBase} == "no" ]] ; then
#vim
#    yum -y install vim
    yum -y install ctags
    yum -y install cscope
    yum -y install ntp
#git
    yum -y install git
#wget
    yum -y install wget
#gcc
    yum -y install yum-fastestmirror
    yum -y install gcc gcc-c++ autoconf libjpeg libjpeg-devel libpng libpng-devel freetype freetype-devel libxml2 libxml2-devel zlib zlib-devel glibc glibc-devel glib2 glib2-devel bzip2 bzip2-devel ncurses ncurses-devel curl curl-devel e2fsprogs e2fsprogs-devel krb5 krb5-devel libidn libidn-devel openssl openssl-devel openldap openldap-devel nss_ldap openldap-clients openldap-servers
#Nginx
    yum -y install pcre-devel  zlib-devel readline-devel
#Php
    yum -y install gd-devel libjpeg-devel libpng-devel freetype-devel libxml2-devel curl-devel freetype-devel
#Mysql
    yum -y install bison gcc gcc-c++ autoconf automake zlib* libxml* ncurses-devel libtool-ltdl-devel*
#bc
    yum -y install bc
  fi

#synchronous time
  chkconfig ntpd on
  ntpdate cn.pool.ntp.org
  hwclock --systohc
#config ulimit configs 
  echo now will config limit allow handles.
  echo "* soft nofile 2048" >> /etc/security/limits.conf
  echo "* hard nofile 32768" >> /etc/security/limits.conf
  echo "config limits is ok, remember reboot your server."
  if [[ $? -ne 0 ]] ; then 
    echo -e "\e[0;31;1minstall base is failed.\e[0m"
    exit 1
  fi
  echo
  echo "**************************************"
  echo "* Install base package is completed. *"
  echo "**************************************"

}
#Mysql
function InstallMysql()
{
  echo
  echo "**********************************************"
  echo "* Start install mysql( Percona-Server-5.6 ). *"
  echo "**********************************************"
  local cMysqlPackage="Percona-Server-5.6.10-alpha60.2.tar.gz"
  local cMysqlPackageDir="Percona-Server-5.6.10-alpha60.2"
  if [[ ${cBase} == "no" ]] ; then    
    yum -y install make
    yum -y install cmake
    yum -y install libtermcap-devel
    yum -y install patch
    yum -y install perl
    yum -y install bison
  fi
  
  if [[ ${cDownload} == "off" ]] ; then
    cd ${cInstallFile}
  fi
  if [ -x ./${cMysqlPackageDir} ]
  then
#rm -fr Percona-Server-5.1.62
    echo "Percona is exists,use it."
  else
    if [ -f ./${cMysqlPackage} ]
    then
      tar -zxvf ${cMysqlPackage}
    else
      wget -c http://www.percona.com/redir/downloads/Percona-Server-5.6/Percona-Server-5.6.10-alpha60.2/source/Percona-Server-5.6.10-alpha60.2.tar.gz
      if [[ $? -ne 0 ]] ; then
        echo "download mysql is failed,please check your network."
        exit 1
      fi
      sleep 3
      tar -zxvf ${cMysqlPackage}
    fi
  fi
  echo "start install mysql"
  sleep 3
  #delete mysql install directory
  cd ${cMysqlPackageDir}
  if [[ "" != `ps -A | grep mysqld` ]]; then
    PRINTWARNING "mysql is install and run now will uninstall it, or Ctrl+C to abort now."
    read answer 
    service mysql stop
#   make clean
#   make uninstall
    rm -rf ${cMysqlInstallPath}
  fi
  
  CC=gcc CFLAGS="-DBIG_JOINS=1 -DHAVE_DLOPEN=1 -O3" CXX=g++ CXXFLAGS="-DBIG_JOINS=1 -DHAVE_DLOPEN=1 -felide-constructors -fno-rtti -O3"
  cmake -DCMAKE_INSTALL_PREFIX=/usr/local/mysql \
  -DMYSQL_UNIX_ADDR=/tmp/mysql.sock \
  -DWITH_EXTRA_CHARSETS=all \
  -DWITH_MYISAM_STORAGE_ENGINE=1 \
  -DWITH_INNOBASE_STORAGE_ENGINE=1 \
  -DWITH_READLINE=1 \
  -DENABLED_LOCAL_INFILE=1 \
  -DMYSQL_DATADIR=/var/mysql/data
  sleep 3
  make -j4 && make  install
  
  groupadd mysql
  useradd -g mysql mysql
  cd ${cMysqlInstallPath} && chown -R mysql . && chgrp -R mysql .
  echo "make install is ok"
  
  sleep 3
  mkdir -p /var/mysql/data && chown mysql:mysql /var/mysql/ -R
  if [[ -f ${cMysqlConfUploadFile} ]] ; then
    cp ${cMysqlConfUploadFile} /etc/my.cnf
  else
    cp ${cMysqlInstallPath}support-files/my-default.cnf /etc/my.cnf
    #sed "s/skip-locking/external-locking/g" -i /etc/my.cnf
    #sed "s/#innodb_/innodb_/g" -i /etc/my.cnf
    #sed -i '32 i\default-storage-engine=InnoDB' -i /etc/my.cnf   
  fi
  ${cMysqlInstallPath}/scripts/mysql_install_db --basedir=${cMysqlInstallPath}/ --user=mysql --datadir=/var/mysql/data
  ln -s /usr/local/mysql/lib/libmysqlclient.so.18 /usr/lib/libmysqlclient.so.18
  echo "install_db Initialize the database is complete"
  sleep 3
  echo "Try to start the database"
  cp ${cMysqlInstallPath}support-files/mysql.server /etc/init.d/mysql
  cp ${cMysqlInstallPath}/bin/mysql  /usr/sbin/
  service mysql start
  cd ${cMysqlInstallPath} && chown -R mysql . &&  chgrp -R mysql .
  echo "Database startup is complete"
  sleep 3
  
  echo "Change your password after 10 seconds"
  sleep 10
  cd ${cMysqlInstallPath} && ./bin/mysqladmin -uroot password ${cMysqlDefaultPasswd}

  if [[ "" == `cat /etc/rc.d/rc.local | grep "service mysql start"` ]] ; then
    echo "service mysql start" >> /etc/rc.d/rc.local
  fi
  if [[ $? -ne 0 ]] ; then
    echo -e "\e[0;31;1mInstallation fails, check the above error\e[0m"
    exit 1
  else
    echo -e "\e[0;34;1mCongratulations, the installation was successful.Directory:"${cMysqlInstallPath}"\e[0m"
  fi
  echo
  echo "**********************************************************"
  echo "* Install Mysql is completed.( Use Percona-Server-5.6 ). *" 
  echo "**********************************************************"

}

#Nginx
function InstallNginx()
{
  cd ${cInstallFile}
  local cNginxPackage=tengine-2.0.0.tar.gz
  local cNginxPackageDir=tengine-2.0.0
  echo
  echo "***************************************"
  echo "* Start install Nginx( Use tengine ). *" 
  echo "***************************************"
#if [[ "" != `ps -A | grep nginx` ]]; then
#    PRINTWARNING "mysql is install and run now will uninstall it, or Ctrl+C to abort now."
#    service nginx stop
#    make uninstall
#    rm -rf ${cInstallNginxPath}
#  fi

  groupadd www
  useradd -g www www
  if [[ ${cDownload} == "on" ]] ; then
    echo -e "\e[0;33;1mnow download package form website,check your network if failed!\e[0m"
#Nginx 1.3.0
    if [[ -f ${cInstallFile}${cNginxPackage} ]] ; then
      echo ${cNginxPackage} is found.
    else
      echo now will download ${cNginxPackage} from network.
      wget -c http://tengine.taobao.org/download/${cNginxPackage}
    fi
#openssl
    if [[ -f ${cInstallFile}openssl-1.0.1c.tar.gz ]] ; then
      echo openssl-1.0.1c.tar.gz is found.
    else
      echo now will download openssl-1.0.1c.tar.gz from network.
      wget -c  http://www.openssl.org/source/openssl-1.0.1c.tar.gz
    fi
#Nginx(pcre)
#    if [[ -f ${cInstallFile}pcre-8.30.tar.gz ]] ; then
#      echo pcre-8.30.tar.gz is found.
#    else
#      echo now will download pcre-8.30.tar.gz from network.
#      wget http://nchc.dl.sourceforge.net/project/pcre/pcre/8.30/pcre-8.30.tar.gz
#    fi
    bExit=`echo $?`
    if [[ ${bExit} != 0 ]] ; then 
      echo -e "\e[0;31;1mdownload package is failed,Please check your network.\e[0m"
      exit
    fi
   fi   
#install pcre
#  tar -zxvf pcre-8.30.tar.gz && cd pcre-8.30/ && ./configure
#  make -s -j4 && make install
#  cd ../
#install openssl
  if [[ -d openssl-1.0.1c ]] ; then
    PRINTUSEDIR openssl-1.0.1c
  else
    tar zxvf openssl-1.0.1c.tar.gz #&& cd openssl-1.0.1c
  fi
    
#  ./config --prefix=/usr/local/ --openssldir=/usr/local/openssl-1.0.1c shared zlib-dynamic enable-camellia enable-tlsext -fPIC
#  make && make install && cd ../
#install nginx   
  if [[ -d ${cNginxPackageDir} ]] ; then
    PRINTUSEDIR ${cNginxPackageDir}
  else
    tar -xzvf ${cNginxPackage}
  fi
  cd ${cInstallFile}${cNginxPackageDir}
  make clean
  ./configure --user=${cWebUser} --group=${cWebUser} \
  --prefix=${cInstallNginxPath} \
  --sbin-path=${cInstallNginxPath}sbin/nginx \
  --conf-path=${cInstallNginxPath}conf/nginx.conf \
  --with-http_stub_status_module \
  --lock-path=/var/run/nginx.lock \
  --pid-path=/var/run/nginx.pid \
  --with-openssl=../openssl-1.0.1c \
  --with-pcre-jit
  sleep 10
  make && make install
  bExit=`echo $?`
  if [[ ${bExit} != 0 ]] ; then
    echo -e "\e[0;31;1minstall nginx have some error,you can read it.\e[0m"
    exit
  else
    echo -e "\e[0;34;1mgood luck! install nginx sucess! \e[0m"
  fi
#if install nginx is sucess,then will config it.
#  echo "Now will config the nginx,please wait."

  if [[ -f ${cNginxConfFile} ]] ; then
    cp -n ${cNginxConfFile} ${cInstallNginxPath}conf/nginx.conf
  else
    sed -e "2 c\user ${cWebUser};" -i ${cInstallNginxPath}conf/nginx.conf
  fi
#start nginx and set start on boot
  echo ${cInstallNginxPath}sbin/nginx >> /etc/rc.d/rc.local
  ${cInstallNginxPath}sbin/nginx
#open port 3306 and 80
  iptables -I INPUT -p tcp --dport 80 -j ACCEPT
  iptables -I OUTPUT -p tcp --sport 80 -j ACCEPT
  iptables -I INPUT -p tcp --dport 3306 -j ACCEPT
  iptables -I OUTPUT -p tcp --sport 3306 -j ACCEPT
  /etc/rc.d/init.d/iptables save
  service iptables restart
  echo
  echo "***********************************************"
  echo "* Install Nginx is completed.( Use tengine ). *" 
  echo "***********************************************"

}


#PHP
function InstallPHP()
{
  cd ${cInstallFile}
  echo
  echo "**********************************************"
  echo "* Start install PHP.( Default version 5.3 ). *" 
  echo "**********************************************"

  local cPhpVersion=${2}
  local cPhpPackage=""
  local cPhpPackageDir=""
  if [[ -z ${cPhpVersion} ]]; then
    echo install default php version 5.3
    cPhpVersion="5.3"
    cPhpPackage="php-5.3.28.tar.bz2"
    cPhpPackageDir="php-5.3.28"
  else
    if [[ "5.3" != ${cPhpVersion} && "5.4" != ${cPhpVersion} ]]; then
      Useage
      exit 1
    else
      if [[ "5.3" == ${cPhpVersion} ]]; then
        cPhpPackage="php-5.3.28.tar.bz2"
        cPhpPackageDir="php-5.3.28"
      else
        cPhpPackage="php-5.4.15.tar.bz2"
        cPhpPackageDir="php-5.4.15"
      fi
    fi
  fi

  if [[ ${cDownload} == "on" ]] ; then
    echo now download package form website,check your network if failed!
#PHP5.3.14
    if [[ -f ${cInstallFile}${cPhpPackage} ]] ; then
      echo ${cPhpPackage} is found.
    else
      echo now will download php package from network.
      wget -c http://cn.php.net/distributions/${cPhpPackage}
    fi
#PHP
    if [[ -f ${cInstallFile}mcrypt-2.6.8.tar.gz ]] ; then
      echo mcrypt-2.6.8.tar.gz is found.
    else
      echo now will download mcrypt-2.6.8.tar.gz from network.
      wget http://ncu.dl.sourceforge.net/project/mcrypt/MCrypt/2.6.8/mcrypt-2.6.8.tar.gz
    fi

    if [[ -f ${cInstallFile}mhash-0.9.9.9.tar.gz ]] ; then
      echo mhash-0.9.9.9.tar.gz is found.
    else
      echo now will download mhash-0.9.9.9.tar.gz from network.
      wget http://ncu.dl.sourceforge.net/project/mhash/mhash/0.9.9.9/mhash-0.9.9.9.tar.gz
    fi

    if [[ -f ${cInstallFile}libmcrypt-2.5.8.tar.gz ]] ; then
      echo libmcrypt-2.5.8.tar.gz is found.
    else
      echo now will download libmcrypt-2.5.8.tar.gz from network.
      wget http://ncu.dl.sourceforge.net/project/mcrypt/Libmcrypt/2.5.8/libmcrypt-2.5.8.tar.gz
    fi

    if [[ -f ${cInstallFile}libiconv-1.14.tar.gz ]] ; then
      echo libiconv-1.14.tar.gz is found.
    else
      echo now will download libiconv-1.14.tar.gz from network.
      wget http://ftp.gnu.org/pub/gnu/libiconv/libiconv-1.14.tar.gz
    fi
      
    if [[ -f ${cInstallFile}memcache-3.0.6.tgz ]] ; then
      echo memcache-3.0.6.tgz is found.
    else
      echo now will download memcache-3.0.6.tgz from network.
      wget http://pecl.php.net/get/memcache-3.0.6.tgz
    fi

    if [[ -f ${cInstallFile}PDO_MYSQL-1.0.2.tgz ]] ; then
      echo PDO_MYSQL-1.0.2.tgz is found.
    else
      echo now will download PDO_MYSQL-1.0.2.tgz from network.
      wget http://pecl.php.net/get/PDO_MYSQL-1.0.2.tgz
    fi
    bExit=`echo $?`
    if [[ ${bExit} != 0 ]] ; then
      echo -e "\e[0;31;1mdownload package failed,you can check your network if can work.\e[0m"
      exit
    fi
  fi
  echo "install some plugin for php"
#1
  if [[ -d libiconv-1.14/ ]] ; then
    PRINTUSEDIR libiconv-1.14/
  else
    tar -zxvf libiconv-1.14.tar.gz && cd libiconv-1.14/
  fi
  ./configure --prefix=/usr/local
  make -s -j4 && make install && cd ../
  sleep 1
#2
  if [[ -d libmcrypt-2.5.8/ ]] ; then
    PRINTUSEDIR libmcrypt-2.5.8/
  else
    tar -zxvf libmcrypt-2.5.8.tar.gz && cd libmcrypt-2.5.8/
  fi
  ./configure &&  make && make install
  /sbin/ldconfig && cd libltdl/ && ./configure --enable-ltdl-install
  make -s -j4 && make install && cd ../ && cd ../
  sleep 2
#3
  if [[ -d mhash-0.9.9.9/ ]] ; then
    PRINTUSEDIR mhash-0.9.9.9/
  else
    tar -zxvf mhash-0.9.9.9.tar.gz && cd mhash-0.9.9.9/
  fi
  ./configure
  make -s -j4 && make install && cd ../
  sleep 3
#4
  ln -s /usr/local/lib/libmcrypt.la /usr/lib/libmcrypt.la
  ln -s /usr/local/lib/libmcrypt.so /usr/lib/libmcrypt.so
  ln -s /usr/local/lib/libmcrypt.so.4 /usr/lib/libmcrypt.so.4
  ln -s /usr/local/lib/libmcrypt.so.4.4.8 /usr/lib/libmcrypt.so.4.4.8
  ln -s /usr/local/lib/libmhash.a /usr/lib/libmhash.a
  ln -s /usr/local/lib/libmhash.la /usr/lib/libmhash.la
  ln -s /usr/local/lib/libmhash.so /usr/lib/libmhash.so
  ln -s /usr/local/lib/libmhash.so.2 /usr/lib/libmhash.so.2
  ln -s /usr/local/lib/libmhash.so.2.0.1 /usr/lib/libmhash.so.2.0.1
  ln -s /usr/local/bin/libmcrypt-config /usr/bin/libmcrypt-config
  sleep 1
#5
  /sbin/ldconfig
  if [[ -d mcrypt-2.6.8/ ]] ; then
    PRINTUSEDIR mcrypt-2.6.8/
  else
    tar -zxvf mcrypt-2.6.8.tar.gz && cd mcrypt-2.6.8/
  fi
  ./configure
  make -s -j4 && make install && cd ../
  sleep 2
  bExit=`echo $?`
  if [[ ${bExit} != 0 ]] ; then
    echo -e "\e[0;31;1minstall package false,please read the errors.\e[0m"
    exit
  fi
  echo install plugin for php is completed.
  echo now will install php, version is ${cPhpVersion}
#64 OR 32 SYSTEM LIB MUST BE DIFFERENT
  if [[ ${iSystemType} == 64 ]] ; then    
    cp -frp /usr/lib64/libldap* /usr/lib
  fi
#6
  cd ${cInstallFile}
  if [[ -d ${cPhpPackageDir} ]] ; then
    PRINTUSEDIR ${cPhpPackageDir}
  else
    tar -xjvf ${cPhpPackage} 
    cd ${cPhpPackageDir}
  fi
  ./configure --prefix=${cInstallPhpPath} \
  --with-config-file-path=${cInstallPhpPath}etc \
  --with-iconv-dir=/usr/local/ --with-freetype-dir \
  --with-mysql=${cInstallMysqlPath} \
  --with-mysqli=${cInstallMysqlPath}bin/mysql_config \
  --with-jpeg-dir --with-png-dir --with-zlib \
  --with-mhash --enable-sockets --enable-ftp \
  --with-libxml-dir --enable-xml --disable-rpath \
  --enable-safe-mode --enable-bcmath \
  --enable-shmop --enable-sysvsem \
  --enable-inline-optimization --with-curl \
  --with-curlwrappers \
  --enable-mbregex \
  --enable-mbstring --with-mcrypt --with-gd \
  --enable-gd-native-ttf --with-openssl --with-mhash \
  --enable-pcntl --enable-sockets --with-ldap --with-ldap-sasl \
  --enable-fpm \
  --with-xmlrpc --enable-zip --enable-soap \
  --without-pear;
  make -s -j4 ZEND_EXTRA_LIBS='-liconv'
  make install
  cp php.ini-production ${cInstallPhpPath}etc/php.ini
  ln -s ${cInstallPhpPath}bin/php /usr/sbin/php
#now we can config php.ini
  sed "538 c\display_errors = ${cDisplay_Errors}" -i ${cInstallPhpPath}etc/php.ini
  sed "521 c\error_reporting = ${cError_Reporting}" -i ${cInstallPhpPath}etc/php.ini
  sed "334 c\allow_call_time_pass_reference = ${cAllow_Call_Time_Pass_Reference}" -i ${cInstallPhpPath}etc/php.ini
  sed "1008 c\date.timezone = ${cDate_TimeZone}" -i ${cInstallPhpPath}etc/php.ini
  bExit=`echo $?`
  if [[ ${bExit} != 0 ]] ; then
    echo -e "\e[0;31;1minstall PHP failed,you can read the errors or send email to me.\e[0m"
    exit
  fi
  echo -------------------------------end-----------------------------------
  cd ${cInstallFile}
  echo "######################install Extensions for PHP#################"
  echo -------------------------------start---------------------------------
  if [[ -d memcache-3.0.6/ ]] ; then
    PRINTUSEDIR memcache-3.0.6/
  else
    tar -zxvf memcache-3.0.6.tgz
    cd memcache-3.0.6/
  fi
  ${cInstallPhpPath}bin/phpize
  ./configure --with-php-config=${cInstallPhpPath}bin/php-config
  sleep 1
  make -s -j4 && make install && cd ../
  if [[ -d PDO_MYSQL-1.0.2/ ]] ; then
    PRINTUSEDIR PDO_MYSQL-1.0.2/
  else
    tar -zxvf PDO_MYSQL-1.0.2.tgz
    cd PDO_MYSQL-1.0.2/
  fi
  ${cInstallPhpPath}bin/phpize
  ./configure --with-php-config=${cInstallPhpPath}bin/php-config  --with-pdo-mysql=${cInstallMysqlPath}
  make -s -j4 && make install && cd ../
  echo "[PDO_MYSQL]" >> ${cInstallPhpPath}etc/php.ini
  echo "extension=pdo_mysql.so" >> ${cInstallPhpPath}etc/php.ini
  if [[ $? -ne 0 ]] ; then
    echo -e "\e[0;31;1minstall extensions for php is failed.\e[0m"
    exit 1
  fi
  sleep 5
#PHP-FPM
  /usr/sbin/groupadd ${cWebUser} && /usr/sbin/useradd -g ${cWebUser} ${cWebUser}
  mkdir -p /var/log/nginx && chmod +w /var/log/nginx &&chown -R ${cWebUser}:${cWebUser} /var/log/nginx
  mkdir -p /data/${cWebUser} && chmod +w /data/${cWebUser} && chown -R ${cWebUser}:${cWebUser} /data/${cWebUser}
  cp -n ${cInstallPhpPath}etc/php-fpm.conf.default  ${cInstallPhpPath}etc/php-fpm.conf
  if [[ -f ${cPhpFpmConfFile} ]] ; then
    cp -n ${cPhpFpmConfFile} ${cInstallPhpPath}etc/php-fpm.conf
  else
    sed "s/;pid = /pid = /g" -i ${cInstallPhpPath}etc/php-fpm.conf
    sed "s/;error_log = /error_log = /g" -i ${cInstallPhpPath}etc/php-fpm.conf
    sed "s/pm.max_children = .*/pm.max_children = ${pm_max_children}/" -i ${cInstallPhpPath}etc/php-fpm.conf
    sed "s/pm.start_servers = .*/pm.start_servers = ${pm_start_servers}/" -i ${cInstallPhpPath}etc/php-fpm.conf
    sed "s/pm.min_spare_servers =.*/pm.min_spare_servers = ${pm_min_spare_servers}/" -i ${cInstallPhpPath}etc/php-fpm.conf
    sed "s/pm.max_spare_servers =.*/pm.max_spare_servers = ${pm_max_spare_servers}/" -i ${cInstallPhpPath}etc/php-fpm.conf
    sed "s/pm.max_requests =.*/pm.max_spare_servers = ${pm_max_requests}/" -i ${cInstallPhpPath}etc/php-fpm.conf
    sed "s/user =.*/user = ${pm_user}/" -i ${cInstallPhpPath}etc/php-fpm.conf
    sed "s/group =.*/group = ${pm_group}/" -i ${cInstallPhpPath}etc/php-fpm.conf
  fi
#start php-fpm and set start on boot
  cp ${cInstallFile}${cPhpPackageDir}/sapi/fpm/init.d.php-fpm /etc/rc.d/init.d/php-fpm
  chmod +x /etc/rc.d/init.d/php-fpm 
  echo service php-fpm start >> /etc/rc.d/rc.local
  service php-fpm start
  bExit=`echo $?`
  if [[ ${bExit} != 0 ]] ; then
    echo -e "\e[0;31;1minstall Extensions for PHP failed! \e[0m"
    exit 1
  else
    echo -e "\e[0;34;1mgood luck! you install Extensions for PHP is secess! \e[0m"
  fi
}

#download tips
function DownloadTip()
{
  if [[ ${cDownload} == "on" ]] ; then
    echo -e "\e[0;35;1mDownload is on,the package will download from website.Please check your network is worked.\e[0m"
  else
    echo -e "\e[0;30;1mDownload is off,make sure your package is Existing.\e[0m"
    #if download if off,now can download from our svn.
  fi

  if [[ -f ${cOtherOfflinePackageFile} ]] ; then
    tar -xzvf ${cOtherOfflinePackageFile} -C ${cInstallFile}
  fi
}
#this function can use insert string above last line
#this have two parameters,one is string another is file path.
#string ervery line must be division by '|'
function InsertStrAboveLastLine()
{
    oldIFS=$IFS
    IFS="|"
    for item in $1
    do
    iInsertLine=`awk 'END{print NR}' $2`
    cNewItem=`echo ${item} | sed '/^$/d'`  
    if [[ ${cNewItem} != "" ]] ; then
#echo this is null
    #else
#echo item:${cNewItem}
      sed -e "${iInsertLine} i\ ${cNewItem}" -i $2
    fi
#iInsertLine=`awk 'END{print NR}' ${cTestFile}`
#sed -e "${iInsertLine} i\ ${item}" -i ${cTestFile}
    done
    IFS=$oldIFS
}
#@desc: set iptable rules
#@param: void
#@return: void
function SetIptables()
{
  Arr_OpenPortList="${1}"
  echo now will config iptables, open port list is:
  PrintArrList "${Arr_OpenPortList}"
  for port in ${Arr_OpenPortList}
  do
    iptables -I INPUT -i eth0 -p tcp --dport ${port} -j ACCEPT 
    iptables -I OUTPUT -o eth0 -p tcp --sport ${port} -j ACCEPT
  done
  /etc/init.d/iptables save
  service iptables restart
}
#@desc: print array list
#@param: array
#@return void
function PrintArrList()
{
  Arr_List="${1}"
  for item in ${Arr_List}
  do
    echo -e "\e[0;33;1m${item}\e[0m"
  done
}


#@desc:print use dir str
#@param: string dirname
#@return: void
function PRINTUSEDIR()
{
  cDirName=$1
  echo ${cDirName} is found, use it and make uninstall.
  cd ${cDirName} && make uninstall
}
#@desc print red error and exit
#@param string $cErrorStr
#@return void
function PRINTERR()
{
  cErrorStr=${1}
  echo -e "\e[0;31;1mERROR:${cErrorStr}\e[0m"
  exit 1
}

#@desc print yellow warning and not exit
#@param string $cWarningStr
#@return void
function PRINTWARNING()
{
  cWarningStr=${1}
  echo -e "\e[0;33;1mWARNING:${cWarningStr}\e[0m"
}
#create install file
if [[ $cCommand != -base ]] ; then

  `mkdir -p ${cInstallFile}`
  bInstallFile=`echo $?`
  if [[ $bInstallFile != 0 ]] ; then
    echo -e "\e[0;31;1mmake ${cInstallFile} file is failed! \e[0m"
    exit
  else
    cd ${cInstallFile}
  fi  
fi

###################main##################
if [[ $cCommand == "-base" ]] ; then
  DownloadTip
#  if [[ ${cDownload} == "off" ]] ; then
#    echo Download is off,please modify it.
#    exit
#  else
  InstallBasePackage
#fi
elif [[ $cCommand == "-m" || "--mysql" == ${cCommand} ]] ; then
  DownloadTip
  InstallMysql
elif [[ $cCommand == "-n" || "--nginx" == ${cCommand} ]] ; then
  DownloadTip
  InstallNginx
elif [[ $cCommand == "-p" || "--php" == ${cCommand} ]] ; then
  DownloadTip
  InstallPHP
elif [[ $cCommand == "--test" ]] ; then
  InsertStrAboveLastLine "|1|2|3|4|5|" /home/test.txt
else
  DownloadTip
  InstallBasePackage
  sleep 3
  InstallMysql
  sleep 3
  InstallPHP
  sleep 3
  InstallNginx
  sleep 3
  SetIptables "${Arr_OpenPortList}"
  exit 0
fi
