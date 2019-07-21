/**
 * 1. 利用函数eventfd(2)创建一个event_fd
 * 2. 打开文件memory.pressure_level，得到pressure_level_fd
 * 3. 往cgroup.event_control中写入这么一串：<event_fd> <pressure_level_fd> <level>
 * 4. 然后通过读event_fd得到通知
 * 
 * note: 多个level可能要创建多个event_fd，好像没有办法共用一个
 **/