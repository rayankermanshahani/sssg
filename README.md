# sssg - A Simple Static Site Generator

### Project Structure:
```
sssg/
├── src/
│   ├── main.cpp        # entry point + CLI + main program flow
│   ├── markdown.hpp    # markdown processing functions + data structures
│   └── markdown.cpp    # implementation of markdown processing
└── CMakeLists.txt      # build configuration
```

### Installation (Manual):
1. Clone directory
```
git clone https://github.com/rayankermanshahani/sssg && cd sssg
```
2. Build executable
```
mkdir build && cd build
cmake ..
```
3. Compile program
```
# unix
make

# windows
cmake --build . --config Release
```
4. Run executable
```
# unix
./sssg build

# windows
.\Release\sssg.exe build
```
5. Install globally (optional)
```
# unix only
sudo make install
```

### Uninstall Instructions (Manual):
1. Remove global installation (if installed)
```
# unix only
sudo make uninstall
```
2. Remove local files
```
# from project root
rm -rf build/  # remove build directory
cd ..          # navigate back to project directory
rm -rf sssg/   # remove project directory
```

