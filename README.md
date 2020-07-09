# macfand
###### version 0.1

Daemon for controlling fans on Linux systems using applesmc and coretemp.

This project was written from scratch, but was highly influenced by the famous [mbpfan](https://github.com/linux-on-mac/mbpfan). Please, check [mbpfan](https://github.com/linux-on-mac/mbpfan) out. That is why I kept the GPL-3.0. I rewrote it mostly for fun and exercise, but also to have more control over it.

It has **automatic detection** of all your **fans**, their **settings** (max and min speed), maximal **temperatures** and hardware temperature **monitors**.

**This is unofficial software and by using it you agree to taking full responsibility for possibly damaging your computer.**

## Prerequsities

Make sure you have loaded `coretemp` and `applesmc` kernel modules by running:

```bash
lsmod | grep -e coretemp -e applesmc
```

These modules should be loaded if you are running Linux on a Mac, but if they are not, add these two lines to `/etc/modules`:

```bash
coretemp
applesmc
```

## Installation

*Arch AUR package will be available.* 

This installation method is using the included `macfand.service` to enable macfand on startup using systemd. If you wish not to use systemd for activation of macfand on startup, remove all lines including `systemctl` from the `Makefile`, continue with this guide and finally enable macfand using your autostart method.

`$` indicates user shell, `#` indicates root shell

1. Clone this repo using:

```bash    
$ git clone https://github.com/Hipuranyhou/macfand.git
```

2. Compile using:

```bash
$ cd macfand
$ make
```

3. Install using:

```
# make install
```

This will install macfand to `/usr/bin`, configuration file to `/etc` and enable the included `macfand.service` unit to start with your system.

## Uninstall

You can uninstall macfand by running 

```
# make uninstall
```

from inside of the cloned directory if you did not change the executable name.

## Configuration file

Macfand expects configuration file by default. If no path is set using [config](https://github.com/Hipuranyhou/macfand#config) argument, macfand checks for configuration file at `/etc/macfand.conf`. If you want do disable check for configuration file, use one of 'no', 'false' and '0' as value for [config](https://github.com/Hipuranyhou/macfand#config)
. 

Configuration file can be reloaded at runtime by sending `SIGHUP` to macfand. Note that **daemon mode** setting **is ignored when reloading** configuration file at runtime.

For more information about configuration 
options see [macfand.config](https://github.com/Hipuranyhou/macfand/blob/master/macfand.config).

## Options

#### Config
`--config` or `-c`

Use to set path to configuration file. Use one of 'no', 'false' and '0' to disable check for configuration file.

#### Verbose
`--verbose` or `-v`

Use to enable verbose mode producing a lot of output. Not recommended when using
syslog because some messages (for example logging of loaded fans and monitors) is disabled even in verbose mode
when using syslog.
