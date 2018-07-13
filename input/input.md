## input

### stage1
With some python help I passed stage 1:

```
./input 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, "`printf "\x00"`" "`printf " \n\r"`" x 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98,
```
### stage2
Then, need to send to stdin and stderr, thanks to the comments in this question https://stackoverflow.com/questions/48443136/c-how-read-function-read-from-stderr
I was able with:
```
echo -e -n '\x00\x0a\x02\xff' > stage2_stderr_payload
dafna@blah:~/kamomil/pwnable/input$ echo -n -e '\x00\x0a\x00\xff' | ./input 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, "`printf "\x00"`" "`printf " \n\r"`" x 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 2<stage2_stderr_payload
bash: warning: command substitution: ignored null byte in input
Welcome to pwnable.kr
Let's see if you know how to give input to program
Just give me correct inputs then you will get the flag :)
Stage 1 clear!
Stage 2 clear!
```
### stage3
need to set a binary env variable, can do it with:
```
echo -e -n '\x00\x0a\x00\xff' > stage2_stdin_payload
$ env $( printf "\xde\xad\xbe\xef=\xca\xfe\xba\xbe") ./input 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, "`printf "\x00"`" "`printf " \n\r"`" x 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 2<stage2_stderr_payload 0<stage2_stdin_payload
bash: warning: command substitution: ignored null byte in input
Welcome to pwnable.kr
Let's see if you know how to give input to program
Just give me correct inputs then you will get the flag :)
Stage 1 clear!
Stage 2 clear!
Stage 3 clear!
```
### stage4
```
$echo -e -n '\x00\x00\x00\x00' > ''$'\n'

$env $( printf "\xde\xad\xbe\xef=\xca\xfe\xba\xbe") ./input 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, "`printf "\x00"`" "`printf " \n\r"`" x 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 2<stage2_stderr_payload 0<stage2_stdin_payload
bash: warning: command substitution: ignored null byte in input
Welcome to pwnable.kr
Let's see if you know how to give input to program
Just give me correct inputs then you will get the flag :)
Stage 1 clear!
Stage 2 clear!
Stage 3 clear!
Stage 4 clear!
```
### stage5
This stage opens a socket on the port specified in index 'C' in the args and listen to it,
So I changed location 'C' (67) to be 1234:
```
env $( printf "\xde\xad\xbe\xef=\xca\xfe\xba\xbe") ./input 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, "`printf "\x00"`" "`printf " \n\r"`" 1234 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 
2<stage2_stderr_payload 0<stage2_stdin_payload
```
Then in another terminal:
```
$echo -e -n '\xde\xad\xbe\xef' | nc 127.0.0.1 1234
```
After I made it working locally it still took me sometime to understand how to make it work on the server.
As it is possible to write files only in /tmp
After many 'permission denied, I made it work like this:"
```
mkdir /tmp/dafna
cd /tmp/dafna
echo -e -n '\x00\x0a\x02\xff' > stage2_stderr_payload
echo -e -n '\x00\x0a\x00\xff' > stage2_stdin_payload
echo -e -n '\x00\x00\x00\x00' > ''$'\n'
ln -s /home/input2/flag flag
env $( printf "\xde\xad\xbe\xef=\xca\xfe\xba\xbe") ~/input 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, "`printf "\x00"`" "`printf " \n\r"`" 1234 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 2<stage2_stderr_payload 0<stage2_stdin_payload
```
Then opened another ssh for the `input2@ubuntu:~$ echo -e -n '\xde\xad\xbe\xef' | nc 127.0.0.1 1234`
