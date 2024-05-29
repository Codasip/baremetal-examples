# exception-demo

Demo configures a custom handler for ilegal instruction exception,
that prints out information about the trap cause. First, an ilegal
instruction is executed with the custom handler configured, then
the configuration is removed to run the default exception handler.
