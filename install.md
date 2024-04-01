# Linux

Assuming that you have all of the code and resources downloaded. They should have come packaged with this.

## Install Opencv
```bash
cd
sudo apt install pkg-config     
sudo apt install ffmpeg libavformat-dev libavcodec-dev libswscale-dev 
mkdir opencv-install
cd opencv-install
git clone https://github.com/opencv/opencv.git
git clone https://github.com/opencv/opencv_contrib.git
mkdir build
cd build
cmake -DOPENCV_EXTRA_MODULES_PATH=../opencv_contrib/modules ../opencv
make -j8
sudo make install
```
This should install opencv and opencv-contrib, as well as whatever else is required.

Next, you 














# Windows
https://sourceforge.net/projects/opencvlibrary/files/
get latest windows (I used 4.9.0)

extract into C:\opencv

run:
setx OpenCV_DIR C:\opencv\build\x64\vc16