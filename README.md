# Intel<sup>®</sup> PMWatch
Intel<sup>®</sup> PMWatch (PersistentMemoryWatch) is a tool that monitors and reports the performance and health information metrics of the Intel<sup>®</sup> Optane™ DC Persistent Memory.

# Supported OS
Linux

# Supported Platforms
Any Intel<sup>®</sup> platform with Intel<sup>®</sup> Optane™ DC Persistent Memory.

# Documentation
The information about features and options available in the tool can be found at [PMWatch_User_Guide.pdf](https://github.com/intel/intel-pmwatch/blob/master/docs/PMWatch_User_Guide.pdf).

# Build
**Check *[Dependencies](#dependencies)* section to resolve the dependencies.**

> $ ./autogen.sh<br/>
> $ ./configure [--with-libipmctl=\<path_to_libipmctl> --with-libipmctl-inc=\<path_to_libipmctl_header_files> --prefix=\$HOME/pmwatch --bindir=\$HOME/pmwatch/bin64 --libdir=\$HOME/pmwatch/lib64]<br/>
> $ make<br/>
> $ make -j install

The parameters for configure are optional.<br/>
**If you are building PMWatch for [collectd (dcpmm plugin)](https://github.com/collectd/collectd) dependency, make sure to use `--libdir=<install_path>/lib64`.**

## Using scripts to build and create install package
You can *also* use the scripts available for build and package creation.

> $ ./prepare_pmwatch_release.sh M=<major_version> m=<minor_version> u=<update_num> with-libipmctl=<path_to_libipmctl> with-libipmctl-inc=<path_to_libipmctl_header_files> os=linux -r

Check *[pmw_version.h](https://github.com/intel/intel-pmwatch/blob/master/src/inc/pmw_version.h)* to obtain major, minor version and update number.

### Build
Standalone build script.

> $ ./build_pmwatch.sh -all with-libipmctl=<path_to_libipmctl> with-libipmctl-inc=<path_to_libipmctl_header_files> os=linux

### Package
Standalone packaging script.

> $ ./package_pmwatch.sh M=<major_number> m=<minor_number> u=<update_num> os=linux -r

Use the `-h` option to get more details about the [scripts'](https://github.com/intel/intel-pmwatch/) options.

## Dependencies
PMWatch is dependent on [libipmctl](https://github.com/intel/ipmctl).

Perform the following steps to install the tool dependencies.

#### Fedora
> $ yum install daxctl-devel.x86_64 ndctl-devel.x86_64 libipmctl-devel.x86_64

#### CentOS, RHEL
> $ wget https://copr-be.cloud.fedoraproject.org/results/jhli/ipmctl/epel-7-x86_64/00874029-ipmctl/libipmctl-devel-02.00.00.3446-1.el7.x86_64.rpm<br/>
> $ yum install ndctl-libs.x86_64<br/>
> $ rpm -ivh libipmctl-devel-02.00.00.3446-1.el7.x86_64.rpm

#### Ubuntu, Debian
> $ apt install libdaxctl-dev libndctl-dev libipmctl-dev

### Build tools
Install the following build tools:
> autoconf, automake, flex, bison, libtool, pkg-config, libkmod-dev,
> libudev-dev, uuid-dev
