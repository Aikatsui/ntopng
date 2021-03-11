--
-- (C) 2020 - ntop.org
--

local alerts_api = require("alerts_api")
local alert_severities = require "alert_severities"
local user_scripts = require("user_scripts")
local alert_consts = require("alert_consts")

local script

-- #################################################################

script = {
  packet_interface_only = true,
  
  -- Script category
  category = user_scripts.script_categories.network,

  -- NB atm working only for packet interfaces
  packet_interface_only = true,
  l4_proto = "tcp",

   -- This script is only for alerts generation
   is_alert = true,

  default_value = {
   severity = alert_severities.warning,
  },
  
  gui = {
    i18n_title        = "zero_tcp_window.zero_tcp_window_title",
    i18n_description  = "zero_tcp_window.zero_tcp_window_description",
  }
}

-- #################################################################

return script
