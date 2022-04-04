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

std::unique_ptr<cv::VideoWriter> DefaultVideoWriter(int width, int height) {
  return std::make_unique<cv::VideoWriter>(DefaultWriterGstPipeline,
                                           0,   // fourcc
                                           30,  // fps
                                           cv::Size(width, height),
                                           true);  // isColor
}

int main(int argc, char** argv) {
  CLI::App app{"GStreamer sender"};
  // Read arguments

  // Read logo
  std::string path_to_logo;
  app.add_option("-l,--logo", path_to_logo, "Path to logo");

  // Read scene geometry
  int scene_width{640};
  app.add_option("--scene-width", scene_width, "Scene width")
      ->check(CLI::Range(320, 3840));
  int scene_height{480};
  app.add_option("--scene-height", scene_height, "Scene height")
      ->check(CLI::Range(240, 2160));

  // Read display options
  bool out_gst{false};
  app.add_flag("--output-gstreamer", out_gst, "Enable GStreamer output");
  bool out_display{false};
  app.add_flag("--output-display", out_display, "Enable output to display");

  // Read GStreamer definition
  std::string gst_pipe_def;
  app.add_option("-g,--gst-pipeline", gst_pipe_def,
                 "GStreamer pipeline definition");
  int gst_fourcc{0};
  app.add_option("-c,--gst-fourcc", gst_fourcc, "GStreamer fourcc number");
  int gst_fps{30};
  app.add_option("-f,--gst-fps", gst_fps, "GStreamer frames per second");
  int gst_width{640};
  app.add_option("--gst-width", gst_width, "GStreamer frame width");
  int gst_height{480};
  app.add_option("--gst-height", gst_height, "GStreamer frame height");

  CLI11_PARSE(app, argc, argv);

  // Validate given arguments
  if (!out_gst && !out_display) {
    std::cout << "[E] choose at least one output" << std::endl;
    exit(app.exit(CLI::CallForHelp()));
  }

  // Create GStreamer output
  std::unique_ptr<cv::VideoWriter> writer;
  if (out_gst) {
    if (gst_pipe_def.empty()) {
      std::cout << "Using GStreamer Pipeline:\n"
                << DefaultWriterGstPipeline << std::endl;
      writer = DefaultVideoWriter(scene_width, scene_height);
    } else {
      std::cout << "Using GStreamer Pipeline:\n" << gst_pipe_def << std::endl;
      writer = std::make_unique<cv::VideoWriter>(
          gst_pipe_def, 0, 30, cv::Size(scene_width, scene_height), true);
    }

    if (!writer->isOpened()) {
      std::cerr << "VideoWriter not opened" << std::endl;
      exit(-1);
    }
  }

  // Create source
  std::unique_ptr<SimpleScreensaver> scr_saver =
      std::make_unique<SimpleScreensaver>(cv::Size(scene_width, scene_height));
  if (path_to_logo.empty()) {
    scr_saver->setLogo(cv::Mat());
  } else {
    cv::Mat logo = cv::imread(path_to_logo, cv::IMREAD_COLOR);
    scr_saver->setLogo(logo);
  }
  scr_saver->restart();

  cv::Mat canvas;
  std::cout << "[I] press q or Ctrl+c to exit" << std::endl;
  while (true) {
    scr_saver->next(canvas);

    if (writer != nullptr) {
      writer->write(canvas);
    }
    if (out_display) {
      cv::imshow("Sender - Display", canvas);
    }
    int key = cv::waitKey(33);
    if (key == 'q' || key == 'Q') {
      break;
    }
  }

  return 0;
}