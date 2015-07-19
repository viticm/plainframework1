# Plain framework

Plain framework is base with c++ smart framework for net applications(socket game or more).

- **Author:** Viticm
- **Website:** [http://www.cnblogs.com/lianyue/](http://www.cnblogs.com/lianyue/)
- **Version:** 1.0.5

[![Build Status](https://travis-ci.org/viticm/plainframework1.svg)](https://travis-ci.org/viticm/plainframework1)

<img src="https://github.com/viticm/plainframework1/blob/master/documentation/examples/gateway.gif" />

## Develop environment

To install plain framework need depends with shell script(Only on linux or unix)

```shell
cd tools/script/linux/install/ && sh install_environment.sh
```


### Build

Plain build in windows can use visual studio or cmake, in linux just use cmake, you can run command `yum -y install cmake` installed.

Your can build the plain framework with this command.

```shell
cd pf/core && cmake ./ && make
```

## Documentation

The complete docs for Plain framework can be found at the docs in the `/documentation` directory.


## Copyright and License
Plain framework was written by Viticm.
Plain framework is released under the MIT License. See the LICENSE file for details.


## Recent Changelog

### 1.0.4
- Bugfix: All memory lost in pf core.

### 1.0.5
- NewAdd: Add project manager files, see the docs in the `/documentation/material/ProjcetManager.txt`
- Optimize: All framework codes and directory.
