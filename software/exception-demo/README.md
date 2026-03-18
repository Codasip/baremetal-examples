# exception-demo

Demo configures a custom handler for illegal instruction exception,
that prints out information about the trap cause. First, an illegal
instruction is executed with the custom handler configured, then
the configuration is removed to run the default exception handler.
