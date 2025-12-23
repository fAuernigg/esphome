#pragma once

#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/components/pn532/pn532.h"
#include "esphome/components/binary_sensor/binary_sensor.h"

namespace esphome {
namespace pn532_watchdog {

class PN532Watchdog : public Component {
 public:
  void set_pn532(pn532::PN532 *pn532) {
      this->pn532_ = pn532;
      pn532_->add_error_callback([this](bool err) {
        if (this->error_sensor_ != nullptr)
          this->error_sensor_->publish_state(err);
        if (err)
          this->timeout_trigger_.trigger();
      });
  }

  void set_timeout(uint32_t timeout_ms) { this->timeout_ms_ = timeout_ms; }

  void set_error_sensor(binary_sensor::BinarySensor *sensor) {
    this->error_sensor_ = sensor;
  }

  void loop() override;

 protected:
  pn532::PN532 *pn532_{nullptr};
  binary_sensor::BinarySensor *error_sensor_{nullptr};

  uint32_t timeout_ms_{30000};
  bool timed_out_{false};
  bool init_done_{false};
  bool triggered_{false};
  Trigger<> timeout_trigger_;

  Trigger<> *get_timeout_trigger() { return &this->timeout_trigger_; }
};

}  // namespace pn532_watchdog
}  // namespace esphome
