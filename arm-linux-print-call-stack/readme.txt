arm-linux-gcc default enable less options than X86,
so X86 gcc default support backtrace_symbols_fd(),
but arm-linux-gcc need to explicitly use -funwind-tables -fasynchronous-unwind-tables 
so that can support backtrace_symbols_fd().


Ref:
https://stackoverflow.com/questions/31528824/no-backtrace-from-sigabrt-signal-on-arm-platform
https://stackoverflow.com/questions/11665829/how-can-i-print-stack-trace-for-caught-exceptions-in-c-code-injection-in-c