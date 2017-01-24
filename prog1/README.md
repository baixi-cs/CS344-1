### OSU CS344 Program 1 
#### student: grantjo
---

##### *Introduction*
This assignment asks you to write a bash shell script to compute statistics.  The purpose is to get you familiar with the Unix shell, shell programming, Unix utilities, standard input, output, and error, pipelines, process ids, exit values, and signals.

What you’re going to submit is your script, called simply "stats".

##### *Overview*
In this assignment, you will write a bash shell script to calculate mean averages and medians of numbers that can be input to your script from either a file or via stdin. This is the sort of calculation I might do when figuring out the grades for this course. The input will have whole number values separated by tabs, and each line will have the same number of values (for example, each row might be the scores of a student on assignments). Your script should be able to calculate the mean and median across the rows (like I might do to calculate an individual student's course grade) or down the columns (like I might do to find the average score on an assignment).

You will probably need commands like these, so please read up on them: <code>while</code>, <code>cat</code>, <code>read</code>, <code>expr</code>, <code>cut</code>, <code>head</code>, <code>tail</code>, <code>wc</code>, and <code>sort</code>.

Your script must be called simply "stats". The general format of the stats command is:
```bash
stats {-rows|-cols} [input_file]
```
##### *Specifications* 
You must check for the right number and format of arguments to stats. You should allow users to abbreviate -rows and -cols; any word beginning with a hyphen and then a lowercase r is taken to be rows and any word beginning with a hyphen and then a lowercase c is taken to be cols.  So, for example, you would get mean averages and medians across the rows with -r, -rowwise and -rumplestiltskin, but not -Rows.  If the command has too many or two few arguments or if the arguments of the wrong format you should output an error message to standard error.  You should also output an error message to stderr if an input file is specified, but is not readable.

You must output the statistics to stdout in the format shown above.  Be sure all error messages are sent to stderr, including the "usage" returned above when someone doesn't specify the correct parameters to stats. If a specified input file is empty, this is not an error: do not output any numbers or statistics. In this event, either send an informational message to stderr and exit, or just exit. If there are any errors of any kind (remember an empty input file is not an error), then the exit status should be set to 1; if the stats program runs successfully, then the exit value should be 0.

Your stats program should be able to handle data with any reasonable number of rows or columns; however you can assume that each row will be less than 1000 bytes long (because Unix utilities assume that input lines will not be too long), but don't make any assumptions about the number of rows. Think about where in your program the size of the input matters. You can assume that all rows will have the same number of values; you do not have to do any error checking on this.

Though optional, I do recommend that you use temporary files; arrays are not recommended. For this assignment, any temporary files you use should be put in the current working directory. (A more standard place for temporary files is in /tmp but don't do that for this assignment; it makes grading easier if they are in the current directory.) Be sure any temporary file you create uses the process id as part of its name, so that there will not be conflicts if the stats program is running more than once. Be sure you remove any temporary files when your stats program is done. You should also use the trap command to catch interrupt, hangup, and terminate signals to remove the temporary files if the stats program is terminated unexpectedly.

All values and results are and must be whole numbers. You may use the <code>expr</code> command to do your calculations, or any other bash shell scripting method. Do not use any other languages other than bash shell scripting. Note that <code>expr</code> only works with whole numbers. When you calculate the average you must round to the nearest whole number, where half values round up (i.e. 7.5 rounds up to 8). This is the most common form of rounding. When doing truncating integer division, this formula works to divide two numbers and end up with the proper rounding:
```
(a + (b/2)) / b
```
You can learn more about rounding methods here (see Half Round Up):

http://www.mathsisfun.com/numbers/rounding-methods.html (Links to an external site.)

To calculate the median, sort the values and take the middle value. For example, the median of 97, 90, and 83 is 90.  The median of 97, 90, 83, and 54 is still 90 - when there are an even number of values, choose the larger of the two middle values.

##### *Grading With a Script* 
p1gradingscript is used for testing the stats program:

This script is the very one that will be used to assign your script a grade. To use the script, just place it in the same directory as your stats script and run it like this:

```bash
$ p1gradingscript
```

When we run your script for grading, we will do this to put your results into a file we can examine more easily:
```bash
$ p1gradingscript > p1results.username 2>&1
```
