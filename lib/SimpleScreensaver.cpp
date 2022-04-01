#include "SimpleScreensaver.hpp"
#include <cmath>
#include <iostream>
#include <random>

SimpleScreensaver::SimpleScreensaver()
    : SimpleScreensaver(cv::Size(640, 480)) {}

SimpleScreensaver::SimpleScreensaver(cv::Size sz)
    : m_frame{cv::Mat::zeros(sz, CV_8UC3)},
      m_logo{cv::Mat::ones(cv::Size(50, 50), CV_8UC3)},
      m_blank{cv::Mat::zeros(cv::Size(50, 50), CV_8UC3)},
      m_logo_roi{cv::Rect2i{cv::Point2i(sz.width / 2, sz.height / 2),
                            cv::Size(50, 50)}},
      m_angle{0.25 * M_PI},
      m_steps{7.0} {}

void SimpleScreensaver::setLogo(cv::Mat logo) {
  if (logo.empty()) {
    m_logo = cv::Mat::ones(cv::Size(50, 50), CV_8UC3);
    m_logo.setTo(cv::Scalar(225, 225, 225));
  } else {
    m_logo = logo.clone();
    while (m_logo.cols >= 0.6 * m_frame.cols ||
           m_logo.rows >= 0.6 * m_frame.rows) {
      cv::resize(m_logo, m_logo, cv::Size(), 0.5, 0.5, cv::INTER_LINEAR);
    }
  }
  m_blank = cv::Mat::zeros(m_logo.size(), m_logo.type());
  m_logo_roi = cv::Rect2i(m_logo_roi.tl(), m_logo.size());
}
cv::Mat SimpleScreensaver::logo() { return m_logo.clone(); }

void SimpleScreensaver::setSize(cv::Size sz) {
  m_frame = cv::Mat::zeros(sz, CV_8UC3);
}
cv::Size SimpleScreensaver::size() { return m_frame.size(); }

void SimpleScreensaver::restart() {
  std::random_device rd{};
  std::mt19937 gen{rd()};
  std::uniform_real_distribution<double> d_uniform(0.0, 1.0);

  int roi_x =
      std::max(0, static_cast<int>(d_uniform(gen) * (m_frame.cols - 1)));
  int roi_y =
      std::max(0, static_cast<int>(d_uniform(gen) * (m_frame.rows - 1)));
  m_logo_roi = cv::Rect2i(cv::Point2i(roi_x, roi_y), m_logo.size());
  m_angle = d_uniform(gen) * 2 * M_PI;

  // double v_x = std::cos(m_angle);
  // double v_y = std::sin(m_angle);
}

void SimpleScreensaver::next(cv::Mat& frame) {
  m_blank.copyTo(m_frame(m_logo_roi));

  auto new_roi = move(m_logo_roi, std::cos(m_angle) * m_steps,
                      std::sin(m_angle) * m_steps);

  // bounce horizontally
  if (new_roi.br().x >= m_frame.cols) {
    if (in_range(m_angle, 0.0, 0.5 * M_PI)) {
      m_angle = m_angle + 0.5 * M_PI;
    } else if (in_range(m_angle, 1.5 * M_PI, 2.0 * M_PI)) {
      m_angle = m_angle - 0.5 * M_PI;
    }
    new_roi = move(m_logo_roi, std::cos(m_angle) * m_steps,
                   std::sin(m_angle) * m_steps);

  } else if (new_roi.tl().x < 0) {
    if (in_range(m_angle, 0.5 * M_PI, M_PI)) {
      m_angle = m_angle - 0.5 * M_PI;
    } else if (in_range(m_angle, M_PI, 1.5 * M_PI)) {
      m_angle = m_angle + 0.5 * M_PI;
    }
    // new_roi = move(m_logo_roi, std::cos(m_angle) * m_steps,
    //                std::sin(m_angle) * m_steps);
    new_roi = move(m_logo_roi, std::cos(m_angle) * m_steps,
                   std::sin(m_angle) * m_steps);
  }

  // bounce vertically
  if (new_roi.br().y >= m_frame.rows) {
    if (in_range(m_angle, 1.5 * M_PI, 2.0 * M_PI)) {
      m_angle = m_angle - 1.5 * M_PI;
    } else if (in_range(m_angle, 1.0 * M_PI, 1.5 * M_PI)) {
      m_angle = m_angle - 0.5 * M_PI;
    }
    // new_roi = move(m_logo_roi, std::cos(m_angle) * m_steps,
    //                std::sin(m_angle) * m_steps);
    new_roi = move(m_logo_roi, std::cos(m_angle) * m_steps,
                   std::sin(m_angle) * m_steps);
  } else if (new_roi.tl().y < 0) {
    if (in_range(m_angle, 0.0 * M_PI, 0.5 * M_PI)) {
      m_angle = m_angle + 1.5 * M_PI;
    } else if (in_range(m_angle, 0.5 * M_PI, 1.0 * M_PI)) {
      m_angle = m_angle + 0.5 * M_PI;
    }
    new_roi = move(m_logo_roi, std::cos(m_angle) * m_steps,
                   std::sin(m_angle) * m_steps);
  }

  m_logo_roi = new_roi;
  std::cout << "m_logo_roi: " << m_logo_roi << std::endl;
  m_logo.copyTo(m_frame(m_logo_roi));
  frame = m_frame;
}