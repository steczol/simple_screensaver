#include "../include/CLI11.hpp"
#include "../lib/SimpleScreensaver.hpp"
//
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
//
#include <random>
// debug
#include <random>
#include <ratio>
#include <thread>

/*
  DEBUG
*/
using TimePointHighPrecision =
    std::chrono::time_point<std::chrono::high_resolution_clock>;
TimePointHighPrecision tick() {
  return std::chrono::high_resolution_clock::now();
}

long tock_us(const TimePointHighPrecision& start) {
  auto stop = std::chrono::high_resolution_clock::now();
  auto diff =
      std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
  return diff.count();
}
/*
 END DEBUG
*/

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
          gst_pipe_def, /*fourcc*/ 0, /*fps*/ 30,
          cv::Size(scene_width, scene_height), /*isColor*/ true);
    }

    if (!writer->isOpened()) {
      std::cerr << "VideoWriter not opened" << std::endl;
      EXIT_FAILURE;
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

  int counter = 0;
  while (true) {
    auto now = std::chrono::system_clock::now();
    auto wake_time = now + std::chrono::milliseconds(33);

    // auto tick_generator = tick();
    scr_saver->next(canvas);
    // auto tock_generator = tock_us(tick_generator);
    // std::cout << counter << " generator took " << tock_generator << " us"
    //           << std::endl;

    if (writer != nullptr) {
      // auto tick_writer = tick();
      writer->write(canvas);
      // auto tock_writer = tock_us(tick_writer);
      // std::cout << counter << " writer took " << tock_writer << " us"
      //           << std::endl;
    }

    if (out_display) {
      // auto tick_imshow = tick();
      cv::imshow("Sender - Display", canvas);
      // auto tock_imshow = tock_us(tick_imshow);
      // std::cout << counter << " imshow took " << tock_imshow << " us"
      //           << std::endl;
      int key = cv::waitKey(1);
      if (key == 'q' || key == 'Q') {
        break;
      }
    }

    std::this_thread::sleep_until(wake_time);

    // auto elapsed = std::chrono::high_resolution_clock::now() - now;
    // auto milliseconds =
    //     std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
    // std::cout << counter << " loop time: " << milliseconds << " ms\n";
    counter++;
  }

  return 0;
}