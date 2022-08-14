# Shell Lab

This is the students' first introduction to application level concurrency, and
gives them a clear idea of Unix process control, signals, and signal handling.

**Need to take [Lecture 15](https://www.bilibili.com/video/BV1iW411d7hd?p=15)
before starting this lab.**

## Prepare

[WriteUp](http://csapp.cs.cmu.edu/3e/shlab.pdf) &
[Signal Blog](https://github.com/huang-feiyu/Learning-Space/tree/main/Tech-Blog/Signal)

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

> Properly terminate on EOF.

It is a EOF test, which has been handled by `main()`. Make sure the following 2
commands' outputs are the same.

```bash
make test01

make rtest01
```

## trace02

> Process builtin quit command.

Take a look at CSAPP textbook 8.4.6. It's easy to implement.

## trace03 & trace04

> Run a foreground/background job.

The two traces need not to **really** implement the `fg` and `bg` built-in commands.
We just need to fork a child process and run the executable file via `execve()`.

## trace05

> Process jobs builtin command.

**Take a look at CSAPP textbook 8.5.6 & 8.5.7**.

We need to use the job list API to implement `jobs`. If do not care about the
signal race condition, we just need to add `addjob()` to `eval()`.

`deletejob()` is a little complicated:
1. background process terminates: `sigchld_handler()`; Use exactly one call to
`waitpid()`
2. foreground process terminates: `waitfg()`; Hang via busy `sleep()`

To avoid race condition: Add `sigprocmask()` in `eval()` to block `SIGCHLD`
signals to avoid `deletejob()` before `addjob()`.

## trace06 & trace07 & trace08

> Forward SIGINT/SIGTSTP only to foreground job.

**Take a look at CSAPP textbook 8.4.3**.

Fill the handlers and add process to `sigchld_handler()` to reap child.

## trace09 & trace10

> Process bg/fg builtin command.

* The *bg* <job> command restarts <job> by sending it a `SIGCONT` signal, and
then runs it in the background. The <job> argument can be either a PID or a JID.
* The *fg* <job> command restarts <job> by sending it a `SIGCONT` signal, and
then runs it in the foreground. The <job> argument can be either a PID or a JID.

1. get <job> according to argument & validate <job>
2. send `SIGCONT` if valid
3. do stuff according to *bg* or *fg*
