#include "pn532_watchdog.h"
#include "esphome/core/log.h"

namespace esphome {
namespace pn532_watchdog {

static const char *TAG = "pn532_watchdog";

void PN532Watchdog::loop() {
  if (this->pn532_ == nullptr) {
    if (!triggered_) {
      if (this->error_sensor_ != nullptr)
        this->error_sensor_->publish_state(true);
      this->timeout_trigger_.trigger();
    }
    triggered_ = true;
    return;
  }
  if (triggered_ && this->error_sensor_ != nullptr) {
    this->error_sensor_->publish_state(false);
    triggered_ = false;
  }

  uint32_t last = this->pn532_->get_last_response_ms();  // accessor required
  if (last == 0)
    return;

  uint32_t now = millis();
  bool timeout = (now - last) > this->timeout_ms_;

  if (timeout != this->timed_out_ || !this->init_done_) {
    this->timed_out_ = timeout;
    this->init_done_ = true;

    // Publish HA state
    if (this->error_sensor_ != nullptr) {
      this->error_sensor_->publish_state(timeout);
    }

    if (timeout) {
      ESP_LOGW(TAG, "PN532 timed out");
      this->timeout_trigger_.trigger();

    } else {
      ESP_LOGI(TAG, "PN532 recovered");
    }
  }
}

}  // namespace pn532_watchdog
}  // namespace esphome
