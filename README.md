# macfand

Daemon for controlling fans on linux systems using applesmc and coretemp.

This project was written from scratch, but was highly influenced by the famous [mbpfan](https://github.com/linux-on-mac/mbpfan). Please, check [mbpfan](https://github.com/linux-on-mac/mbpfan) out. That is why I kept the GPL-3.0. I rewrote it mostly for fun and exercise, but also to have more control over it.

It has **automatic detection** of all your **fans**, their **settings** (max and min speed), maximal **temperatures** and hardware temperature **monitors**.

**This is unofficial software and by using it you agree to taking full responsibility for possibly damaging your computer.**

## Prerequsities

Make sure you have loaded `coretemp` and `applesmc` kernel modules by running:

```bash
lsmod | grep -e coretemp -e applesmc
```

These modules should be loaded if you are running linux on a Mac, but if they are not, add these two lines to `/etc/modules`:

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

This will install macfand to `/usr/bin` and enable the included `macfand.service` unit to start with your system.

## Configuration

No configuration file is currently used. All configuration is done using command line options.

## Options

#### Version
`--version` or `-V`

Use to see which version of macfand you are using.

#### Help
`--help` or `-?`

Use to get quick info about available options.

#### Daemon
`--daemon` or `-d`

Use to run macfand as daemon. In daemon mode, syslog is used for all messages (stdout and stderr otherwise)and pidfile is located at `/run/macfand.pid`.

#### Poll time
`--poll=NUM` or `-p NUM`

Use to set poll time of (how often temperature is checked and fans adjusted). **Must be whole number bigger than 0.**

#### Verbose
*Verbose mode will be added.*