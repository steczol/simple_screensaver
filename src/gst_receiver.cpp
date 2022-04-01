// inspired by:
// https://github.com/simondlevy/OpenCV_GStreamer/blob/master/Source/Receiver.cpp

#include <iostream>
#include <opencv2/opencv.hpp>

int main() {
  // The sink caps for the 'rtpjpegdepay' need to match the src caps of the
  // 'rtpjpegpay' of the sender pipeline Added 'videoconvert' at the end to
  // convert the images into proper format for appsink, without 'videoconvert'
  // the receiver will not read the frames, even though 'videoconvert' is not
  // present in the original working pipeline
  cv::VideoCapture cap(
      "udpsrc port=5000 ! "
      "application/"
      "x-rtp,media=video,payload=26,clock-rate=90000,encoding-name=JPEG,"
      "framerate=30/1 ! rtpjpegdepay ! jpegdec ! videoconvert ! appsink",
      cv::CAP_GSTREAMER);

  if (!cap.isOpened()) {
    std::cerr << "VideoCapture not opened" << std::endl;
    exit(-1);
  }

  cv::Mat frame;
  while (true) {
    if (!cap.read(frame)) {
      break;
    }

    imshow("receiver", frame);

    int key = cv::waitKey(1);
    if (key == 'q' || key == 'Q') {
      break;
    }
  }

  return 0;
}