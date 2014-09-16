/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id macros.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/07/10 16:45
 * @uses the game define module all macros file
 */
#ifndef COMMON_DEFINE_MACROS_H_
#define COMMON_DEFINE_MACROS_H_

/* account about { */
#define ACCOUNT_LENGTH_MAX 32 //邮箱账号会长一些
#define PASSWORD_LENGTH_MAX 36
/* } account about */

/* role about */
#define ROLE_NAME_LENGTH_MAX (3*10)
/* role about */

/* guild about { */
#define GUILDLEVEL_MAX 5 //公会最大等级
/* } guild about */

/* scene about { */
#define SCENE_MAX 1024
/* } scene about */

/* server about { */
#define OVER_SERVER_MAX 256 //一组（区）服务器最大的服务器数量
#define GROUP_SERVER_MAX 24 //一组（组）服务器最大的服务器数量
/* } server about */

/* database about { */
#define DB_ROLE_MAX 6 //数据库最多的角色数量
/* } database about */

#endif //COMMON_DEFINE_MACROS_H_
