#/**
# * PLAIN SERVER Engine ( https://github.com/viticm/plainserver )
# * $Id compiler.sh
# * @link https://github.com/viticm/plianserver for the canonical source repository
# * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
# * @license
# * @user viticm<viticm.ti@gmail.com>
# * @date 2014/07/04 16:25
# * @uses shell compile script
# */
#!/bin/sh

currentdir=`pwd`
project_rootpath=${currentdir}/../../../../
gateway_path=${project_rootpath}applications/gateway
center_path=${project_rootpath}applications/center
login_path=${project_rootpath}applications/login
server_path=${project_rootpath}applications/server

compile_gateway="not compile"
compile_center="not compile"
compile_login="not compile"
compile_server="not compile"

update_luadir="${project_rootpath}/tools/script/lua/update_cmakelist/"
update_luafile="update_CMakeLists.lua"

#print error message, red words
#@param string message
#@return void
function error_message() {
  local message=${@}
  echo -e "\e[0;31;1merror: ${message}\e[0m"
  exit 1
}

#print warning message, yellow words
#@param message
#@return string void
function warning_message() {
  local message=${@}
  echo -e "\e[0;33;1merror: ${message}\e[0m"
}

#print debug message, green words
#@param message
#@return string void
function debug_message() {
  local message=${@}
  echo -e "\e[0;32;1mdebug: ${message}\e[0m"
}

function main() {
  local cmdline=""
  cmdline=`which lua`
  if [[ $? != 0 ]] ; then
    error_message "not found lua in your system, at lest version 5.1"
  fi
  echo -n "do you want to complie gateway [y/n]:"
  read compile_gateway

  #update all cmakelists
  cd $update_luadir && lua $update_luafile
  cd $currentdir
  if [[ $compile_gateway == "y" ]] ; then
    debug_message "begine complie gateway `date`"
    cd $gateway_path && cmake ./ && make
    if [[ $? != 0 ]] ; then
      compile_gateway="gateway compile have error, code:"$?
    else
      compile_gateway="gateway compile success"
    fi
    debug_message "end complie gateway `date`"
  else
    compile_gateway="gateway not compile"
  fi
  cd $currentdir

  #result
  echo ""
  echo "************************[result]**************************"
  debug_message "compile end"
  debug_message $compile_gateway
  echo "************************[result]**************************"
}

#main function
main
