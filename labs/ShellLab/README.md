# Shell Lab

This is the students' first introduction to application level concurrency, and
gives them a clear idea of Unix process control, signals, and signal handling.

## Prepare

[WriteUp](http://csapp.cs.cmu.edu/3e/shlab.pdf)

```bash
man 2 fork
man 2 execve
man 3 exit
man 7 signal
man 2 sigsuspend
man 3 sigsetops
```

job list helper functions:
* `clearjob`: Clear the entries in a job struct
* `initjobs`: Initialize the job list

* `listjobs`: Print the job list

* `addjob`: Add a job to the job list
* `deletejob`: Delete a job whose PID=pid from the job list

* `getjobjid`: Find a job (by JID) on the job list
* `getjobpid`: Find a job (by PID) on the job list

* `maxjid`: Returns largest allocated job ID

* `fgpid`: Return PID of current foreground job, 0 if no such job

* `pid2jid`: Map process ID to job ID

What we gonna implement:
* `ctrl-c`: cause `SIGINT` signal to terminate the process to foreground job
* `ctrl-z`: cause `SIGTSTP` signal to place a process in the stopped state to
  the foreground job, which remains until awakened by `SIGCONT` signal
* `jobs`: List the running and stopped background jobs.
* `bg <job>`: Change a stopped background job to a running background job.
* `fg <job>`: Change a stopped or running background job to a running in the foreground.
* `quit`: Terminate the shell.

functions we will implement:
* `eval`: Main routine that parses and interprets the command line
* `builtin_cmd`: Recognizes and interprets the built-in commands: quit, fg, bg, and jobs.
* `do_bgfg`: Implements the bg and fg built-in commands.
* `waitfg`: Waits for a foreground job to complete.
* `sigchld_handler`: Catches SIGCHILD signals.
* `sigint_handler`: Catches SIGINT (ctrl-c) signals.
* `sigtstp_handler`: Catches SIGTSTP (ctrl-z) signals.

## trace01

It is a EOF test, which is handled by `main()`. Make sure the following 2 commands'
outputs are the same.

```bash
make test01

make rtest01
```
