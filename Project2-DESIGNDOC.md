# Os15_158_159_160_161
Project1-2
		     +--------------------------+
		     |           CS101OS        |
         | PROJECT 2: USER PROGRAMS	|
		     |     DESIGN DOCUMENT      |
		     +--------------------------+



---- GROUP ----

Chen Hu <10132510158@ecnu.cn>
Jiajun Shi <10132510159@ecnu.cn>
Yi Feng <10132510160@ecnu.cn>
Shuaifeng Pang <10132510161@ecnu.cn>


---- PRELIMINARIES ----

>> If you have any preliminary comments on your submission, notes for the
>> TAs, or extra credit, please give them here.

>> Please cite any offline or online sources you consulted while
>> preparing your submission, other than the Pintos documentation, course
>> text, lecture notes, and course staff.

    部分参考GITHUB上的代码资源

                           ARGUMENT PASSING
                           = = = = = = = = = = = = = = 

---- DATA STRUCTURES ----

>> A1: Copy here the declaration of each new or changed `struct' or
>> `struct' member, global or static variable, `typedef', or
>> enumeration.  Identify the purpose of each in 25 words or less.

无

---- ALGORITHMS ----

>> A2: Briefly describe how you implemented argument parsing.  How do
>> you arrange for the elements of argv[] to be in the right order?
>> How do you avoid overflowing the stack page?
    
首先，修改了一些字符串来避免过量的空白。
然后，将字符串转换并复制每个参数字符串的头部存入一个数组内strtok_r()调用的次数也给我了一个参数。然后从数组中获得每个参数的地址和长度并且将他们以相反的顺序放入栈中。
同时，我将的得到的地址存入另外一个数组中。然后push所有地址，再push一个假的返回值。
argv[]的元素都反序push到栈中。通过检查来避免了溢出确定参数的字符串不会过长。

---- RATIONALE ----

>> A3: Why does Pintos implement strtok_r() but not strtok()?

内核需要折返。因此，实现 strtok_r()，就是strtok()的折返版本。使用strtok_r(),能保存最后一个token的指针。pintos中如果使用strtok()，在多个线程涉及此函数时可能会导致竞争。


>> A4: In Pintos, the kernel separates commands into a executable name
>> and arguments.  In Unix-like systems, the shell does this
>> separation.  Identify at least two advantages of the Unix approach.

1.内核的代码能够更加简化，减少在内核中运行的时间。因为shell是系统程序，变量和参数的分离是在内核外的，所以减少了内核中的潜在bug 的数量，避免kernel fail。
2.当使用shell实现分离命令后，可以同时传递多条命令。


                             SYSTEM CALLS
                             = = = = = = = = = =

---- DATA STRUCTURES ----

>> B1: Copy here the declaration of each new or changed `struct' or
>> `struct' member, global or static variable, `typedef', or
>> enumeration.  Identify the purpose of each in 25 words or less.

子线程的结构
struct child_thread {
    tid_t pid;  // thread pid
    int exit_status;  // exit status (used by wait)
    bool exited;   // is the thread running?
    bool waiting;  // are we waiting on the child?
    struct list_elem elem;
};


打开的文件的结构
struct file_desc {
    int id;  // file descriptor
    struct list_elem elem;  
    struct file *file;  // file
};

添加部分变量来包含用户进程的相关信息
struct thread {
    …
    tid_t parent_pid;  // parent process pid
    int load_success;  // did the executable load?
    struct file *executable; // executable file
    struct list file_descs;  // file descriptor list
    struct list child_threads;  // child thread list
    …
};


>> B2: Describe how file descriptors are associated with open files.
>> Are file descriptors unique within the entire OS or just within a
>> single process?

当一个文件打开的时候文件描述符就创建了。每个文件描述符在整个OS里都是唯一的。当文件关闭的时候，就删除。file_desc的结构体将文件描述符和打开的文件匹配起来。
进程拥有一个file_desc结构体的列表。

---- ALGORITHMS ----

>> B3: Describe your code for reading and writing user data from the
>> kernel.
READ：
首先检查buffer与buffer+size是否为合法指针。如果不是，exit(-1)。获得fs_lock（文件系统锁）。当当前线程成为锁持有者，检查fd是否是STDOUT_FILENO或STDIN_FILENO。如果是标准输出，释放锁返回-1；如果是STDIN_FILENO，从标准输入流中获取keys，然后释放锁，返回0。否则，根据fd从open_file list中找到打开文件。然后使用filesys中的file_read来读文件，获取状态，释放锁返回状态。
WRITE:
前几步与read操作类似，当fd为STDIN_FILENO时，释放锁，返回-1；当fd为STDOUT_FILENO时，用putbuf打印出buffer的内容。当fd不是上述两种情况时，根据fd找到相应的打开文件，使用filesys中的file_write将buffer写到文件里，获得状态，释放锁，返回状态。
>> B4: Suppose a system call causes a full page (4,096 bytes) of data
>> to be copied from user space into the kernel.  What is the least
>> and the greatest possible number of inspections of the page table
>> (e.g. calls to pagedir_get_page()) that might result?  What about
>> for a system call that only copies 2 bytes of data?  Is there room
>> for improvement in these numbers, and how much?

