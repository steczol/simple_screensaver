#include "../include/CLI11.hpp"
#include "../lib/SimpleScreensaver.hpp"
//
#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
//
#include <random>

const std::string DefaultWriterGstPipeline =
    "appsrc ! videoconvert ! "
    "video/x-raw,format=YUY2,width=640,height=480,framerate=30/1 ! jpegenc ! "
    "rtpjpegpay ! udpsink host=127.0.0.1 port=5000";

std::unique_ptr<cv::VideoWriter> DefaultVideoWriter() {
  return std::make_unique<cv::VideoWriter>(DefaultWriterGstPipeline,
                                           0,   // fourcc
                                           30,  // fps
                                           cv::Size(640, 480),
                                           true);  // isColor
}

int main(int argc, char** argv) {
  CLI::App app{"Simple screensaver"};
  // Read arguments
  std::string path_to_logo;
  app.add_option("-l,--logo", path_to_logo, "Path to logo");
  //
  std::string gst_pipe_def;
  app.add_option("-g,--gst-pipeline", gst_pipe_def,
                 "GStreamer pipeline definition");
  int gst_fourcc = 0;
  app.add_option("-c,--gst-fourcc", gst_fourcc, "GStreamer fourcc number");
  int gst_fps = 30;
  app.add_option("-f,--gst-fps", gst_fps, "GStreamer frames per second");
  int gst_width = 640;
  app.add_option("--gst-width", gst_width, "GStreamer frame width");
  int gst_height = 480;
  app.add_option("--gst-height", gst_height, "GStreamer frame height");

  CLI11_PARSE(app, argc, argv);
  // std::cout << "path to logo: " << path_to_logo << std::endl;

  // GStreamer
  // cv::VideoWriter writer(
  //     "appsrc ! videoconvert ! "
  //     "video/x-raw,format=YUY2,width=640,height=480,framerate=30/1 ! jpegenc
  //     ! " "rtpjpegpay ! udpsink host=127.0.0.1 port=5000", 0,   // fourcc 30,
  //     // fps cv::Size(640, 480), true);  // isColor

  std::unique_ptr<cv::VideoWriter> writer;
  if (gst_pipe_def.empty()) {
    writer = DefaultVideoWriter();
  } else {
    writer = std::make_unique<cv::VideoWriter>(gst_pipe_def, 0, 30,
                                               cv::Size(640, 480), true);
  }

  if (!writer->isOpened()) {
    std::cerr << "VideoWriter not opened" << std::endl;
    exit(-1);
  }

  std::unique_ptr<SimpleScreensaver> scr_saver =
      std::make_unique<SimpleScreensaver>();
  if (path_to_logo.empty()) {
    scr_saver->setLogo(cv::Mat());
  } else {
    cv::Mat logo = cv::imread(path_to_logo, cv::IMREAD_COLOR);
    scr_saver->setLogo(logo);
  }

  scr_saver->restart();
  cv::Mat canvas;
  while (true) {
    scr_saver->next(canvas);

    writer->write(canvas);
    cv::imshow("Screensaver", canvas);
    int key = cv::waitKey(33);
    if (key == 'q' || key == 'Q') {
      break;
    }
  }

  return 0;
}