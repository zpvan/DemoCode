

                         CORE ANALYSIS SUITE

  The core analysis suite is a self-contained tool that can be used to
  investigate either live systems, kernel core dumps created from dump
  creation facilities such as kdump, kvmdump, xendump, the netdump and
  diskdump packages offered by Red Hat, the LKCD kernel patch, the mcore
  kernel patch created by Mission Critical Linux, as well as other formats
  created by manufacturer-specific firmware.

核心分析套件是一个可以使用的独立工具
调查实时系统，从dump创建的内核核心转储
创建设施，如kdump，kvmdump，xendump，netdump和
red Hat提供的diskdump包，LKCD内核补丁，mcore
由Mission Critical Linux创建的内核补丁，以及其他格式
由制造商特定的固件创建。

  o  The tool is loosely based on the SVR4 crash command, but has been
     completely integrated with gdb in order to be able to display 
     formatted kernel data structures, disassemble source code, etc.
     该工具松散地基于SVR4崩溃命令，但一直如此
     与gdb完全集成以便能够显示
     格式化内核数据结构，反汇编源代码等

  o  The current set of available commands consist of common kernel core
     analysis tools such as a context-specific stack traces, source code
     disassembly, kernel variable displays, memory display, dumps of 
     linked-lists, etc.  In addition, any gdb command may be entered,
     which in turn will be passed onto the gdb module for execution.  

​    当前可用命令集由通用内核核心组成
​    分析工具，例如特定于上下文的堆栈跟踪，源代码
​    反汇编，内核变量显示，内存显示，转储
​    链接列表等。此外，可以输入任何gdb命令，
​    然后将其传递到gdb模块以供执行。



  o  There are several commands that delve deeper into specific kernel
     subsystems, which also serve as templates for kernel developers
     to create new commands for analysis of a specific area of interest.
     Adding a new command is a simple affair, and a quick recompile
     adds it to the command menu.

​    有几个命令深入研究特定内核
​    子系统，也可作为内核开发人员的模板
​    创建用于分析特定感兴趣区域的新命令。
​    添加新命令是一件简单的事情，并且可以快速重新编译
​    将其添加到命令菜单中。



  o  The intent is to make the tool independent of Linux version dependencies,
     building in recognition of major kernel code changes so as to adapt to 
     new kernel versions, while maintaining backwards compatibility.

​    目的是使该工具独立于Linux版本依赖项，
​    构建以识别主要内核代码的变化以适应
​    新的内核版本，同时保持向后兼容性。

  

A whitepaper with complete documentation concerning the use of this utility
can be found here:

         http://people.redhat.com/anderson/crash_whitepaper

  These are the current prerequisites: 

  o  At this point, x86, ia64, x86_64, ppc64, ppc, arm, arm64, alpha, mips,
     s390 and s390x-based kernels are supported.  Other architectures may be
     addressed in the future.

​    此时，x86，ia64，x86_64，ppc64，ppc，arm，arm64，alpha，mips,
​    支持基于s390和s390x的内核。 其他架构可能是
​    在未来解决。

 o  One size fits all -- the utility can be run on any Linux kernel version
     version dating back to 2.2.5-15.  A primary design goal is to always
     maintain backwards-compatibility.

​    一种尺寸适合所有 - 该实用程序可以在任何Linux内核版本上运行
​    版本可以追溯到2.2.5-15。 主要设计目标是始终
​    保持向后兼容性。



  o  In order to contain debugging data, the top-level kernel Makefile's CFLAGS
     definition must contain the -g flag.  Typically distributions will contain
     a package containing a vmlinux file with full debuginfo data.  If not, the
     kernel must be rebuilt:

