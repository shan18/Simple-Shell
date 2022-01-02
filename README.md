# Simple Shell

This is a very simplified version of the Unix shell.

## Compiling and Usage

To compile the project, run the command

```
$ make
```

To start using the shell, run the executable file generated after compiling

```
$ ./shell
```

## Supported Features

The shell supports the following features.

### Built-in Commands

The shell contains four built-in commands

- **`cd [dir]`**: For changing directories.
- **`exit`**: To terminate the shell.
- **`jobs`**: Prints a list of currently suspended jobs to `STDOUT`.
- **`fg [index]`**: Resumes a job in the foreground.

More details about the `jobs` and the `fg` commands are given below.

### Running External Programs

An external program can be specified by either an **absolute path**, a **relative path**, or **base name only**.

If only the base name of the command is specified, the shell searches for the program under `/bin` and `/usr/bin` (in such order). For example, if the user enters `ls`, the shell first tries `/bin/ls`, if that fails then `/usr/bin/ls` is tried.

### I/O Redirection

#### Input Redirection

The **standard input** (`STDIN`) can be redirected using `<`. For example:

```
[user dir]$ cat < input.txt
```

#### Output Redirection

The **standard output** (`STDOUT`) can be redirected using `>` and `>>`. If the output file already exists, redirecting with `>` will **overwrite** the file, whereas `>>` will **append** to the existing file. For example:

```
[user dir]$ ls -l > output.txt
[user dir]$ ls -l >> output.txt
```

#### Pipe

A pipe (`|`) can be used to connect the standard output of the first program to the standard input of the second program. For example:

```
[user dir]$ cat shell.c | grep main | less
```

### Suspending and Resuming Processes

If a child process (_job_) has been suspended via `Ctrl-Z` or a `SIGTSTP` or `SIGSTOP` signal, the shell will keep a record of it so that the child can be resumed later.

To check the list of currently suspended jobs, the `jobs` command can be used. It prints the information in the following format: `[index] command`. For example

```
[user dir]$ jobs
[1] cat
[2] top | cat
[3] cat > output.txt
[user dir]$ _
```

The list printed above is sorted by the time each job is suspended (oldest first).

To resume a suspended job, the `fg` command can be used. It takes exactly one argument: the job index, which is the number in the bracket printed by the `jobs` command. For example

```
[user dir]$ jobs
[1] cat
[2] top | cat
[3] cat > output.txt
[user dir]$ fg 2
```

This command would resume `top | cat` in the foreground.

## Contact/Getting Help

If you need any help or want to report a bug, raise an issue in the repo.
