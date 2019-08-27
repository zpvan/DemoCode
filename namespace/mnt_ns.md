文件打开的源码分析

1 sys_open

打开一个文件, 是通过内核提供的系统调用sys_open实现的, sys_open是do_sys_open的封装函数



2 do_sys_open

do_sys_open首先把文件名从用户态复制到内核, 然后获得一个未使用的文件号, 最后调用do_flip_open执行文件打开的过程



3 do_flip_open

主要执行两步:

- 第一步是open_namei, 它的作用是沿着要打开文件名的整个路径, 一层层解析路径, 最后得到文件的dentry和vfsmount对象, 保存到一个nameidata结构中, 这个nameidata结构就是open_namei的输出参数nd
- 第二步是nameidata_to_flip, 它的作用是根据第一步获得的nameidata结构, 初始化一个file对象



4 open_namei

open_namei的第一部分是设置权限参数. 如果打开文件时带有O_TRUNC标志, 说明要修改文件的长度, 对文件的操作模式要加上可写权限; 如果打开文件时带有O_APPEND标志, 对文件的操作模式要加上MAY_APPEND权限. MAY_APPEND也可当做可写权限(MAY_WRITE)的一种

open_namei的第二部分是两种打开文件的模式. 打开文件的时候, 如果文件不存在, 可以为用户创建一个文件, 这是通过文件的O_CREATE标志来控制的. 如果不带有O_CREATE标志, 不需要创建文件, 那么调用path_lookup_open函数. 如果带有O_CREATE标志, 说明需要创建文件, 则调用path_lookup_create函数(调用函数时带有LOOKUP_PARENT)标志. path_lookup_create不处理最终目标文件, 它只查找到文件所在目录就结束查找过程了, 等函数返回后, 再检查最终目标文件是否存在.

open_namei的第三部分首先检查path_lookup_create函数的返回值. 第一种情况是检查返回类型. 返回类型有很多种, 可以是LAST_NORM, LAST_DOT, LAST_DOTDOT等. 如果返回不等于LAST_NORM, 说明文件名字是"."或者"..", 则直接返回. 另一种情况是文件名是一个目录, 也不处理, 直接返回. 如果文件名是目录, 那么文件名最后一个字符是斜杠符"/", 而普通文件的最后一个字符不可能是斜杠符, 因此目录文件的长度比nd指示的文件名长度多出一个字符, 通过检查最后一个字符是否为空判断文件是否是目录.



5 nameidata_to_flip

实现打开文件的最后一步, 获得文件结构. 文件结构file已经在open_namei的过程中创建了. 只不过它还没完成初始化.

nameidata_to_flip主要调用__dentry_open, 对文件打开时设置的选项进行处理.分两个部分.

__dentry_open的第一部分主要是初始化文件的参数. 文件的操作数f_op从inode获得. f_mapping是文件的读写cache的管理结构, 同样从inode获得. 函数file_move把文件加入超级块对象的文件链表, 这样可以从超级块对象遍历整个文件系统内所有的文件结构

__dentry_open的第二部分首先检查文件系统是否为文件定义了open函数, 如果已经定义, 那么随后执行文件的open函数. 然后file_ra_stat_init初始化文件预读的参数