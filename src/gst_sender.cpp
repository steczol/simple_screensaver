// inspired by:
// https://github.com/simondlevy/OpenCV_GStreamer/blob/master/Source/Sender.cpp

#include <iostream>
#include <opencv2/opencv.hpp>
#include <random>

std::random_device rd{};
std::mt19937 gen{rd()};
std::uniform_int_distribution<> f_8bit(0, 255);

int main() {
  //   cv::VideoCapture cap(0);
  //   if (!cap.isOpened()) {
  //     std::cerr << "VideoCapture not opened" << std::endl;
  //     exit(-1);
  //   }

  cv::VideoWriter writer(
      "appsrc ! videoconvert ! "
      "video/x-raw,format=YUY2,width=640,height=480,framerate=30/1 ! jpegenc ! "
      "rtpjpegpay ! udpsink host=127.0.0.1 port=5000",
      0,   // fourcc
      30,  // fps
      cv::Size(640, 480),
      true);  // isColor

  if (!writer.isOpened()) {
    std::cerr << "VideoWriter not opened" << std::endl;
    exit(-1);
  }

  cv::Mat frame;
  cv::Scalar val;
  while (true) {
    val = cv::Scalar(f_8bit(gen), f_8bit(gen), f_8bit(gen));
    frame = cv::Mat(cv::Size(640, 480), CV_8UC3, val);
    // cap.read(frame);

    writer.write(frame);
  }

  return 0;
}