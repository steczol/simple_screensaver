#include "../include/CLI11.hpp"
//
#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
//
#include <random>

template <typename T, typename N>
cv::Rect_<T> move(const cv::Rect_<T>& r, N step_x, N step_y) {
  return cv::Rect_<T>(
      cv::Point_<T>(static_cast<T>(r.x + step_x), static_cast<T>(r.y + step_y)),
      r.size());
}

// template <typename T, typename N>
// void resize(cv::Rect_<T>& r, N sz) {
//   r = cv::Rect_<T>(cv::Point_<T>(r.x, r.y),
//                    cv::Size(static_cast<T>(sz), static_cast<T>(sz)));
// }

int main(int argc, char** argv) {
  CLI::App app{"Simple screensaver"};
  std::string path_to_logo;
  app.add_option("-l,--logo", path_to_logo, "Path to logo");
  CLI11_PARSE(app, argc, argv);

  std::cout << "path to logo: " << path_to_logo << std::endl;

  // GStreamer
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

  // Geometry
  int scene_width = 640;
  int scene_height = 480;
  cv::Mat canvas = cv::Mat::zeros(cv::Size(scene_width, scene_height), CV_8UC3);

  // read logo
  cv::Mat logo;
  if (path_to_logo.empty()) {
    int width = 50;
    int height = 50;
    logo = cv::Mat::ones(cv::Size(width, height), CV_8UC3);
    logo.setTo(cv::Scalar(255, 255, 255));
  } else {
    logo = cv::imread(path_to_logo, cv::IMREAD_COLOR);
    std::cout << "logo read | size: " << logo.size() << std::endl;
    // make sure the logo fits into window
    while (logo.cols >= 0.6 * scene_width || logo.rows >= 0.6 * scene_height) {
      cv::resize(logo, logo, cv::Size(), 0.5, 0.5, cv::INTER_LINEAR);
      std::cout << "logo resized | logo.size(): " << logo.size() << std::endl;
    }
  }
  cv::Mat blank = cv::Mat::zeros(logo.size(), logo.type());
  std::cout << "blank: " << blank.size() << std::endl;

  // cv::Mat logo = cv::Mat::ones(cv::Size(logo_width, logo_height), CV_8UC3);
  // cv::Mat blank = cv::Mat::zeros(cv::Size(logo_width, logo_height), CV_8UC3);
  // cv::Mat canvas = cv::Mat::zeros(cv::Size(scene_width, scene_height),
  // CV_8UC3);

  // define random number generators
  std::random_device rd{};
  std::mt19937 gen{rd()};
  std::uniform_int_distribution<> f_roi_start_x(0, scene_width - logo.cols - 1);
  std::uniform_int_distribution<> f_roi_start_y(0,
                                                scene_height - logo.rows - 1);

  std::uniform_real_distribution<double> f_angle(0.0, 2 * M_PI);
  std::normal_distribution<double> f_roi_size(50.0, 20.0);
  // Create roi
  // double roi_size = std::max(0.0, f_roi_size(gen));
  cv::Rect2i roi =
      cv::Rect2i(cv::Point2i(f_roi_start_x(gen), f_roi_start_y(gen)),
                 cv::Size(logo.cols, logo.rows));
  cv::Rect2i new_roi;

  double angle = f_angle(gen);
  // std::cout << "angle: " << angle << std::endl;
  double v_x = std::cos(angle);
  double v_y = std::sin(angle);
  // std::cout << "v_x: " << v_x << " | v_y: " << v_y << std::endl;
  // Define speed
  double steps = 7.0;

  logo.copyTo(canvas(roi));

  while (true) {
    blank.copyTo(canvas(roi));

    new_roi = move(roi, v_x * steps, v_y * steps);
    // std::cout << "new_roi: " << new_roi << std::endl;
    // bounce horizontally
    if (new_roi.br().x >= scene_width || new_roi.tl().x < 0) {
      v_x = -v_x;
      // resize(roi, roi_size);
      new_roi = move(roi, v_x * steps, v_y * steps);
    }
    // bounce vertically
    if (new_roi.br().y >= scene_height || new_roi.tl().y < 0) {
      v_y = -v_y;
      // resize(roi, roi_size);
      new_roi = move(roi, v_x * steps, v_y * steps);
    }
    roi = new_roi;
    // std::cout << roi << " | v_x: " << v_x << ", v_y: " << v_y << std::endl;
    logo.copyTo(canvas(roi));
    writer.write(canvas);
    cv::imshow("Screensaver", canvas);
    int key = cv::waitKey(33);
    if (key == 'q' || key == 'Q') {
      break;
    }
  }

  return 0;
}