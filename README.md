# macfand
###### version 0.1-alpha

Daemon for controlling fans on Linux systems using applesmc and coretemp.

This project was written from scratch, but was highly influenced by the famous [mbpfan](https://github.com/linux-on-mac/mbpfan). Please, check [mbpfan](https://github.com/linux-on-mac/mbpfan) out. That is why I kept the GPL-3.0. I rewrote it mostly for fun and exercise, but also to have more control over it.

It has **automatic detection** of all your **fans**, their **settings** (max and min speed), maximal **temperatures** and hardware temperature **monitors**.

**This is unofficial software and by using it you agree to taking full responsibility for possibly damaging your computer.**

For more information about installation and configuration, please, continue to [wiki](https://github.com/Hipuranyhou/macfand/wiki).

## Options

#### Config
`--config` or `-c`

Use to set path to configuration file. Use one of 'no', 'false' and '0' to disable check for configuration file.

#### Verbose
`--verbose` or `-v`

Use to enable verbose mode producing a lot of output. Not recommended when using
syslog because some messages (for example logging of loaded fans and monitors) is disabled even in verbose mode
when using syslog.
