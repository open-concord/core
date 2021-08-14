# Concord/Main
Concord is a distributed messaging protocol

This is currently the Pre-Alpha build, Run-At-Your-Own-Risk (or not at all)

Register to become a beta tester; shoot either maintainer an email

<br>

## Using
`Concord/Main` produces either a Shared Object (Unix), or a Dynamic-Link Library (Windows). If you're here because you want a plug and play application, check out [Concord/Desktop](https://github.com/open-concord/desktop/).

You can find stable releases of both the SO and DLL under this repository's releases page. You can also build from source.

*Concord currently only has g++ support in its makefile(s), but feel free to contribute Windows support!*

<br>
<br>

## Building from source
Making a SO <ins>On Unix</ins>
To install globally, you can simply use:
```
> make install_unix
```
You're now able to use the Concord share lib!
<br>

**Compiling your own (cpp) code with the `concord.so` using g++**

*See above note on folder location and linking*
```
// compile your program
> g++ ... my_code.cpp -o my_executable -lconcord

// make the .SO avaliable at runtime (this step is unnessecary if you installed in a common location; /usr/, /usr/local/, etc.)
> export LD_LIBRARY_PATH=<path_to_.SO_file>:$LD_LIBRARY_PATH

// run your program
> ./my_executable
```


> Issues with building? Check out [this site's wonderful .so section](http://www.yolinux.com/TUTORIALS/LibraryArchives-StaticAndDynamic.html) first, then if your problem persists, file an issue.

*Remember, the last code block is for compiling C++ code with the concord lib, other languages will have different ways of linking.*

<br>

----

<br>

Making a DLL **not very well implemented, use at own risk, but feel free to pull req :)**
- This assumes you have MinGW installed; if not, vsCode has a [good article](https://code.visualstudio.com/docs/cpp/config-mingw) on it

```
// building the lib
> cd src
> make win

// move to a known dir
move <path_to_.DLL_file> <known_dir>

// compile with own code
g++ my_code.cpp -o my_executable -lconcord
```
*Something wrong with this? Submit a pull request, every contribution helps!*
<br>
<br>

## Dependencies
Boost Asio >= 1.71