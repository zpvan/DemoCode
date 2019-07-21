/**
 * 1. 利用函数eventfd(2)创建一个event_fd
 * 2. 打开文件memory.usage_in_bytes，得到usage_in_bytes_fd
 * 3. 往cgroup.event_control中写入这么一串：<event_fd> <usage_in_bytes_fd> <threshold>
 * 4. 然后通过读event_fd得到通知
 **/