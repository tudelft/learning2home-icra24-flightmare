//
// This bridge was originally from FlightGoggles.
// We made several changes on top of it.
//
#pragma once

// std libs
#include <unistd.h>
#include <experimental/filesystem>
#include <fstream>
#include <map>
#include <string>
#include <unordered_map>
#include <cmath>

// opencv
#include <opencv2/imgproc/types_c.h>

// Include ZMQ bindings for communications with Unity.
#include <zmqpp/zmqpp.hpp>

// flightlib
#include "flightlib/bridges/unity_message_types.hpp"
#include "flightlib/common/logger.hpp"
#include "flightlib/common/math.hpp"
#include "flightlib/common/quad_state.hpp"
#include "flightlib/common/types.hpp"
#include "flightlib/objects/quadrotor.hpp"
#include "flightlib/objects/static_object.hpp"
#include "flightlib/objects/unity_camera.hpp"
#include "flightlib/sensors/rgb_camera.hpp"

#define PI 3.1415926535897932384626433832795028841971

using json = nlohmann::json;

namespace flightlib {

class PyBridge {
 public:
  // constructor & destructor
  PyBridge();
  ~PyBridge(){};

  // connect function
  bool connectUnity(const SceneID scene_id);
  bool disconnectUnity(void);

  // public get functions
  bool getRender(const FrameID frame_id);
  bool getRender2(const FrameID frame_id);
  bool handleOutput();
  bool handleOutputDelayed(const FrameID frame_id);
  bool handleEvaluationOutput();
  std::string getHomeVectorCoords();
  bool getPointCloud(PointCloudMessage_t &pointcloud_msg,
                     Scalar time_out = 600.0);

  // public set functions
  bool setScene(const SceneID &scene_id);

  // add object
  bool addQuadrotor(std::shared_ptr<Quadrotor> quad);
  bool addCamera(std::shared_ptr<UnityCamera> unity_camera);
  bool addStaticObject(std::shared_ptr<StaticObject> static_object);

  // public auxiliary functions
  inline void setPubPort(const std::string &pub_port) { pub_port_ = pub_port; };
  inline void setSubPort(const std::string &sub_port) { sub_port_ = sub_port; };
  // create unity bridge
  static std::shared_ptr<PyBridge> getInstance(void) {
    static std::shared_ptr<PyBridge> bridge_ptr =
      std::make_shared<PyBridge>();
    return bridge_ptr;
  };

 private:
  bool initializeConnections(void);

  //
  SettingsMessage_t settings_;
  PubMessage_t pub_msg_;
  Logger logger_{"PyBridge"};

  std::vector<std::shared_ptr<Quadrotor>> unity_quadrotors_;
  std::vector<std::shared_ptr<RGBCamera>> rgb_cameras_;
  std::vector<std::shared_ptr<StaticObject>> static_objects_;

  // ZMQ variables and functions
  std::string client_address_;
  std::string pub_port_;
  std::string sub_port_;
  zmqpp::context context_;
  zmqpp::socket pub_{context_, zmqpp::socket_type::publish};
  zmqpp::socket sub_{context_, zmqpp::socket_type::subscribe};
  bool sendInitialSettings(void);
  bool handleSettings(void);

  // timing variables
  int64_t num_frames_;
  int64_t last_downloaded_utime_;
  int64_t last_download_debug_utime_;
  int64_t u_packet_latency_;

  // axuiliary variables
  const Scalar unity_connection_time_out_{120.0};
  bool unity_ready_{false};

  // variable to hold the home vector coords
  std::string home_vector_coords;

  // bool to prevent from accepting home vector duplicates
  bool accept_done = false;
};
}  // namespace flightlib