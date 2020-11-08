/* 这是一份关于从Http请求信息中提取键值的接口声明文件 */
#ifndef _URIDecode_H_
#define _URIDecode_H_
#include "status.h"
/*
 * 相关数据类型
 */
typedef unsigned char uChar;

/*
 * 获取指定键的键值
 *
 * queryStr -- 请求信息的字符串
 * keyName -- 指定的键名
 * resultp -- （用指针）返回键值
 * 返回值，如果操作成功返回OK；否则返回ERROR。 注意的是，键名不存在的话，键值返回NULL。
 */
STATUS
GetTheKeyValue(char const *queryStr, char const *keyName, uChar **resultp);
#endif