​    为了包含调试数据，顶级内核Makefile的CFLAGS
​    definition必须包含-g标志。 通常，分发包含
​    包含带有完整debuginfo数据的vmlinux文件的包。 如果没有，那就是
​    内核必须重建：



     For 2.2 kernels that are not built with -g, change the following line:
    
        CFLAGS = -Wall -Wstrict-prototypes -O2 -fomit-frame-pointer
    
     to:
    
        CFLAGS = -g -Wall -Wstrict-prototypes -O2 -fomit-frame-pointer
    
     For 2.4 kernels that are not built with -g, change the following line:
    
        CFLAGS := $(CPPFLAGS) -Wall -Wstrict-prototypes -O2 -fomit-frame-pointer -fno-strict-aliasing
    
     to:
    
        CFLAGS := -g $(CPPFLAGS) -Wall -Wstrict-prototypes -O2 -fomit-frame-pointer -fno-strict-aliasing
    
     For 2.6 and later kernels that are not built with -g, the kernel should
     be configured with CONFIG_DEBUG_INFO enabled, which in turn will add
     the -g flag to the CFLAGS setting in the kernel Makefile.
     
     After the kernel is re-compiled, the uncompressed "vmlinux" kernel
     that is created in the top-level kernel build directory must be saved.

  To build the crash utility: 

    $ tar -xf crash-7.2.6.tar.gz
    $ cd crash-7.2.6
    $ make

  The initial build will take several minutes  because the embedded gdb module
  must be configured and built.  Alternatively, the crash source RPM file
  may be installed and built, and the resultant crash binary RPM file installed.

  由于嵌入式gdb模块，初始构建将花费几分钟
  必须配置和构建。 或者，崩溃源RPM文件
  可以安装和构建，并安装生成的崩溃二进制RPM文件。

  The crash binary can only be used on systems of the same architecture as
  the host build system.  There are a few optional manners of building the
  crash binary:

  崩溃二进制文件只能用于与之相同的体系结构的系统上
  主机构建系统。 有一些可选的方式来构建
  崩溃二进制：

  o  On an x86_64 host, a 32-bit x86 binary that can be used to analyze
     32-bit x86 dumpfiles may be built by typing "make target=X86".
  o  On an x86 or x86_64 host, a 32-bit x86 binary that can be used to analyze
     32-bit arm dumpfiles may be built by typing "make target=ARM".
  o  On an x86 or x86_64 host, a 32-bit x86 binary that can be used to analyze
     32-bit mips dumpfiles may be built by typing "make target=MIPS".
  o  On an ppc64 host, a 32-bit ppc binary that can be used to analyze
     32-bit ppc dumpfiles may be built by typing "make target=PPC".
  o  On an x86_64 host, an x86_64 binary that can be used to analyze
     arm64 dumpfiles may be built by typing "make target=ARM64".

