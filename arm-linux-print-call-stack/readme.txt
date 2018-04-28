arm-linux-gcc default enable less options than X86,
so X86 gcc default support backtrace_symbols_fd(),
but arm-linux-gcc need to explicitly use -funwind-tables -fasynchronous-unwind-tables 
so that can support backtrace_symbols_fd().