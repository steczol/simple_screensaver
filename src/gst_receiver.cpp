// inspired by:
// https://github.com/simondlevy/OpenCV_GStreamer/blob/master/Source/Receiver.cpp

#include "../include/CLI11.hpp"
//
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>

const std::string DefaultReceiverGstPipeline =
    "udpsrc port=5000 ! "
    "application/"
    "x-rtp,media=video,payload=26,clock-rate=90000,encoding-name=JPEG,"
    "framerate=30/1 ! rtpjpegdepay ! jpegdec ! videoconvert ! appsink";

std::unique_ptr<cv::VideoCapture> DefaultVideoReceiver() {
  return std::make_unique<cv::VideoCapture>(DefaultReceiverGstPipeline,
                                            cv::CAP_GSTREAMER);
}

int main(int argc, char** argv) {
  // The sink caps for the 'rtpjpegdepay' need to match the src caps of the
  // 'rtpjpegpay' of the sender pipeline Added 'videoconvert' at the end to
  // convert the images into proper format for appsink, without 'videoconvert'
  // the receiver will not read the frames, even though 'videoconvert' is not
  // present in the original working pipeline
  CLI::App app{"GStreamer receiver"};

  // Read arguments
  std::string gst_pipe_def;
  app.add_option("-g,--gst-pipeline", gst_pipe_def,
                 "GStreamer pipeline definition");

  std::unique_ptr<cv::VideoCapture> cap;
  if (gst_pipe_def.empty()) {
    std::cout << "Using GStreamer Pipeline:\n"
              << DefaultReceiverGstPipeline << std::endl;
    cap = DefaultVideoReceiver();
  } else {
    std::cout << "Using GStreamer Pipeline:\n" << gst_pipe_def << std::endl;
    cap = std::make_unique<cv::VideoCapture>(gst_pipe_def, cv::CAP_GSTREAMER);
  }

  if (!cap->isOpened()) {
    std::cerr << "VideoCapture not opened" << std::endl;
    exit(-1);
  }

  cv::Mat frame;
  std::cout << "[I] press q or Ctrl+c to exit" << std::endl;
  while (true) {
    if (!cap->read(frame)) {
      break;
    }

    imshow("Receiver", frame);

    int key = cv::waitKey(1);
    if (key == 'q' || key == 'Q') {
      break;
    }
  }

  return 0;
}