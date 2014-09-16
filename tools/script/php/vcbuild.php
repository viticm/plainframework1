<?php
/**
 * bacause the vs obj name can't rename, so i will chang it
 * @author viticm<viticm@126.com>
 * @version 2.0
 * @uses rename source files and visual studio script(also can revert it)
 */

/**
 * string replace of php system extend (viticm)
 * @param mixed $search
 * @param mixed $replace
 * @param mixed $subject
 * @param number $count
 * @param bool $beginlast
 * @return mixed
 */
function str_nreplace($search, 
                      $replace, 
                      $subject, 
                      $count = -1, 
                      $beginlast = false) {
  $result = $subject;
  if ($count < -1) return $result;
  if (-1 === $count) {
    $result = str_replace($search, $replace, $subject);
  }
  else {
    for ($i = 0; $i < $count; ++$i) {
      $pos = $beginlast ? strrpos($result, $search): strpos($result, $search);
      if ($pos === false) {
        break;
      }
      else {
        $result = substr_replace($result, $replace, $pos, strlen($search));
      }
    }
  }
  return $result;
}

/**
 * complement full path(add the last delimiter)
 * this function not check the path if exist
 * @param string $path
 * @param string $delimiter
 * @return string
 */
function complementpath($path, $delimiter = '/') {
  $result = $path;
  $lastlimiter = substr($path, -1, 1);
  if ($lastlimiter != $delimiter) $result .= $delimiter;
  return $result;
}

/**
 * format the path to use system path
 * and if path like this server/name/../path will chage to server/path
 * @param string $path
 * @param string $from_ostype
 * @return string
 */
function format_systempath($path, $from_ostype = OS_LINUX) {
  $result = $path;
  $current_ostype = get_ostype();
  $find_delimiter = OS_WINDOWS == $from_ostype ? '\\' : '/';
  $delimiter = OS_WINDOWS == $current_ostype ? '\\' : '/';
  $result = str_replace($find_delimiter, $delimiter, $path);
  $fatherpath = '..'.$delimiter;
  while (($fatherpath_pos = strpos($result, $fatherpath)) !== false) {
    $temp = substr($result, 0, $fatherpath_pos - 1);
    $find_fatherpath_pos = strrpos($temp, $delimiter);
    if (!$find_fatherpath_pos) break;
    $find_fatherpath_length = strlen($temp) - $find_fatherpath_pos;
    $result = substr_replace($result, 
                             '', 
                             $find_fatherpath_pos, 
                             $find_fatherpath_length);
    $result = str_nreplace($fatherpath, '', $result, 1);
  }
  return $result;
}

//project visual studio script file dirs
$g_scriptdirs = array(
  "gateway applications/gateway/scripts",
  "pf_simple applications/pf_simple/scripts",
  "pf_core pf/core/scripts",

); //not use EOF, if you want it work not just use output

$selfpath = str_replace('\\', '/', dirname(realpath(__FILE__)));
$projectpath = str_replace('tools/script/php', '', $selfpath);
define('PROJECTPATH', $projectpath);
define('PROJECTNAME', 'pap');
define('OS_UNKONWN', 0);
define('OS_WINDOWS', 1);
define('OS_LINUX', 2);

/**
 * get server visual studio script dir(full path)
 * @param string $modelname
 * @return string
 */
function get_scriptdir($modelname) {
  $result = NULL;
  $scriptdir_array = $GLOBALS['g_scriptdirs'];
  foreach ($scriptdir_array as $key => $val) {
    list($model, $dir) = explode(' ', $val);
    if ($model == $modelname) {
      $result = PROJECTPATH.$dir;
      break;
    }
  }
  $result = complementpath($result);
  return $result;
}

/**
 * rename dir or file (form system cmd)
 * @param string $path
 * @param string $old
 * @param string $new
 */
function sys_rename($path, $old, $new) {
  $path = complementpath($path);
  $path = format_systempath($path);
  if (!file_exists($path.$old) && !is_dir($path.$old)) return false;
  $ostype = get_ostype();
  $cmd = '';
  $cmd .= 'cd '.$path;
  $cmd .= ' &&';
  $cmd .= OS_WINDOWS == $ostype ? ' ren' : ' mv';
  $cmd .= ' '.$old.' '.$new;
  execcmd($cmd, $ostype);
  return true;
}

/**
 * delete path more delimiter, example: /// to /
 */
function trimpath($path) {
  $result = $path;
  
  $result = str_replace('//', '/', $result, $count);
  while ($count != 0) {
    $result = str_replace('//', '/', $result, $count);
  }
  $result = str_replace('\\\\', '\\', $result, $count);
  while ($count != 0) {
    $result = str_replace('\\\\', '\\', $result, $count);
  }
  return $result;
}

/**
 * get server visual studio script files(full path)
 * @param string $path
 * @return array
 */
function get_scriptfile($path) {
  $result = array();
  if (empty($path) || !is_dir($path)) return $result;
  $vc9_scriptfiles = glob($path.'*.vcproj');
  $vc11_scriptfiles = glob($path.'*.vcxproj*');
  $result = array_merge($vc9_scriptfiles, $vc11_scriptfiles);
  return $result;
}

/**
 * rewrite visual studio script
 * @param string $modelname
 * @param bool|number $revert
 * @param bool $randsuffix
 * @return bool
 */
