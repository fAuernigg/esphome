"""ESPHome pn532 watchdog external component"""
__version__ = "1.0.0"

import esphome.codegen as cg
import esphome.config_validation as cv
import esphome.automation as automation

from esphome.const import CONF_ID, CONF_TIMEOUT, CONF_ON_TIMEOUT
from esphome.components import pn532, binary_sensor

pn532_watchdog_ns = cg.esphome_ns.namespace("pn532_watchdog")
PN532Watchdog = pn532_watchdog_ns.class_("PN532Watchdog", cg.Component)

CONF_PN532_ID = "pn532_id"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(PN532Watchdog),

        cv.Required(CONF_PN532_ID): cv.use_id(pn532.PN532),

        cv.Optional(CONF_TIMEOUT, default="30s"):
            cv.positive_time_period_milliseconds,

        cv.Optional("error_sensor"):
            binary_sensor.binary_sensor_schema(),

        cv.Optional(CONF_ON_TIMEOUT):
            automation.validate_automation(single=True),
    }
)

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])

    pn532_var = yield cg.get_variable(config[CONF_PN532_ID])
    cg.add(var.set_pn532(pn532_var))

    cg.add(var.set_timeout(config[CONF_TIMEOUT].total_milliseconds))

    sensor = yield binary_sensor.new_binary_sensor(config["error_sensor"])
    cg.add(var.set_error_sensor(sensor))

    if CONF_ON_TIMEOUT in config:
        automation.build_automation(
            var.get_timeout_trigger(),
            [],
            config[CONF_ON_TIMEOUT],
        )

    yield cg.register_component(var, config)