​     在x86_64主机上，可以用于分析的x86_64二进制文件
​     可以通过键入“make target = ARM64”来构建arm64 dumpfiles。

  o  On an x86_64 host, an x86_64 binary that can be used to analyze
     ppc64le dumpfiles may be built by typing "make target=PPC64".

  Traditionally when vmcores are compressed via the makedumpfile(8) facility
  the libz compression library is used, and by default the crash utility
  only supports libz.  Recently makedumpfile has been enhanced to optionally
  use either the LZO or snappy compression libraries.  To build crash with
  either or both of those libraries, type "make lzo" or "make snappy".

  传统上，当通过makedumpfile（8）工具压缩vmcores时
  使用libz压缩库，默认情况下使用崩溃实用程序
  只支持libz。 最近makedumpfile已经增强到可选
  使用LZO或snappy压缩库。 与...建立崩溃
  这些库中的一个或两个，键入“make lzo”或“make snappy”。

  All of the alternate build commands above are "sticky" in that the
  special "make" targets only have to be entered one time; all subsequent
  builds will follow suit.

  上面的所有备用构建命令都是“粘性的”
  特殊的“制造”目标只需要输入一次; 随后都是
  构建将效仿。

  If the tool is run against a kernel dumpfile, two arguments are required, the
  uncompressed kernel name and the kernel dumpfile name.

  如果该工具是针对内核转储文件运行的，则需要两个参数
  未压缩的内核名称和内核转储文件名称。  

  If run on a live system, only the kernel name is required, because /dev/mem 
  will be used as the "dumpfile".  On Red Hat or Fedora kernels where the
  /dev/mem device is restricted, the /dev/crash memory driver will be used.
  If neither /dev/mem or /dev/crash are available, then /proc/kcore will be
  be used as the live memory source.  If /proc/kcore is also restricted, then
  the Red Hat /dev/crash driver may be compiled and installed; its source
  is included in the crash-7.2.6/memory_driver subdirectory.

  如果在实时系统上运行，则只需要内核名称，因为/ dev / mem
  将用作“dumpfile”。 在Red Hat或Fedora内核中
  / dev / mem设备受限制，将使用/ dev / crash内存驱动程序。
  如果/ dev / mem或/ dev / crash都不可用，则/ proc / kcore将是
  用作实时内存源。 如果/ proc / kcore也受限制，那么
  可以编译和安装Red Hat / dev / crash驱动程序; 它的来源
  包含在crash-7.2.6 / memory_driver子目录中。

  If the kernel file is stored in /boot, /, /boot/efi, or in any /usr/src
  or /usr/lib/debug/lib/modules subdirectory, then no command line arguments
  are required -- the first kernel found that matches /proc/version will be
  used as the namelist.

  如果内核文件存储在/ boot，/，/ boot / efi或任何/ usr / src中
  或/ usr / lib / debug / lib / modules子目录，然后没有命令行参数
  是必需的 - 找到匹配/ proc / version的第一个内核
  用作名单。

  For example, invoking crash on a live system would look like this:

    $ crash
    
    crash 7.2.6
    Copyright (C) 2002-2019  Red Hat, Inc.
    Copyright (C) 2004, 2005, 2006, 2010  IBM Corporation
    Copyright (C) 1999-2006  Hewlett-Packard Co
    Copyright (C) 2005, 2006, 2011, 2012  Fujitsu Limited
    Copyright (C) 2006, 2007  VA Linux Systems Japan K.K.
    Copyright (C) 2005, 2011  NEC Corporation
    Copyright (C) 1999, 2002, 2007  Silicon Graphics, Inc.
    Copyright (C) 1999, 2000, 2001, 2002  Mission Critical Linux, Inc.
    This program is free software, covered by the GNU General Public License,
    and you are welcome to change it and/or distribute copies of it under
    certain conditions.  Enter "help copying" to see the conditions.
    This program has absolutely no warranty.  Enter "help warranty" for details.
     
    GNU gdb 7.6
    Copyright 2013 Free Software Foundation, Inc.
    License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
    This is free software: you are free to change and redistribute it.
    There is NO WARRANTY, to the extent permitted by law.  Type "show copying"
    and "show warranty" for details.
    This GDB was configured as "i686-pc-linux-gnu"...
     
          KERNEL: /boot/vmlinux
        DUMPFILE: /dev/mem
            CPUS: 1
            DATE: Fri May  3 13:57:10 2019
          UPTIME: 10 days, 22:55:18
    LOAD AVERAGE: 0.08, 0.03, 0.01
           TASKS: 42
        NODENAME: ha2.mclinux.com
         RELEASE: 2.4.0-test10
         VERSION: #11 SMP Thu Nov 4 15:09:25 EST 2000
         MACHINE: i686  (447 MHz)
      MEMORY: 128 MB
             PID: 3621                                  
         COMMAND: "crash"
            TASK: c463c000  
             CPU: 0
           STATE: TASK_RUNNING (ACTIVE)
    
    crash> help
    
    *              extend         log            rd             task           
    alias          files          mach           repeat         timer          
    ascii          foreach        mod            runq           tree           
    bpf            fuser          mount          search         union          
    bt             gdb            net            set            vm             
    btop           help           p              sig            vtop           
    dev            ipcs           ps             struct         waitq          
    dis            irq            pte            swap           whatis         
    eval           kmem           ptob           sym            wr             
    exit           list           ptov           sys            q              
    
    crash version: 7.2.6    gdb version: 7.6
    For help on any command above, enter "help <command>".
    For help on input options, enter "help input".
    For help on output options, enter "help output".
    
    crash> 

  When run on a dumpfile, both the kernel namelist and dumpfile must be 
  entered on the command line.  For example, when run on a core dump created
  by the Red Hat netdump or diskdump facilities:

  在dumpfile上运行时，内核名称列表和转储文件都必须是
  在命令行输入。 例如，在创建的核心转储上运行时
  通过Red Hat netdump或diskdump工具：

    $ crash vmlinux vmcore
     
    crash 7.2.6
    Copyright (C) 2002-2019  Red Hat, Inc.
    Copyright (C) 2004, 2005, 2006, 2010  IBM Corporation
    Copyright (C) 1999-2006  Hewlett-Packard Co
    Copyright (C) 2005, 2006, 2011, 2012  Fujitsu Limited
    Copyright (C) 2006, 2007  VA Linux Systems Japan K.K.
    Copyright (C) 2005, 2011  NEC Corporation
    Copyright (C) 1999, 2002, 2007  Silicon Graphics, Inc.
    Copyright (C) 1999, 2000, 2001, 2002  Mission Critical Linux, Inc.
    This program is free software, covered by the GNU General Public License,
    and you are welcome to change it and/or distribute copies of it under
    certain conditions.  Enter "help copying" to see the conditions.
    This program has absolutely no warranty.  Enter "help warranty" for details.
     
    GNU gdb 7.6
    Copyright 2013 Free Software Foundation, Inc.
    License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
    This is free software: you are free to change and redistribute it.
    There is NO WARRANTY, to the extent permitted by law.  Type "show copying"
    and "show warranty" for details.
    This GDB was configured as "i686-pc-linux-gnu"...
    
          KERNEL: vmlinux
        DUMPFILE: vmcore
            CPUS: 4
            DATE: Tue Mar  2 13:57:09 2004
          UPTIME: 00:02:40
    LOAD AVERAGE: 2.24, 0.96, 0.37
           TASKS: 70
        NODENAME: pro1.lab.boston.redhat.com
         RELEASE: 2.6.3-2.1.214.11smp
         VERSION: #1 SMP Tue Mar 2 10:58:27 EST 2004
         MACHINE: i686  (2785 Mhz)
          MEMORY: 512 MB
           PANIC: "Oops: 0002 [#1]" (check log for details)
             PID: 0
         COMMAND: "swapper"
            TASK: 22fa200  (1 of 4)  [THREAD_INFO: 2356000]
             CPU: 0
           STATE: TASK_RUNNING (PANIC)
    
    crash> 

  The tool's environment is context-specific.  On a live system, the default
  context is the command itself; on a dump the default context will be the
  task that panicked.  The most commonly-used commands are:

  该工具的环境是特定于上下文的。 在实时系统上，默认
  context是命令本身; 在转储上，默认上下文将是
  惊慌失措的任务。 最常用的命令是：

    set     - set a new task context by pid, task address, or cpu.
    bt      - backtrace of the current context, or as specified with arguments.
    p       - print the contents of a kernel variable.
    rd      - read memory, which may be either kernel virtual, user virtual, or
              physical.
    ps      - simple process listing.
    log     - dump the kernel log_buf.
    struct  - print the contents of a structure at a specified address.
    foreach - execute a command on all tasks, or those specified, in the system.

  Detailed help concerning the use of each of the commands in the menu above 
  may be displayed by entering "help command", where "command" is one of those 
  listed above.  Rather than getting bogged down in details here, simply
  run the help command on each of the commands above.  Note that many commands
  have multiple options so as to avoid the proliferation of command names.

  有关使用上述菜单中每个命令的详细帮助
  可以通过输入“help command”来显示，其中“command”就是其中之一
  以上所列。 简单地说，不要在这里陷入细节
  在上面的每个命令上运行help命令。 请注意许多命令
  有多个选项，以避免命令名称的扩散。

  Command output may be piped to external commands or redirected to files.
  Enter "help output" for details.

  命令输出可以通过管道传输到外部命令或重定向到文件。
  输入“帮助输出”了解详细信息。

  The command line history mechanism allows for command-line recall and 
  command-line editing.  Input files containing a set of crash commands may 
  be substituted for command-line input.  Enter "help input" for details.

  命令行历史记录机制允许命令行调用和
  命令行编辑。 包含一组崩溃命令的输入文件可以
  替换命令行输入。 输入“帮助输入”了解详细信息。

  Note that a .crashrc file (or .<your-command-name>rc if the name has been 
  changed), may contain any number of "set" or "alias" commands -- see the
  help pages on those two commands for details.

  命令行历史记录机制允许命令行调用和
  命令行编辑。 包含一组崩溃命令的输入文件可以
  替换命令行输入。 输入“帮助输入”了解详细信息。

  Lastly, if a command is entered that is not recognized, it is checked
  against the kernel's list of variables, structure, union or typedef names, 
  and if found, the command is passed to "p", "struct", "union" or "whatis".
  That being the case, as long as a kernel variable/structure/union name is 
  different than any of the current commands.

  最后，如果输入的命令无法识别，则会进行检查
  针对内核的变量，结构，联合或typedef名称列表，
  如果找到，则将命令传递给“p”，“struct”，“union”或“whatis”。
  既然如此，只要内核变量/结构/联合名称是
  与任何当前命令不同。

  (1) A kernel variable can be dumped by simply entering its name:

 只需输入其名称即可转储内核变量

      crash> init_mm
      init_mm = $2 = {
        mmap = 0xc022d540, 
        mmap_avl = 0x0, 
        mmap_cache = 0x0, 
        pgd = 0xc0101000, 
        count = {
          counter = 0x6
        }, 
        map_count = 0x1, 
        mmap_sem = {
          count = {
            counter = 0x1
          }, 
          waking = 0x0, 
          wait = 0x0
        }, 
        context = 0x0, 
        start_code = 0xc0000000, 
        end_code = 0xc022b4c8,
        end_data = c0250388,
        ...

  (2) A structure or can be dumped simply by entering its name and address:  

只需输入其名称即可转储内核变量

      crash> vm_area_struct c5ba3910
      struct vm_area_struct {
        vm_mm = 0xc3ae3210, 
        vm_start = 0x821b000, 
        vm_end = 0x8692000, 
        vm_next = 0xc5ba3890, 
        vm_page_prot = {
          pgprot = 0x25
        }, 
        vm_flags = 0x77, 
        vm_avl_height = 0x4, 
        vm_avl_left = 0xc0499540, 
        vm_avl_right = 0xc0499f40, 
        vm_next_share = 0xc04993c0, 
        vm_pprev_share = 0xc0499060, 
        vm_ops = 0x0, 
        vm_offset = 0x0, 
        vm_file = 0x0, 
        vm_pte = 0x0
      }

  The crash utility has been designed to facilitate the task of adding new 
  commands.  New commands may be permanently compiled into the crash executable,
  or dynamically added during runtime using shared object files.

  崩溃实用程序旨在促进添加新任务
  命令。 新命令可以永久编译到崩溃可执行文件中，
  或在运行时使用共享对象文件动态添加。

  To permanently add a new command to the crash executable's menu:

  要永久地向崩溃可执行文件的菜单添加新命令：

    1. For a command named "xxx", put a reference to cmd_xxx() in defs.h.
       
    2. Add cmd_xxx into the base_command_table[] array in global_data.c. 
    
    3. Write cmd_xxx(), putting it in one of the appropriate files.  Look at 
       the other commands for guidance on getting symbolic data, reading
       memory, displaying data, etc...
    
    4. Recompile and run.

  Note that while the initial compile of crash, which configures and compiles
  the gdb module, takes several minutes, subsequent re-compiles to do such
  things as add new commands or fix bugs just takes a few seconds.

  注意，虽然初始编译崩溃，它配置和编译
  gdb模块，需要几分钟，随后重新编译才能这样做
  添加新命令或修复错误只需几秒钟。

  Alternatively, you can create shared object library files consisting of
  crash command extensions, that can be dynamically linked into the crash
  executable during runtime or during initialization.  This will allow
  the same shared object to be used with subsequent crash releases without
  having to re-merge the command's code into each new set of crash sources.
  The dynamically linked-in commands will automatically show up in the crash
  help menu.  For details, enter "help extend" during runtime, or enter
  "crash -h extend" from the shell command line.

  或者，您可以创建包含的共享库文件
  崩溃命令扩展，可以动态链接到崩溃中
  在运行时或初始化期间可执行 这将允许
  没有用于后续崩溃版本的相同共享对象
  必须将命令的代码重新合并到每组新的崩溃源中。
  动态链接的命令将自动显示在崩溃中
  帮助菜单。 有关详细信息，请在运行时输入“help extend”，或输入
  shell命令行中的“crash -h extend”。

