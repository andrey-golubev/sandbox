Cool Compiler
---

This is the subrepository containing files to complement Cool compiler (more on
https://web.stanford.edu/class/archive/cs/cs143/cs143.1128/).

## Install: course steps

* Ubuntu packages: `sudo apt-get install flex bison build-essential csh openjdk-6-jdk libxaw7-dev`.
  JDK is optional, only required for Java version.
* `sudo mkdir -p /usr/class/cs143/cool/ && sudo chown $(whoami):$(whoami) -R /usr/class/`
* Untar contents of [cool_sources.tar.gz](./files/cool_sources.tar.gz) into /usr/class (`tar -xf`
  will suffice).
* `ln -s /usr/class/cs143/cool ~/cool`
* Update PATH environment variable (e.g. in .bashrc): `export PATH=$PATH:/usr/class/cs143/cool/bin/`

## Install: extra steps for Assignment 2

* Make build directory: `rm -rf ~/cool/build && mkdir -p ~/cool/build/pa2`
* Run `cd ~/cool/build/pa2 && make -f ../../assignments/PA2/Makefile` to generate all necessary
  files and create symlinks
* Delete `cool.flex` and make a symlink to this repo's [cool.flex](./pa2/cool.flex). For
  example: `rm cool.flex && ln -s ~/sandbox/cool/pa2/cool.flex ~/cool/build/pa2/cool.flex`

> Note: the following step is for newer versions of Flex (e.g. Flex 2.6.x)
* Delete `Makefile` and make a symlink to this repo's version of [Makefile](./pa2/Makefile). For
  example: `rm Makefile && ln -s ~/sandbox/cool/pa2/Makefile ~/cool/build/pa2/Makefile`
* Test: `make lexer && ./lexer test.cl`. This should work successfully


## Install: extra steps for Assignment 3

> Note: this assignment is done with `flex-old` package (available in Ubuntu). Tested flex version
> 2.5.4. Newer version have known issues with "undefined reference to yylex".

* Make build directory: `rm -rf ~/cool/build && mkdir -p ~/cool/build/pa3`
* Run `cd ~/cool/build/pa3 && make -f ../../assignments/PA3/Makefile` to generate all necessary
  files and create symlinks
* Update `parser-phase.cc`, comment out line #29 like:
```
// int curr_lineno;               // needed for lexical analyzer
```
* Delete `cool.y` and make a symlink to this repo's version of [cool.y](./pa3/cool.y). For example:
  `rm cool.y && ln -s ~/sandbox/cool/pa3/cool.y cool.y`
* Test `make parser`. Compilation should be successful.

## Install: extra steps for Assignment 4

> Note: this assignment is done with `flex-old` package (available in Ubuntu). Tested flex version
> 2.5.4. Newer version have known issues with "undefined reference to yylex".

* Make build directory: `rm -rf ~/cool/build && mkdir -p ~/cool/build/pa4`
* Run `cd ~/cool/build/pa4 && make -f ../../assignments/PA4/Makefile` to generate all necessary
  files and create symlinks
* Delete `cool-tree.h` and make a symlink to this repo's [cool-tree.h](./pa4/cool-tree.h). For
  example: `rm cool-tree.h && ln -s ~/sandbox/cool/pa4/cool-tree.h cool-tree.h`
* Delete `semant.cc` and make a symlink to this repo's [semant.cc](./pa4/semant.cc). For example:
  `rm semant.cc && ln -s ~/sandbox/cool/pa4/semant.cc semant.cc`
* Delete `semant.h` and make a symlink to this repo's file [semant.h](./pa4/semant.h). For example:
  `rm semant.h && ln -s ~/sandbox/cool/pa4/semant.h semant.h`
* Test `make semant`. Compilation should be successful.
