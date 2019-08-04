#!/bin/bash
debugfs=/sys/kernel/debug
echo nop > $debugfs/tracing/current_tracer
echo 0 > $debugfs/tracing/tracing_on
echo $$ > $debugfs/tracing/set_ftrace_pid
echo function_graph > $debugfs/tracing/current_tracer
<span style="color:#ff0000;">#replace test_function by your function name
echo test_function > $debugfs/tracing/set_graph_function</span>
echo 1 > $debugfs/tracing/tracing_on
<span style="color:#ff0000;">exec "$@"</span>