/*
 * Copyright (C) 2022 Michael Ferguson
 * Copyright (C) 2014-2017 Fetch Robotics Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ROBOT_CALIBRATION_CAPTURE_PLANE_FINDER_H
#define ROBOT_CALIBRATION_CAPTURE_PLANE_FINDER_H

#include <ros/ros.h>
#include <robot_calibration/capture/depth_camera.h>
#include <robot_calibration/eigen_geometry.h>
#include <robot_calibration/plugins/feature_finder.h>
#include <robot_calibration_msgs/CalibrationData.h>
#include <cv_bridge/cv_bridge.h>
#include <tf2_ros/transform_listener.h>

namespace robot_calibration
{
/**
 * @brief Finds the largest plane in a point cloud.
 */
class PlaneFinder : public FeatureFinder
{
public:
  PlaneFinder();
  virtual ~PlaneFinder() = default;
  virtual bool init(const std::string& name, ros::NodeHandle & n);
  virtual bool find(robot_calibration_msgs::CalibrationData * msg);

protected:
  /**
   * @brief ROS callback - updates cloud_ and resets waiting_ to false
   */
  virtual void cameraCallback(const sensor_msgs::PointCloud2& cloud);

  /**
   * @brief Remove invalid points from a cloud
   * @param cloud The point cloud to remove invalid points from
   *
   * Invalid points include:
   *  * points with NAN values
   *  * points with infinite values
   *  * points with z-distance of 0
   *  * points outside our min/max x,y,z parameters
   */
  virtual void removeInvalidPoints(sensor_msgs::PointCloud2& cloud,
                                   double min_x, double max_x, double min_y, double max_y, double min_z, double max_z);

  /**
   * @brief Extract a plane from the point cloud
   * @brief cloud The cloud to extract plane from - non-plane points will remain
   * @return New point cloud comprising only the points in the plane
   */
  virtual sensor_msgs::PointCloud2 extractPlane(sensor_msgs::PointCloud2& cloud);

  /**
   * @brief Extract points from a cloud into a calibration message
   * @param sensor_name Used to fill in observation "sensor_name"
   * @param cloud Point cloud from which to extract observations
   * @param msg CalibrationData to fill with observation
   * @param publisher Optional pointer to publish the observations as a cloud
   */
  virtual void extractObservation(const std::string& sensor_name,
                                  const sensor_msgs::PointCloud2& cloud,
                                  robot_calibration_msgs::CalibrationData * msg,
                                  ros::Publisher* publisher);

  /**
   * @brief Wait until a new cloud has arrived
   */
  virtual bool waitForCloud();

  ros::Subscriber subscriber_;
  ros::Publisher publisher_;

  tf2_ros::Buffer tf_buffer_;
  tf2_ros::TransformListener tf_listener_;

  bool waiting_;
  sensor_msgs::PointCloud2 cloud_;
  DepthCameraInfoManager depth_camera_manager_;

  // See init() function for parameter definitions
  std::string plane_sensor_name_;
  int points_max_;
  double initial_sampling_distance_;
  double plane_tolerance_;
  double min_x_, max_x_;
  double min_y_, max_y_;
  double min_z_, max_z_;
  Eigen::Vector3d desired_normal_;
  double cos_normal_angle_;
  std::string transform_frame_;
  int ransac_iterations_;
  int ransac_points_;

  bool output_debug_;
};

}  // namespace robot_calibration

#endif  // ROBOT_CALIBRATION_CAPTURE_PLANE_FINDER_H