function rewrite_vcscript($modelname = NULL, 
                          $revert = false, 
                          $randsuffix = false) {
  $result = true;
  if (empty($modelname)) return false;
  $model_scriptpath = get_scriptdir($modelname);
  $needrevert = file_exists($model_scriptpath.'vcscript_revert');
  if (-1 == $revert && $needrevert) $revert = true;
  if (true === $revert) {
    echo '['.$modelname.'] will revret to standard.',"\n";
  }
  else {
    echo '['.$modelname.'] will rename the all scripts',"\n";
  }
  if (false == $revert && $needrevert) {
    echo 'warning: ['.$modelname.'] need revert, your choice not revert.';
  }
  $scriptfiles = get_scriptfile($model_scriptpath);
  if (0 == count($scriptfiles)) return false;
  foreach ($scriptfiles as $scriptfile) {
    $scriptfile_info = file_get_contents($scriptfile);
    $sourcefiles = array();
    $matchcorcc_files = array();
    $matchcpp_files = array(); 
    preg_match_all('/".*\.cc?"/', $scriptfile_info, $matchcorcc_files);
    preg_match_all('/".*\.cpp"/', $scriptfile_info, $matchcpp_files);
    $sourcefiles = array_merge($matchcorcc_files[0], $matchcpp_files[0]);
    foreach ($sourcefiles as $sourcefile) {
      $scriptfile_old = $sourcefile;
      $sourcefile = substr($sourcefile, 1, strlen($sourcefile) - 2); //del '"'
      $sourcefile = trimpath($sourcefile);
      $sourcefile_path = $model_scriptpath.$sourcefile;
      $sourcefile_path = format_systempath($sourcefile_path);
      $sourcefile_name = basename($sourcefile_path);
      $sourcefile_dir = dirname($sourcefile_path);
      $temp = str_replace(format_systempath(PROJECTPATH), '', $sourcefile_dir);
      $temp = str_nreplace('src\\', '', $temp, 1);
      $new_sourcefile_name = str_replace('\\', '_', $temp);
      if (true === $revert) {
        $new_sourcefile_name = str_replace($new_sourcefile_name.'_', 
                                           '', 
                                           $sourcefile_name);
        $randsuffix_match = array();
        preg_match('/_\d{11,13}\./', $sourcefile_name, $randsuffix_match);
        if (count($randsuffix_match) > 0) {
          $randsuffix_str = //del '.'
            substr($randsuffix_match[0], 0, strlen($randsuffix_match[0]) - 1);
          $new_sourcefile_name = //del random str
            str_replace($randsuffix_str, '', $new_sourcefile_name);
        }
      }
      else {
        $new_sourcefile_name .= 
          true === $randsuffix ? '_'.time().rand(1, 1000) : '';
        $new_sourcefile_name .= '_'.$sourcefile_name;
      }
      $new_sourcefile = str_replace($sourcefile_name,
                                    $new_sourcefile_name,
                                    $sourcefile);
      sys_rename($sourcefile_dir, 
                 $sourcefile_name, 
                 $new_sourcefile_name);
      $scriptfile_info = str_replace($scriptfile_old, 
                                     '"'.$new_sourcefile.'"', 
                                     $scriptfile_info);

    }
    $result = file_put_contents($scriptfile, $scriptfile_info);
    if (!$result) {
      echo 'rewrite '.$scriptfile.' failed!',"\n";
    }
    else {
      echo 'rewrite '.$scriptfile.' success.',"\n";
    }
  }

$revert_fileinfo = <<<EOF
1
en:
  This is a flag file of visual studio script if need revert.
  If you don't understand this, can't delete it.
cn:
    这个文件用来作为是否需要还原vcscript的标记。
    如果你不清楚该文件的用途，则不要删除它。
EOF;
 
  if (true === $revert) { 
    @unlink($model_scriptpath.'vcscript_revert');
  }
  else {
    file_put_contents($model_scriptpath.'vcscript_revert', $revert_fileinfo);
  }
}

/**
 * get os type(windows/linux)
 * @param void
 * @return int
 */
function get_ostype() {
  if (str_ireplace('windows', '', php_uname('s')) != php_uname('s'))
    return OS_WINDOWS;
  if (str_ireplace('linux', '', php_uname('s')) != php_uname('s'))
    return OS_LINUX;
  return OS_UNKONWN;
}

/**
 * exec command
 * @param string $cmd
 * @param number $ostype
 */
function execcmd($cmd, $ostype) {
  if (OS_WINDOWS == $ostype) {
    $cmdarray = explode(' ', $cmd);
    foreach ($cmdarray as $k => &$val) {
      if (is_dir($val) || is_file($val)) {
        $val = str_replace('/', '\\', $val);
      }
    }
    $cmd = implode(' ', $cmdarray);
  }
  exec($cmd);
}

/**
 * this php file just a script command like shell, as c/c++ enter function
 * @param void
 * @return int
 */
function main() {
  $argc = $GLOBALS['argc'];
  $argv = $GLOBALS['argv'];
  if (1 >= $argc || 3 < $argc) {
    echo 'param error',"\n";
    return 1;
  }
  $revert = -1;
  if (3 == $argc && 'yes' === $argv[2]) $revert = true;
  if (3 == $argc && 'no' === $argv[2]) $revert = false;
  $models = explode(' ', $argv[1]);
  foreach ($models as $k => $model) {
    $result = 
      2 == $argc ? rewrite_vcscript($model) : rewrite_vcscript($model, $revert);
    if (!$result) return 1;
  }
  return 0;
}
main();