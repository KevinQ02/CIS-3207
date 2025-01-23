# Project 1

## Unix/Linux Utilities

**Before beginning:** Read this [lab tutorial](http://pages.cs.wisc.edu/~remzi/OSTEP/lab-tutorial.pdf); it has some useful tips for programming in the C environment.

In this project, you'll build a UNIX utility, a simple version of a commonly used command, **cp**. This program can be used from the Linux command line. We'll call the program with a slightly different name from its Linux counterpart to avoid confusion; for example, instead of **cp**, you'll be implementing **tucp** .

**Objectives:**

* Re-familiarize yourself with the C programming language
* Re-familiarize yourself with a the basics of the shell / terminal / command-line of Linux
* Learn (as a side effect) how to use a proper code editor such as emacs, or vi, or…
* Learn a little about how Linux utilities are implemented
* Begin to learn about and use system calls – i.e, an introduction to system programming

⠀
While the project focuses on writing a simple C program, you can see from the above that even doing so requires a bunch of other previous knowledge, including a basic idea of what a shell is and how to use the command line on some UNIX-based systems (e.g., Linux or macOS), how to use an editor such as emacs, and of course a basic understanding of C programming. If you **do not** already have these skills or knowledge, you should do a refresh of C programming before starting the project (there many resources for this including those available in LinkedIn Learning via TUPortal)

Summary of what gets turned in:

* The source, include and make files for the utility described below: **tucp**
* The project should compile successfully when compiled with the **-Wall** and **-Werror** flags.
* The project should (hopefully) meet the requirements we will discuss.

⠀
**Tips**

**Start small, and get things working incrementally.**

**Testing is critical.** A great programmer we once knew said you have to write five to ten lines of test code for every line of code you produce; testing your code to make sure it works is crucial. Write tests to see if your code handles all the cases you think it should. Be as comprehensive as you can be. Of course, when grading your projects, we will be. Thus, it is better if you find your bugs first, before we do. Think of how you will test when you are designing and writing your code.

**Design First**. Think about the program requirements. Break down the problem into components. Consider how data will be stored (what are the appropriate data structures?)

**tucp**

The utility you will build is called **tucp**, a variant of the Linux tool **cp**. In this project, this command is used to copy files or a group of files. It creates an exact image (copy) of a file found on a disk. The **tucp** command requires at least two filenames in its arguments. 

The following are the possible command usages:

Prompt>**./tucp** Sourcefile Destinationfile

Prompt>**./tucp** Sourcefile Directory

Prompt>**./tucp** Sourcefile-1 Sourcefile-2 Sourcefile-3 Sourcefile-n Directory

The "**./**" before the **tucp** above is a UNIX/Linux thing; it just tells the system in which directory to find the program **tucp** (in this case, in the "." (dot) directory, which means the *current working directory*).

**Details**

·       If the **tucp** command contains two “file names”, then it copies the contents of the 1st file to the 2nd file. If the 2nd file doesn’t exist, then it first creates a file with the specified name and the content of the first file is copied to it. But if the second file already exists, then it is simply overwritten without any warning.  

·       If the **tucp** command has one or more arguments specifying source file names and following those arguments, an argument specifying a directory name, then the command copies each source file to the destination directory with the same source file name. The destination file is created if did not exist, or it is overwritten if it already existed.

·       The Linux function *stat()* can be used to determine information about a file. The *stat struct* returned by the function includes a field, *st_mode* , that can be used to determine if the path evaluated by *stat()* is a file or a directory. Use *man* to learn about the functions *stat()* and *inode()*.

·       Any type of file, i.e., any content type in the file, should be able to be copied by your **tucp**.

You'll need to learn how to use a few library functions from the C standard library (often called **libc**) to implement the source code for this program, which we'll assume is in a file called **tucp.c**. All C code is automatically linked with the C library, which is full of useful functions you can call to implement your program. Learn more about the C library [here](https://en.wikipedia.org/wiki/C_standard_library) and perhaps [here](https://www.gnu.org/software/libc/manual/html_node/index.html).

At the end of this document is a discussion about some system library functions that may be useful in this project and the extra credit project.

To create the **tucp** executable, you'll be creating a single source file, **tucp.c**, and writing a little C code to implement this simplified version of **tucp**. To compile this program, you will do the following:

prompt> gcc -o tucp tucp.c -Wall -Werror

prompt>

This will make a single *executable binary* called **tucp**which you can then run as above.

## Additional Tips

**Testing is critical.** As noted above, including sufficient code to test your program code to make sure it works is crucial. [This is a good time to learn to **use a debugger**]. Write tests to see if your code handles all the cases you think it should. Be as comprehensive as you can be. Of course, when grading your projects, we will be. Thus, it is better if you find your bugs first, before we do.

**Keep old versions around.**  Experienced developers use version control systems such as git (e.g.,through GitHub); We will be using GitHub beginning with this project. A repository will be created for your use. That is, **we will create a private GitHub repository for your use for each project** assignment going forward. **You are not to use your own personal GitHub account.**

**Footnotes**

[1](): Unfortunately, there is a lot to learn about the C library, but at some point, you've just got to **read documentation** to learn what is available. Why not now, when you are young? Or, if you are old, why not now, before it's ... ahem ... too late? We will use C library functions and system calls (including from other libraries) in all our projects.

## Project 1 Grading  **5 Points**    

tucp – correctly parses the command line into source and destination components (1 pt)

tucp - correctly copies 1 source file to a destination file (1 pt)

tucp – correctly copies a source file to a specified directory (1 pt)

tucp - correctly copies multiple source files to a specified directory. (1.5 pt)

tucp- correctly uses GitHub in the development process (0.5 pts)

No Demo for: - 30%

Does not compile and run: -15%

***

# Extra Credit

## tuls

The program **tuls** is a simple Linux utility program. Generally, it lists information about the FILES in a directory (folder). By default, **tuls** lists the files in the current directory. To invoke the program the user types:
```
prompt> **./tuls** 
  lscmd.c
  a.out
  ..
  .

or,
prompt> **./tuls direc   <**where **direc** is a directory path**>**
[[direc]]
  ->direc_1_file
  ->direc_2_file
->local_1_file
->local_2_file
->local_3_file
..
.
```


·      When there is no additional parameter for the **tuls** command, just produce the list of the files and directories in the current directory, as shown above.

·      When a directory path is included with the **tuls** command, then the **tuls** command prints the list of files and directories in the specified directory path argument, and the list of files in the current directory. 

To create the **tuls** executable, you'll be creating a single source file, **tuls.c**, and writing a little C code to implement this simplified version of **tuls**. To compile this program, you will do the following:

prompt> gcc -o tuls tuls.c -Wall -Werror

prompt>

This will make a single *executable binary* called **tuls** which you can then run as above.

You'll need to learn how to use a few library functions from the C standard library (often called **libc**) to implement the source code for this program, which we'll assume is in a file called **tuls.c**. All C code is automatically linked with the C library, which is full of useful functions you can call to implement your program. Learn more about the C library [here](https://en.wikipedia.org/wiki/C_standard_library) and perhaps [here](https://www.gnu.org/software/libc/manual/html_node/index.html).

**Details**

* Your program **tuls** can be invoked with zero or one argument on the command line; with 1 argument (a directory name), print the list of files in the specified directory then print the directories found within the specified directory. Then print the list of files in the current directory where the command was executed.
* In all non-error cases, **tuls** should exit with status code 0, usually by returning a 0 from **main()** (or by calling **exit(0)**).
* If no *arguments* are specified on the command line, **tuls** should just print the names of the entries (files and directories) in the current directory.
* If the program tries and fails to **open or scan a directory**, it should print the exact message "tuls: cannot open directory" (followed by a newline) and exit with status code 1.

⠀
 
## Grading for Project 1 Extra Credit 

**tuls** 3 Points    

tuls – correctly uses the library functions scandir(), opendir(), readdir() (1 pt)

tuls – correctly identifies directories vs files (0.5 pts)

tuls - correctly lists the current directory contents (1 pt)

tuls - correctly uses GitHub in the development process (0.5 pts)

No Demo for: - 30%

Does not compile and run: -15%

## Some System Library Functions

For this project, we recommend using the following routines to do operations on directories: **scandir()**, **opendir()**, **readdir(),** and **closedir().**Whenever you use a new function such as these, the first thing you should do is **read about it** -- how else will you learn to use it properly?

On UNIX/Linux systems, the best way to read about such functions is to use what are called the **man** pages (short for **manual**). In our HTML/web-driven world, the man pages feel a bit antiquated, but they are useful and informative and generally quite easy to use.

To access the man page for **scandir()**, for example, just type the following at your Linux shell prompt:

prompt> man scandir

Then, **read!** Reading man pages effectively takes practice; why not start learning now?

We will also give a simple overview here. The **scandir()** function scans a directory for matching entries and returns the number of directory entries found.

Here is a typical usage of **scandir()**:

struct dirent **namelist;

int n;

n=scandir(".",&namelist,NULL,alphasort);

printf("Number Files %d\n", n);

if (n == -1) {

perror("scandir");

exit(EXIT_FAILURE);

}

while (n--)

{

printf("%s\n",namelist[n]->d_name);

free(name[n]);

}

A couple of points here. First, note that **scandir()** takes multiple arguments: the *directory to search, an array to store the found entries, a ‘filter’ for the files to return, and a parameter specifying how to sort the found entries*. Read the man pages to see how to use the arguments.

Second, note the *critical* checking of whether the **scandir()** actually succeeded. This is not Java where an exception will be thrown when things go wrong; rather, it is C, and it is expected in good programs, (i.e., the only kind you'd want to write) that you always will check if the function call succeeded. Reading the man page tells you the details of what is returned when an error is encountered; in this case, the Linux man page says:

```
The scandir() function returns the number of directory entries selected. On error, -1 is returned, with errno set to indicate the error.
```

Thus, as the code above does, please check that **scandir()** does not return -1 before trying to use the return value.

Third, note that when the error case occurs, the program prints a message and then exits with error status of 1. In UNIX systems, it is traditional to return 0 upon success, and non-zero upon failure. Here, we will use 1 to indicate failure.

Side note: if **scandir()** does fail, there are many reasons possible as to why. You can use the functions **perror()** or **strerror()** to print out more about *why* the error occurred; learn about those on your own (using ... you guessed it ... the man pages!).

To print out directory contents, just use **printf()**. For example, after reading the directory entry names with **scandir()** into an array named **namelist**, you can just print out individual entry names as follows:

printf("%s\n", namelist[i]->d_name);

The ‘->’symbol is used to specify a component of the structure of the namelist entry, in this case one that has the entry name. Look in the system include file *<dirent.h>* to explore the contents of the *dirent* structure. 

Note that inclusion of a newline (\n) character to the printf(), because the *d-name* entry is just a text string with the entry name.
