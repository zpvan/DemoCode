#!/bin/bash
debugfs=/sys/kernel/debug
echo nop > $debugfs/tracing/current_tracer
echo 0 > $debugfs/tracing/tracing_on
echo $$ > $debugfs/tracing/set_ftrace_pid
echo function_graph > $debugfs/tracing/current_tracer
<span style="color:#ff0000;">#replace KERNEL_FUNCTION by your function name</span>
echo KERNEL_FUNCTION > $debugfs/tracing/set_graph_function
echo 1 > $debugfs/tracing/tracing_on
<span style="color:#ff0000;">exec "$@"</span>
