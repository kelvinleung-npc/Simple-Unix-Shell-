# Simple-Unix-Shell-
A simple shell made for assignment 1 of CSC 360 

Intro
-----------
Acknowledgements: This Assignment was for CSC 360 at the University of Victoria in the Summer 2020 semester taught by Dr Jason Corless. 
The purpose of this assignment was to learn the basics of how the user and the kernal interact through terminal. 
The shell program used the functions: fork(), execvp(), getcwd, chdir(),waitpid(), SIGSTOP, SIGCONT among other similar functions. 

Part 1: 
If the user types: shell> ls -l /usr/bin 
The shell should execute the ls command with the parameters -l /usr/bin which should return the results of the contents 
in that directory. 

Part 2: 
Using getcwd() and chdir(), add the functionality that allows the user to use the command cd and pwd to change the working directory and print the working directory. 
ex: "cd home/coolstuff" 
ex: "pwd" > "/home/coolstuff"

Part 3: 
Execute programs in the background. 
ex: "bg cmd text.txt" will start the command cmd with the argument text.txt following it 
ex: "bglist" will display a list of currently executing commands in the bg 
0: /.......
1: /.......
Total Background Jobs: 2
ex: "bgkill 1" will kill job 1 

Part 4: 
Stop and Resume jobs. 
stop will suspend a job by SIGSTOP
start will resume a job that is stopped
updated bg list to include [R] and [S] for Running and Stopped 