>> B5: Briefly describe your implementation of the "wait" system call
>> and how it interacts with process termination.

等待的系统调用获得一个pid。然后会在它的子进程中查找到该pid的进程。如果没有找到，返回-1。否则，它会检查该子进程是否结束运行。如果结束了，就返回该子进程现在的状态，然后从列表中删除。如果没有结束，就设置等待位和blocks。如果问题中的子进程结束了，它会看向父进程并更新自己的状态。如果waiting bit设置了，子进程会unblock父进程所以父进程能够得到存在的状态并返回值。
如果子进程被内核终止，等待位仍旧为false。那么父进程就会知道子进程是被内核终止的。
如果父进程提早被终止，其占有资源会被释放，那么其子进程就会发现其父进程已经存在，然后不进行状态的设置，继续执行。

>> B6: Any access to user program memory at a user-specified address
>> can fail due to a bad pointer value.  Such accesses must cause the
>> process to be terminated.  System calls are fraught with such
>> accesses, e.g. a "write" system call requires reading the system
>> call number from the user stack, then each of the call's three
>> arguments, then an arbitrary amount of user memory, and any of
>> these can fail at any point.  This poses a design and
>> error-handling problem: how do you best avoid obscuring the primary
>> function of code in a morass of error-handling?  Furthermore, when
>> an error is detected, how do you ensure that all temporarily
>> allocated resources (locks, buffers, etc.) are freed?  In a few
>> paragraphs, describe the strategy or strategies you adopted for
>> managing these issues.  Give an example.

进程尝试读完PHYS_BASE或者有分段错误会有exit code 为-1。当exit被调用的时候
它会释放所有必要的blocks以及其它任务。

---- SYNCHRONIZATION ----

>> B7: The "exec" system call returns -1 if loading the new executable
>> fails, so it cannot return before the new executable has completed
>> loading.  How does your code ensure this?  How is the load
>> success/failure status passed back to the thread that calls "exec"?

当process_execute 运行的时候，它会创建一个值为1的信号量并减。然后它会开始它的子进程，	并尝试再次减少。当然，这样会被block知道信号量增加，而且这只会在新的执行load()被调用时实现。这确保了只有load()执行后exec才会返回。子线程之后会block来允许父进程检查子进程的load_status。


>> B8: Consider parent process P with child process C.  How do you
>> ensure proper synchronization and avoid race conditions when P
>> calls wait(C) before C exits?  After C exits?  How do you ensure
>> that all resources are freed in each case?  How about when P
>> terminates without waiting, before C exits?  After C exits?  Are
>> there any special cases?

如果在Cexit之前 P就调用了wait(C),它会blocks直到C exit并且unblocks P。
内核会释放C的进程，并且P会释放P中的C 的数据。如果C存在以后P调用wait(C),C已经更新了P的进程的exits的细节所以P能访问并释放他们在调用wait时。内核已经释放了C的进程。如果P在Cexits之前就exits了，P会更新C的父进程的PID所以会指向main。如果P在C exit之后exit。P会有效地释放所有C的信息。

---- RATIONALE ----

>> B9: Why did you choose to implement access to user memory from the
>> kernel in the way that you did?

使用了参考的代码，确保了指针在正确的范围内，成功解决了分页错误，
因为使用了处理器MMU的优点所以更快。


>> B10: What advantages or disadvantages can you see to your design
>> for file descriptors?

优点：使用了一个简单的文件描述符列表，文件和整数之间的匹配更加的简单，所以使用文件描述符不是特别复杂。一个helper函数从描述符的标号获得file_desc的结构体使得操作更加简单。
缺点：如果某个进程含有大量的文件，打开它会消耗一些时间来定位到文件描述符。这回增加内核的开销。

>> B11: The default tid_t to pid_t mapping is the identity mapping.
>> If you changed it, what advantages are there to your approach?

没有改变。

                           SURVEY QUESTIONS
                           = = = = = = = = = = = = =

Answering these questions is optional, but it will help us improve the
course in future quarters.  Feel free to tell us anything you
want--these questions are just to spur your thoughts.  You may also
choose to respond anonymously in the feedback survey on the course
website.

>> In your opinion, was this assignment, or any one of the three problems
>> in it, too easy or too hard?  Did it take too long or too little time?

>> Did you find that working on a particular part of the assignment gave
>> you greater insight into some aspect of OS design?

>> Is there some particular fact or hint we should give students in
>> future quarters to help them solve the problems?  Conversely, did you
>> find any of our guidance to be misleading?

>> Do you have any suggestions for the TAs to more effectively assist
>> students, either for future quarters or the remaining projects?

>> Any other comments?
