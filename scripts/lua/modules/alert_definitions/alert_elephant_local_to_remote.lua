--
-- (C) 2019-21 - ntop.org
--
--
-- (C) 2019-21 - ntop.org
--

-- ##############################################

local alert_keys = require "alert_keys"
local status_keys = require "status_keys"
-- Import the classes library.
local classes = require "classes"
-- Make sure to import the Superclass!
local alert = require "alert"

-- ##############################################

local alert_elephant_local_to_remote = classes.class(alert)

-- ##############################################

alert_elephant_local_to_remote.meta = {
   status_key = status_keys.ntopng.status_elephant_local_to_remote,
   alert_key = alert_keys.ntopng.alert_elephant_local_to_remote,
   i18n_title = "flow_details.elephant_flow_l2r",
   icon = "fas fa-exclamation",
}

-- #######################################################

-- @brief Prepare an alert table used to generate the alert
-- @param l2r_threshold Local-to-Remote threshold, in bytes, for a flow to be considered an elephant
-- @param r2l_threshold Remote-to-Local threshold, in bytes, for a flow to be considered an elephant
-- @return A table with the alert built
function alert_elephant_local_to_remote:init(l2r_threshold, r2l_threshold)
   -- Call the parent constructor
   self.super:init()

   self.alert_type_params = {
      ["elephant.l2r_threshold"] = l2r_threshold,
      ["elephant.r2l_threshold"] = r2l_threshold,
   }
end

-- #######################################################

-- @brief Format an alert into a human-readable string
-- @param ifid The integer interface id of the generated alert
-- @param alert The alert description table, including alert data such as the generating entity, timestamp, granularity, type
-- @param alert_type_params Table `alert_type_params` as built in the `:init` method
-- @return A human-readable string
function alert_elephant_local_to_remote.format(ifid, alert, alert_type_params)
   return formatElephantAlertType(alert_type_params, true --[[ l2r ]])
end

-- #######################################################

return alert_elephant_local_to_remote

-- #######################################################
