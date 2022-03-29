#include <iostream>
#include <opencv2/highgui.hpp>
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
// void moveRight(cv::Rect_<T>& r, N step) {
//   move(r, step, 0);
// }

// template <typename T, typename N>
// void moveLeft(cv::Rect_<T>& r, N step) {
//   move(r, -step, 0);
// }

// template <typename T, typename N>
// void moveUp(cv::Rect_<T>& r, N step) {
//   move(r, 0, -step);
// }

// template <typename T, typename N>
// void moveDown(cv::Rect_<T>& r, N step) {
//   move(r, 0, step);
// }

int main() {
  std::cout << "Simple screensaver" << std::endl;
  // Geometry
  int scene_width = 800;
  int scene_height = 600;
  int logo_width = 50;
  int logo_height = 50;

  // read logo
  // 	for now mock logo
  cv::Mat logo = cv::Mat::ones(cv::Size(logo_width, logo_height), CV_8UC3);
  cv::Mat blank = cv::Mat::zeros(cv::Size(logo_width, logo_height), CV_8UC3);
  logo.setTo(cv::Scalar(255, 255, 255));
  cv::Mat canvas = cv::Mat::zeros(cv::Size(scene_width, scene_height), CV_8UC3);

  // define random number generators
  std::random_device rd{};
  std::mt19937 gen{rd()};
  std::uniform_int_distribution<> f_roi_start_x(0,
                                                scene_width - logo_width - 1);
  std::uniform_int_distribution<> f_roi_start_y(0,
                                                scene_height - logo_height - 1);

  std::uniform_real_distribution<double> f_angle(0.0, 2 * M_PI);
  std::normal_distribution<double> f_roi_size(50.0, 20.0);
  // Create roi
  double roi_size = std::max(0.0, f_roi_size(gen));
  cv::Rect2i roi =
      cv::Rect2i(cv::Point2i(f_roi_start_x(gen), f_roi_start_y(gen)),
                 cv::Size(logo_width, logo_height));
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
      resize(roi, size);
      new_roi = move(roi, v_x * steps, v_y * steps);
    }
    // bounce vertically
    if (new_roi.br().y >= scene_height || new_roi.tl().y < 0) {
      v_y = -v_y;
      resize(roi, size);
      new_roi = move(roi, v_x * steps, v_y * steps);
    }
    roi = new_roi;
    // std::cout << roi << " | v_x: " << v_x << ", v_y: " << v_y << std::endl;
    logo.copyTo(canvas(roi));
    cv::imshow("Screensaver", canvas);
    int key = cv::waitKey();
    if (key == 'q' || key == 'Q') {
      break;
    }
  }

  return 0;
}