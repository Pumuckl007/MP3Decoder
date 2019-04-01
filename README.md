This is an attempt at creating an mp3 decoder from scratch.

## Building
To build you must have the cmake and clang packages installed.

### Dependencies
Start by initializing the submodules for googletest
```bash
git submodule init googletest
git submodule update googletest
```
Then compile googletest.
```bash
cd googletest/googletest
mkdir build
cd build
cmake ..
make
cd ../../../
```

### Compiling
Now all the dependencies are taken care of it is time to compile by running
cmake in the build directory.

```bash
mkdir build
cd build
cmake ..
make
```

After compiling the tests executable can be found in build/test/ and the
application executable can be found in build/src .
