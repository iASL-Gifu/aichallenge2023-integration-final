#include "aichallenge_scoring_msgs/msg/score.hpp"

#include "rclcpp/rclcpp.hpp"

#include "geometry_msgs/msg/point.hpp"
#include "autoware_auto_vehicle_msgs/msg/engage.hpp"
#include "autoware_auto_vehicle_msgs/msg/control_mode_report.hpp"
#include "autoware_auto_vehicle_msgs/srv/control_mode_command.hpp"

#include <cstdio>
#include <iostream>
#include <fstream>

class ScoringResult : public rclcpp::Node
{
public:
  ScoringResult() : Node("score_node") {
    // Subscribers
    score_subscriber = this->create_subscription<aichallenge_scoring_msgs::msg::Score>(
      "/aichallenge_scoring/score", 1, std::bind(&ScoringResult::scoreCallback, this, std::placeholders::_1));
    vehicle_control_mode_sub_ = this->create_subscription<autoware_auto_vehicle_msgs::msg::ControlModeReport>(
      "/vehicle/status/control_mode", 1, std::bind(&ScoringResult::vehicleControlModeCallback, this, std::placeholders::_1));
    
    // Publishers
    engage_pub_ = this->create_publisher<autoware_auto_vehicle_msgs::msg::Engage>("/autoware/engage", 1);

    // Service Clients
    control_mode_client_ = this->create_client<autoware_auto_vehicle_msgs::srv::ControlModeCommand>("/control/control_mode_request");
  }

private:

  void vehicleControlModeCallback(const autoware_auto_vehicle_msgs::msg::ControlModeReport& msg) {

    // If mode is AUTONOMOUS=1
    if (msg.mode == 1) {
      is_autonomous_mode_ = true;
      is_overridden_ = false;
      is_result_generated_ = false;
    }

    // If already been in AUTONOMOUS mode and mode changed to MANUAL=4
    if (is_autonomous_mode_ && msg.mode == 4) {
      std::cout << "Overriden by Driver." << std::endl;
      is_overridden_ = true;
    }
  }

  void scoreCallback(const aichallenge_scoring_msgs::msg::Score& msg) {
    if (is_result_generated_)
      return;
    
    const auto has_finished =
      msg.is_outside_lane || msg.is_timeout ||
      msg.has_exceeded_speed_limit || msg.has_finished_task3 ||
      is_overridden_;
    if (!has_finished)
      return;

    std::cout << "Scoring completed." << std::endl;
    writeResultJson(msg);

    is_result_generated_ = true;

    // Disengage Autoware
    auto disengage_autoware_msg = autoware_auto_vehicle_msgs::msg::Engage();
    disengage_autoware_msg.engage = false;
    engage_pub_->publish(disengage_autoware_msg);

    // Disable Vehicle
    const auto request = std::make_shared<autoware_auto_vehicle_msgs::srv::ControlModeCommand::Request>();
    request->stamp = now();
    request->mode = 4;
    control_mode_client_->async_send_request(request);

    is_autonomous_mode_ = false;
  }

  float calculateDistanceScore(const aichallenge_scoring_msgs::msg::Score& score_msg) {
    if (score_msg.has_exceeded_speed_limit)
      return 0.0f;

    auto penalty_ratio = 1.0f;

    if (!score_msg.has_finished_task1)
      penalty_ratio -= 0.05f;

    // Add penalty if the vehicle goes out of lane after it completes the course
    if (score_msg.is_distance_score_maxed_out && score_msg.is_outside_lane)
      penalty_ratio -= 0.05f;

    return score_msg.distance_score * penalty_ratio;
  }

  void writeResultJson(const aichallenge_scoring_msgs::msg::Score& score_msg) {

    // Get time and use it to name the result.json filename
    time_t t = time(0);
    struct tm * now = localtime(&t);
    char filename[50];
    strftime(filename, 50, "results/%Y-%m-%d_%H-%M-%S_result.json", now);

    std::ofstream ofs(filename);
    ofs << "{" << std::endl;
    ofs << "  \"rawDistanceScore\": " << score_msg.distance_score << "," << std::endl;
    ofs << "  \"distanceScore\": " << calculateDistanceScore(score_msg) << "," << std::endl;
    ofs << "  \"task3Duration\": " << score_msg.task3_duration << "," << std::endl;
    ofs << std::boolalpha << "  \"isOutsideLane\": " << score_msg.is_outside_lane << "," << std::endl;
    ofs << std::boolalpha << "  \"isTimeout\": " << score_msg.is_timeout << "," << std::endl;
    ofs << std::boolalpha << "  \"hasExceededSpeedLimit\": " << score_msg.has_exceeded_speed_limit << "," << std::endl;
    ofs << std::boolalpha << "  \"isOverridden\": " << is_overridden_ << "," << std::endl;
    ofs << std::boolalpha << "  \"hasFinishedTask1\": " << score_msg.has_finished_task1 << "," << std::endl;
    ofs << std::boolalpha << "  \"hasFinishedTask2\": " << score_msg.has_finished_task2 << "," << std::endl;
    ofs << std::boolalpha << "  \"hasFinishedTask3\": " << score_msg.has_finished_task3 << std::endl;
    ofs << "}" << std::endl;
    ofs.close();
  }

  // Subscribers
  rclcpp::Subscription<aichallenge_scoring_msgs::msg::Score>::SharedPtr score_subscriber;
  rclcpp::Subscription<autoware_auto_vehicle_msgs::msg::ControlModeReport>::SharedPtr vehicle_control_mode_sub_;

  // Publishers
  rclcpp::Publisher<autoware_auto_vehicle_msgs::msg::Engage>::SharedPtr engage_pub_;

  // Service Clients
  rclcpp::Client<autoware_auto_vehicle_msgs::srv::ControlModeCommand>::SharedPtr control_mode_client_;

  rclcpp::TimerBase::SharedPtr timer;

  // Internal states
  bool is_result_generated_ = false;
  bool is_autonomous_mode_ = false;
  bool is_overridden_ = false;
};

int main(int argc, char ** argv) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<ScoringResult>());
  rclcpp::shutdown();
  return 0;
}
