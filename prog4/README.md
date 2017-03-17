verview

In this assignment, you will be creating five small programs that encrypt and decrypt information using a one-time pad-like system. I believe that you will find the topic quite fascinating: one of your challenges will be to pull yourself away from the stories of real-world espionage and tradecraft that have used the techniques you will be implementing.

These programs serve as a capstone to what you have been learning in this course, and will combine the multi-processing code you have been learning with socket-based inter-process communication. Your programs will also accessible from the command line using standard UNIX features like input/output redirection, and job control. Finally, you will write a short compilation script.

## Specifications

All execution, compiling, and testing of this program should ONLY be done in the bash prompt on our class server!

Use the following link as your primary reference on One-Time Pads (OTP):

[<span style="text-decoration: underline;"><span style="color: #0563c1; text-decoration: underline;">http://en.wikipedia.org/wiki/One-time_pad</span></span>](http://en.wikipedia.org/wiki/One-time_pad)

The following definitions will be important:

**Plaintext** is the term for the information that you wish to encrypt and protect. It is human readable.

**Ciphertext** is the term for the plaintext after it has been encrypted by your programs. Ciphertext is not human-readable, and in fact cannot be cracked, if the OTP system is used correctly.

A **Key** is the random sequence of characters that will be used to convert Plaintext to Ciphertext, and back again. It must not be re-used, or else the encryption is in danger of being broken.

The following excerpt from this Wikipedia article was captured on 2/21/2015:

<span style="color: #993300;">"Suppose Alice wishes to send the message "HELLO" to Bob. Assume two pads of paper containing identical random sequences of letters were somehow previously produced and securely issued to both. Alice chooses the appropriate unused page from the pad. The way to do this is normally arranged for in advance, as for instance 'use the 12th sheet on 1 May', or 'use the next available sheet for the next message'.</span>

<span style="color: #993300;">The material on the selected sheet is the key for this message. Each letter from the pad will be combined in a predetermined way with one letter of the message. (It is common, but not required, to assign each letter a numerical value, e.g., "A" is 0, "B" is 1, and so on.)</span>

<span style="color: #993300;">In this example, the technique is to combine the key and the message using modular addition. The numerical values of corresponding message and key letters are _added_ together, modulo 26\. So, if key material begins with "XMCKL" and the message is "HELLO", then the coding would be done as follows:</span>

<div>

<pre> <span style="color: #993300;">H       E       L       L       O  message
   7 (H)   4 (E)  11 (L)  11 (L)  14 (O) message
+ 23 (X)  12 (M)   2 (C)  10 (K)  11 (L) key
= 30      16      13      21      25     message + key
=  4 (E)  16 (Q)  13 (N)  21 (V)  25 (Z) message + key (mod 26)
      E       Q       N       V       Z   ciphertext</span></pre>

</div>

<span style="color: #993300;">If a number is larger than 26, then the remainder, after _subtraction_ of 26, is taken [as the result]. This simply means that if the computations "go past" Z, the sequence starts again at A.</span>

<span style="color: #993300;">The ciphertext to be sent to Bob is thus "EQNVZ". Bob uses the matching key page and the same process, but in reverse, to obtain the plaintext. Here the key is _subtracted_ from the ciphertext, again using modular arithmetic:</span>

<div>

<pre> <span style="color: #993300;">E       Q       N       V       Z  ciphertext
    4 (E)  16 (Q)  13 (N)  21 (V)  25 (Z) ciphertext
-  23 (X)  12 (M)   2 (C)  10 (K)  11 (L) key
= -19       4      11      11      14     ciphertext - key
=   7 (H)   4 (E)  11 (L)  11 (L)  14 (O) ciphertext - key (mod 26)
       H       E       L       L       O   message</span></pre>

</div>

<span style="color: #993300;">Similar to the above, if a number is negative then 26 is _added_ to make the number zero or higher.</span>

<span style="color: #993300;">Thus Bob recovers Alice's plaintext, the message "HELLO". Both Alice and Bob destroy the key sheet immediately after use, thus preventing reuse and an attack against the cipher."</span>

Your program will encrypt and decrypt plaintext into ciphertext, using a key, in exactly the same fashion as above, except it will be using modulo 27 operations: your 27 characters are the 26 capital letters, and the space character ( ). All 27 characters will be encrypted and decrypted as above.

To do this, you will be creating five small programs in C. Two of these will function like "daemons" (but aren't actually daemons), and will be accessed using network sockets. Two will use the daemons to perform work, and the last is a standalone utility.

Your programs must use the network calls we've discussed in class (<span style="font-family: Courier New;">send()</span>, <span style="font-family: Courier New;">recv()</span>, etc.) to send and receive sequences of bytes for the purposes of encryption and decryption by the appropriate daemons. The whole point is to use the network, even though for testing purposes we're using the same machine: if you just <span style="font-family: Courier New;">open()</span> the datafiles from the server without using the network calls, you'll receive 0 points on the assignment.

Here are the specifications of the five programs:

<span style="font-size: 12pt;">**<span style="font-family: Courier New;">otp_enc_d:</span>**</span> This program will run in the background as a daemon. Upon execution, <span style="font-family: Courier New;">otp_enc_d</span> must output an error if it cannot be run due to a network error, such as the ports being unavailable. Its function is to perform the actual encoding, as described above in the Wikipedia quote. This program will listen on a particular port/socket, assigned when it is first ran (see syntax below). When a connection is made, <span style="font-family: Courier New;">otp_enc_d</span> must call <span style="font-family: Courier New;">accept()</span> to generate the socket used for actual communication, and then use a separate process to handle the rest of the transaction (see below), which will occur on the newly accepted socket.

In the child process of <span style="font-family: Courier New;">otp_enc_d</span>, it must first check to make sure it is communicating with <span style="font-family: Courier New;">otp_enc</span> (see <span style="font-family: Courier New;">otp_enc</span>, below). After verifying that the connection to <span style="font-family: Courier New;">otp_enc_d</span> is coming from <span style="font-family: Courier New;">otp_enc</span>, then this child receives from <span style="font-family: Courier New;">otp_enc</span> plaintext and a key via the communication socket (not the original listen socket). The <span style="font-family: Courier New;">otp_enc_d</span> child will then write back the ciphertext to the <span style="font-family: Courier New;">otp_enc</span> process that it is connected to via the same communication socket. Note that the key passed in must be at least as big as the plaintext.

Your version of <span style="font-family: Courier New;">otp_enc_d</span> must support up to five _concurrent_ socket connections. Again, only in the child process will the actual encryption take place, and the ciphertext be written back.

In terms of creating that child process as described above, you may either create with <span style="font-family: Courier New;">fork()</span> a _new_ process every time a connection is made, _or_ set up a pool of five processes at the beginning of the program, before connections are allowed, to handle your encryption tasks. Your system must be able to do five separate encryptions at once, using either method you choose.

Use this syntax for <span style="font-family: Courier New;">otp_enc_d</span>:

<pre>otp_enc_d _listening_port_</pre>

_listening_port_ is the port that <span style="font-family: Courier New;">otp_enc_d</span> should listen on. You will always start <span style="font-family: Courier New;">otp_enc_d</span> in the background, as follows (the port 57171 is just an example; yours should be able to use any port):

<pre>$ otp_enc_d 57171 &</pre>

In all error situations, this program must output errors as appropriate (see grading script below for details), but should not crash or otherwise exit, unless the errors happen when the program is starting up (i.e. are part of the networking start up protocols like bind()). That is, if given bad input, once running, <span style="font-family: Courier New;">otp_enc_d</span> should recognize the bad input, report an error to the screen, and continue to run. All error text must be output to _stderr_.

This program, and the other 3 network programs, should use "localhost" as the target IP address/host. This makes them use the actual computer they all share as the target for the networking connections.

<span style="font-size: 12pt;">**<span style="font-family: Courier New;">otp_enc:</span>**</span> This program connects to <span style="font-family: Courier New;">otp_enc_d</span>, and asks it to perform a one-time pad style encryption as detailed above. By itself, <span style="font-family: Courier New;">otp_enc</span> doesnt do the encryption - <span style="font-family: Courier New;">otp_end_d</span> does. The syntax of <span style="font-family: Courier New;">otp_enc</span> is as follows:

<pre>otp_enc _plaintext key port_</pre>

In this syntax, _<span style="font-family: Courier New;">plaintext</span>_ is the name of a file in the current directory that contains the plaintext you wish to encrypt. Similarly, _<span style="font-family: Courier New;">key</span>_ contains the encryption key you wish to use to encrypt the text. Finally, _<span style="font-family: Courier New;">port</span>_ is the port that <span style="font-family: Courier New;">otp_enc</span> should attempt to connect to <span style="font-family: Courier New;">otp_enc_d</span> on.

When <span style="font-family: Courier New;">otp_enc</span> receives the ciphertext back from <span style="font-family: Courier New;">otp_enc_d</span>, it should output it to _stdout_. Thus, <span style="font-family: Courier New;">otp_enc</span> can be launched in any of the following methods, and should send its output appropriately:

<pre>$ otp_enc myplaintext mykey 57171  
$ otp_enc myplaintext mykey 57171 > myciphertext  
$ otp_enc myplaintext mykey 57171 > myciphertext &</pre>

If <span style="font-family: Courier New;">otp_enc</span> receives key or plaintext files with bad characters in them, or the key file is shorter than the plaintext, it should exit with an error, and set the exit value to 1\. This character validation can happen in either <span style="font-family: Courier New;">otp_enc</span> or <span style="font-family: Courier New;">otp_enc_d</span>, your choice. If <span style="font-family: Courier New;">otp_enc</span> cannot find the port given, it should report this error to the screen (not into the plaintext or ciphertext files) with the bad port, and set the exit value to 2\. Otherwise, on successfully running, <span style="font-family: Courier New;">otp_enc</span> should set the exit value to 0.

<span style="font-family: Courier New;">otp_enc</span> should NOT be able to connect to <span style="font-family: Courier New;">otp_dec_d</span>, even if it tries to connect on the correct port - you'll need to have the programs reject each other. If this happens, <span style="font-family: Courier New;">otp_enc</span> should report the rejection and then terminate itself.

<span>All error text must be output to _stderr_.</span>

<span style="font-size: 12pt;">**<span style="font-family: Courier New;">otp_dec_d:</span>**</span> This program performs exactly like <span style="font-family: Courier New;">otp_enc_d</span>, in syntax and usage. In this case, however, <span style="font-family: Courier New;">otp_dec_d</span> will decrypt ciphertext it is given, using the passed-in ciphertext and key. Thus, it returns plaintext again to <span style="font-family: Courier New;">otp_dec</span>.

<span style="font-size: 12pt;">**<span style="font-family: Courier New;">otp_dec:</span>**</span> Similarly, this program will connect to <span style="font-family: Courier New;">otp_dec_d</span> and will ask it to decrypt ciphertext using a passed-in ciphertext and key, and otherwise performs exactly like <span style="font-family: Courier New;">otp_enc</span>, and must be runnable in the same three ways. <span style="font-family: Courier New;">otp_dec</span> should NOT be able to connect to <span style="font-family: Courier New;">otp_enc_d</span>, even if it tries to connect on the correct port - you'll need to have the programs reject each other, as described in <span style="font-family: Courier New;">otp_enc</span>.

<span style="font-size: 12pt;">**<span style="font-family: Courier New;">keygen:</span>**</span> This program creates a key file of specified length. The characters in the file generated will be any of the 27 allowed characters, generated using the standard UNIX randomization methods. Do not create spaces every five characters, as has been historically done. Note that you specifically do not have to do any fancy random number generation: were not looking for cryptographically secure random number generation! <span style="font-family: Courier New;">rand()</span> is just fine. The last character keygen outputs should be a newline. <span>All error text must be output to _stderr_, if any.</span>

The syntax for <span style="font-family: Courier New;">keygen</span> is as follows:

<pre>keygen _keylength_</pre>

Where _<span style="font-family: Courier New;">keylength</span>_ is the length of the key file in characters. <span style="font-family: Courier New;">keygen</span> outputs to stdout. Here is an example run, which redirects stdout to a key file of 256 characters called "mykey" (note that mykey is 257 characters long because of the newline):

<pre>$ keygen 256 > mykey</pre>

**Files and Scripts**

You are provided with 5 plaintext files to use ([one](/courses/1616246/files/66087271/download?wrap=1 "plaintext1"), [two](/courses/1616246/files/66087270/download?wrap=1 "plaintext2"), [three](/courses/1616246/files/66087273/download?wrap=1 "plaintext3"), [four](/courses/1616246/files/66087274/download?wrap=1 "plaintext4"), [five](/courses/1616246/files/66087272/download?wrap=1 "plaintext5")). The grading will use these specific files; do not feel like you have to create others.

You are also provided with a grading script ("[p4gradingscript](/courses/1616246/files/66087254/download?wrap=1 "p4gradingscript")") that you can run to test your software. If it passes the tests in the script, and has sufficient commenting, it will receive full points (see below). EVERY TIME you run this script, change the port numbers you use! Otherwise, because UNIX may not let go of your ports immediately, your successive runs may fail!

Finally, you will be required to write a compilation script (see below) that compiles all five of your programs, allowing you to use whatever C code and methods you desire. This will ease grading. Note that only C will be allowed, no C++ or any other language (Python, Perl, awk, etc.).

**Example**

Here is an example of usage, if you were testing your code from the command line:

<pre>$ cat plaintext1
THE RED GOOSE FLIES AT MIDNIGHT  
$ otp_enc_d 57171 &  
$ otp_dec_d 57172 &
$ keygen 10 > myshortkey  
$ otp_enc plaintext1 myshortkey 57171 > ciphertext1   
Error: key myshortkey is too short
$ echo $?
1  
$ keygen 1024 > mykey  
$ otp_enc plaintext1 mykey 57171 > ciphertext1
$ cat ciphertext1  
GU WIRGEWOMGRIFOENBYIWUG T WOFL  
$ keygen 1024 > mykey2  
$ otp_dec ciphertext1 mykey 57172 > plaintext1_a  
$ otp_dec ciphertext1 mykey2 57172 > plaintext1_b  
$ cat plaintext1_a  
THE RED GOOSE FLIES AT MIDNIGHT  
$ cat plaintext1_b  
WVIOWBTUEIOBC  FVTROIROUXA JBWE  
$ cmp plaintext1 plaintext1_a  
$ echo $?
0
$ cmp plaintext1 plaintext1_b  
plaintext1 plaintext1_b differ: byte 1, line 1
$ echo $?
1  
$ otp_enc plaintext5 mykey 57171  
otp_enc_d error: input contains bad characters
$ otp_enc plaintext3 mykey 57172  
Error: could not contact otp_enc_d on port 57172  
$ echo $?
2
$</pre>

**Compilation Script**

You must also write a short bash shell script called "compileall" that merely compiles your five programs. For example, the first two lines might be:

<pre>#!/bin/bash  
gcc -o otp_enc_d otp_enc_d.c  
</pre>

This script will be used to compile your software, and must successfully run on our class server. The compilation must create all five programs, in the same directory as "compileall", for immediate use by the grading script, which is named "p4gradingscript".

## What to Submit

Please submit a single zip file of your program code, which may be in as many different files as you want. Inside that zip file, include the following files:

1.  All of your program code

2.  The compilation script named "compileall"

3.  All five plaintext# files, numbered 1 through 5

4.  A copy of the grading script named "p4gradingscript"

Failing to submit one of the required pieces results in an 8-point deduction, while we attempt to contact you to submit what's missing. Your submission date & time is whenever you send in the missing piece.

## Hints

**Staring Points**

Start with our sample network programs [client.c](/courses/1616246/files/66087255/download?wrap=1 "client.c") and [server.c](/courses/1616246/files/66087261/download?wrap=1 "server.c"). These compile and function! Base your code off of these.

If you have questions about what your programs needs to be able to do, just examine the grading script. Your programs have to deal with exactly what's in there: no more, no less. :)

**Sending Data**

Recall that when sending data, not all of the data may be written with just one call to send() or write(). This occurs because of network interruptions, server load, and other factors. You'll need to carefully watch the number of characters read and/or written, as appropriate. If the number returned is less than what you intended, you'll need to restart the process from where it stopped. This means you'll need to wrap a loop around the send/receive routines to ensure they finish their job before continuing.

If you try to send too much data at once, the server will likely break the transmission, as in the previous paragraph. Consider setting a maximum send size, breaking the transmission yourself every 1000 characters, say.

There are a few ways to handle knowing how much data you need to send in a given transmission. One way is to send an integer from client to server (or vice versa) first, informing the other side how much is coming. This relatively small integer is unlikely to be split and interrupted. Another way is to have the listening side looking for a termination character that it recognizes as the end of the transmission string. It could loop, for example, until it has seen that termination character.

**Concurrency Implications**

Remember that only one socket can be bound to a port at a time. Multiple incoming connections all queue up on the socket that has had <span style="font-family: Courier New;">listen()</span> called on it for that port. After each <span style="font-family: Courier New;">accept()</span> call is made, a new socket file descriptor is returned which is your server's handle to that TCP connection. The server can accept multiple incoming streams, and communicate with all of them, by continuing to call <span style="font-family: Courier New;">accept()</span>, generating a new socket file descriptor each time.

**About Newlines**

You are only supposed to accept the 26 letters of alphabet and the "space" character as valid for encrypting/decrypting. However, all of the plaintext input files end with a newline character. _Text files need to end in a newline character for various reasons._

When one of your programs reads in an input file, strip off the newline. Then encrypt and decrypt the text string, again with no newline character. _When you send the result to stdout, or save results into a file, tack a newline to the end, or your length will be off in the grading script._ Note that the newline character affects the length of files as reported by the wc command! Try it!

**About Reusing Sockets**

In the p4gradingscript, you can select which ports to use: I recommend ports in the 50000+ range. However, UNIX doesn't immediately let go of the ports you use after your program finishes! I highly recommend that you frequently change and randomize the sockets you're using, to make sure you're not using sockets that someone else is playing with. In addition, to allow your program to continue to use the same port (your mileage may vary), read this:

[http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html#setsockoptman](http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html#setsockoptman)

...and then play around with this command:

<pre>setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));</pre>

**Where to Develop**

Finally, I HIGHLY recommend that you develop this program directly on our class server! Doing so will prevent you from having problems transferring the program back and forth, which can cause compatibility issues. Do not use any other non-class server to develop these programs.

If you do see ^M characters all over your files, which come from copying a Windows-saved file onto a UNIX file system, try this command:

<pre>$ dos2unix bustedFile</pre>

