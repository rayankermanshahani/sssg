# sssg - A Simple Static Site Generator

sssg is a lightweight static site generator that converts markdown files into consistently formatted HTML files. It is a command line tool designed to be simple to use and integrate into your existing website workflow.

> **Note:** sssg is designed for Unix-like systems (MacOS, Linux) only. 


### Features
- converts markdown files to HTML
- supports math equations using MathJax
- handles twitter embeds
- generates an index page of all converted files (ie. articles)
- maintains consistent styling across all generated pages


### Prerequisites
- [CMake](https://cmake.org/) (version 3.10 or higher)
- C++ compiler with C++17 support
- Git (for cloning this repository)


### Installation
1. Clone this repository:
```bash
clone https://github.com/rayankermanshahani/sssg.git && cd sssg
```

2. Create build directory and build the project:
```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

3. Install the executable globally:
```bash
sudo make install
```

### Uninstallation
1. Delete the cloned `sssg/` directory:
```bash
cd ..       # assuming you are inside the sssg directory
rm -rf sssg # may require sudo privileges
```
2. Remove the executable binary file:
```bash
rm $(which sssg) # may require sudo privileges
rm -rf 
```


### Usage Instructions
```bash
sssg -h
```


### Expected Directory Structure
sssg expects the following directory structure by default, unless you specify otherwise via its command line arguments:
```
website/
├── content/         # Your markdown files go here
├── styles/          # CSS files
│   ├── reset.css
│   └── writingstyle.css
└── writing/         # Generated HTML files will be placed here
```

### Markdown File Format
Your markdown files should follow this format:
```markdown
# Title of Your Post

Content goes here...

You can use standard markdown syntax:
- **bold**
- *italic*
- [link title](https://example.com)
- ![image alt text](path/to/image.jpg)

Math equations: $x^2 + y^2 = z^2$

Twitter embeds: <tweet>tweet_id</tweet>
```


