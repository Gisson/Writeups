# Baby reverse (108 points)


## The challenge
```
Hey there!

Disclaimer: This chall is intended for new gamers only ;-)! You veterans got plenty of other Challenges which will keep you busy, so please pass this Challenge to someone, who never or rarely reversed before! We encourage everyone who never reversed anything to try this challenge. We believe in you and your future reversing skills =). You CAN do it!

The task is to find the correct input which will be the flag. See the challenge files for more instructions.
```
We are presented with the above message and a zip file to download containing [challenge binary](public/chall) and [notes](public/notes).


The notes just contain some tips for doing this challenge.


## Solving
Firstly I needed to see what was inside that binary so I ran `objdump -D -M "intel" chall` and I quickly notice that there is
only 1 section and its a .text section. I fired up gdb to easily check what was being run.


The first few instructions (3 or so) were just jumping from here to there. and then there is a point where instruction syscall is run.


The `syscall` instruction is a very special instruction that changes the context of the program to a kernel mode which is the
 highest privilege mode in a system. This instruction executes different things depending on the value of rax register. The function
 it will execute is defined on a syscall table (or in `/usr/include/asm/unistd_{32,64,x32}`)
At this moment I opened up a [syscall table](https://blog.rchapman.org/posts/Linux_System_Call_Table_for_x86_64/).


With gdb we can check that the value of rax is 1. This means that `syscall` will execute a write. From the table we can check that
 rdi is the file descriptor, rsi register is the buffer (string) to be written and rdx is the count (size) of the string.


Back to gdb, rdi has the value of 0, this means that the write will occur on the stdout (standard output), rsi
 has random numbers (so it seems). Using the gdb command `x/s $rsi` we validate that rsi has the value `Welcome to this Chall! \nEnter the Key to win: `, which is probably what will be written in the stdout. Finally, rdx has 0x2e value, which coincidentally is the size
 of the previous string.


Stepping through the `syscall` there is a print, good!
Afterwards there is another `syscall` this time with the value 0. Using the table as a reference we find out that this is a
 read call with basically the same arguments as the write call. So before we run the `syscall` instruction we expect it will
 read 0x2e (46) characters.


Hitting the step button after `syscall` an input shows up, perfect!


Inputing `AAAAA` it accepts the input with no problem.



An instruction `movzx` is called. This is a menmonic for MOVe Zero eXtended. Which is basically a MOV instruction with extra features.
 Then there is a `xor`, an `inc` and a `dec` and right after there is a `jne`. At this point I thought to myself "This is most likely
 a cycle". After hitting step a couple times ... "yeah its a cycle".


Step, step step a little more and finally the cycle broke. It seems like it is just clearing the rsi values. Not much.


The program runs some instruction and gets to a rather strange instruction `repz cmps BYTE PTR ...`. Looking at it "That is some
 strange instruction" and activated google mode.



[Repz](http://www.felixcloutier.com/x86/REP:REPE:REPZ:REPNE:REPNZ.html)  is a prefix instruction menmonic for REPeat until
 Zero.



Basically it is a loop, so let's hit step. and then there is a condition which apparently only doesn't if the earlier instruction
 succeded (in this case if the comparations of repz were ok). Alright let's hit step!


The jump is taken and program just exits... that's bad. Going back to the `jne` instruction I changed the instruction pointer
 to the instruction next to the jump using `set $rip=0x4000bd`. Hitting step a couple of times I get greeted with a message
 'Yay!'. That's where I want to be!



Checking the program execution I tried to figure out where my input was used and it was only in the cycle mentioned before.


Alright! Time to debunk that cycle!


After trying to understand the cycle, I thought it was better if I wrote a pseudocode that would resemble that same cycle.
 So I came up with the following:
```
char input[46] = read();

for(int i=0;i<45;i++){
	input[i]=input[i]^input[i+1];
}
```
This seems fine. This means that every letter of the input will be Xor'd with the next.


Next I decided the best was to write a C program that would resemble this simple Xor algorithm. So I wrote [test.c](test/test.c)
 which Xors all of our input. That is fantastic (not really). But which value are we really aiming at?


Going back to gdb and pausing on the `repz`, it is comparing out input Xor'd with 0x261838221c060d0a. Bingo!


Now we just need something that can validate the expression `input[i]^input[i+1]=target[i]`. Since we know the flag is in the format
 `flag{...}` then we know target[i] and input[i]. Using xor properties (if a^b=c then b^c=a and a^c=b) then we get an expression
 which is `input[i]^target[i]=input[i+1]`.


Since I'm almost always insecure about my crazy theories, lets first do some of there by hand. After some writing I start comming up
 with `flag{Yay`. Good! 


After this just a little scripting and a stupid and boring way (because I didnt want to lose much time) I came up with
 [exploit.py](exploit.py) which is self explanatory.


After running the script:

flag{Yay_if_th1s_is_yer_f1rst_gnisrever_flag!}


Lovely challenge. :D

## FAQ

### What is a register?
Think of registers as processor values. They exist to temporarily store some value. Some registers are specific for some actions.
[Learn more](https://en.wikibooks.org/wiki/X86_Assembly/X86_Architecture).


### What is al, ah and eax?
Registers can be 64,32,16 and 8 bits. r(something) stands for 64-bit registers, e(something) stands for 32-bit registers
 ax, bx, etc stand for 16 bit registers. al, bl, etc are the lowest bits of ax,bx, etc respectively. ah, bh, etc
 are the highest bits of ax, bc, etc respectively.
[Learn more](https://en.wikibooks.org/wiki/X86_Assembly/X86_Architecture).

### How do I know 0 is stdout?
In Unix systems there are reserved file descriptors. These are 0 for stdout, 1 for stdin and 2 for stderr.
[Learn more](https://linuxmeerkat.wordpress.com/2011/12/02/file-descriptors-explained/)

