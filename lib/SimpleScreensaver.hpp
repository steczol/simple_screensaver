#ifndef SIMPLE_SCREENSAVER_HPP
#define SIMPLE_SCREENSAVER_HPP

#include <opencv2/imgproc.hpp>

// Screensaver - rectangle bouncing off the borders

class SimpleScreensaver {
 public:
  SimpleScreensaver();
  SimpleScreensaver(cv::Size sz);

  SimpleScreensaver(const SimpleScreensaver&) = delete;
  SimpleScreensaver(SimpleScreensaver&&) = delete;
  SimpleScreensaver& operator=(const SimpleScreensaver&) = delete;
  SimpleScreensaver& operator=(SimpleScreensaver&&) = delete;

  ~SimpleScreensaver() = default;

  void setLogo(cv::Mat logo);
  cv::Mat logo();

  void setSize(cv::Size sz);
  cv::Size size();

  void restart();
  void next(cv::Mat& frame);

 private:
  cv::Mat m_frame;
  cv::Mat m_logo;
  cv::Mat m_blank;

  cv::Rect2i m_logo_roi;
  double m_angle;
  double m_steps;
};

template <typename T, typename N>
cv::Rect_<T> translate(const cv::Rect_<T>& r, N step_x, N step_y) {
  return cv::Rect_<T>(
      cv::Point_<T>(static_cast<T>(r.x + step_x), static_cast<T>(r.y + step_y)),
      r.size());
}

template <typename T>
bool in_range(T val, T min, T max) {
  return min <= val && val < max;
}

#endif
