These are all analysis on data from various experiments conducted, including outputs from the programs. Used CERN's ROOT framework.

Some files have the line:

```#include <all_headers.h>```

all_headers.h is a file that includes all paths for headers that come with ROOT, for my own convenience. Because ROOT automatically includes all files in the include path, but my editor was sad.

Source files have the extension .C, but they are C++ files. The compiler I used, which I believe to be Cling, compiled .C files as C++.
