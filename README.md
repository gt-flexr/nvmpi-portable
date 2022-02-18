# nvmpi-portable

L4T Multimedia API with ffmpeg, jetson-ffmpeg, and opencv4.

## Installation
1. Dependencies
  - jetson-ffmpeg: https://github.com/jocover/jetson-ffmpeg
  - opencv4: https://docs.opencv.org/4.x/d7/d9f/tutorial_linux_install.html

2. nvmpi-portable
```
git clone https://github.com/jheo4/nvmpi-portable.git
cd nvmpi-portable && mkdir build
cd build && cmake ..
make -j$(nproc) && sudo make install
```

3. testing
```
g++ nvmpi_test.cc `pkg-config --libs --cflags libavcodec libavformat libavutil opencv4 nvmpi `
g++ portable_test.cc `pkg-config --libs --cflags libavcodec libavformat libavutil opencv4 nvmpi nvmpi_portable`
```

## What nvmpi-portable is?
It's for HW-accelerated encoding/decoding at the code level and each frame granularity.
[jetson-ffmpeg](https://github.com/jocover/jetson-ffmpeg) is easily usable with ffmpeg, but there are some glitches
using it with libavcodec. It seems because of the discrepancies between libavcodec contexts and jetson-ffmpeg. So,
instead of using nvmpi via libavcodec interfaces, nvmpi-portable directly use nvmpi for encoding/decoding.
nvmpi-portable has discrepancies of nvmpi, ffmpeg, and opencv4 because nvmpi-portable delegates the image/video-related
compelxities to them. Moreover, it is opencv compatible.

So, it can be easily usable as a library and integratble to other projects; I wrote this to use for my research project
:). You can compare the compelxities between `nvmpi_test.cc` and `portable_test.cc`.


## Credit
nvmpi-portable is inspired by [jetson-ffmpeg](https://github.com/jocover/jetson-ffmpeg) and I refer it.

