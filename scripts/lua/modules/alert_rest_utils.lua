--
-- (C) 2014-21 - ntop.org
--

local dirs = ntop.getDirs()
package.path = dirs.installdir .. "/scripts/lua/modules/pools/?.lua;" .. package.path

local json = require "dkjson"
local rest_utils = require "rest_utils"
local user_scripts = require "user_scripts"
local alert_utils = require "alert_utils"
local hosts_control = require "hosts_control"

local alert_rest_utils = {}

-- #################################

-- @brief exclude an alert using the parameters that the POST has
function _exclude_flow_alert(additional_filters, delete_alerts)
   local alert_key = tonumber(_POST["alert_key"])

   -- Getting the parameters
   success, new_filter = user_scripts.parseFilterParams(additional_filters, "flow", false)

   if success then
      for _, filter in pairs(new_filter.new_filters) do
	 for _, host_ip in pairs(filter) do
	    -- Disable host for the current filter
	    hosts_control.disable_alert(host_ip, alert_key)
	    if delete_alerts == "true" then
	       alert_utils.deleteFlowAlertsMatching(host_ip, alert_key)
	    end
	 end
      end
   else
      -- Error while parsing the params, error is printed
      update_err = new_filter
   end

   if success then
      rc = rest_utils.consts.success.ok
      rest_utils.answer(rc)
   else
      rc = rest_utils.consts.err.invalid_args
      res = update_err
      rest_utils.answer(rc, res)
   end
end

-- #################################

-- @brief exclude an alert using the parameters that the POST has
function alert_rest_utils.exclude_alert()
   -- POST parameters
   local additional_filters = _POST["filters"]
   local subdir = _POST["subdir"]
   local script_key = _POST["script_key"]
   local delete_alerts = _POST["delete_alerts"] or "false"
   
   -- Parameters used by the various functions
   local success = ""
   local new_filter  = {}
   local update_err = ""

   -- Parameters used for the rest answer
   local rc = ""
   local res = ""
   
   -- Checking that all parameters where given to the POST
   if not additional_filters or not subdir or not script_key then
      rest_utils.answer(rest_utils.consts.err.invalid_args)
      return
   end

   if subdir == "flow" then
      return _exclude_flow_alert(additional_filters, delete_alerts)
   end

   -- Getting the parameters
   success, new_filter = user_scripts.parseFilterParams(additional_filters, subdir, false)
      
   if success then
      success, update_err = user_scripts.updateScriptConfig(script_key, subdir, nil, nil, new_filter)
   else
      -- Error while parsing the params, error is printed
      update_err = new_filter
   end

   if success then
      if delete_alerts == "true" then
	 alert_utils.deleteAlertsMatchingUserScriptFilter(subdir, script_key, new_filter.new_filters[1])                                                                            
      end
      
      rc = rest_utils.consts.success.ok
      rest_utils.answer(rc)
   else
      rc = rest_utils.consts.err.invalid_args
      res = update_err
      rest_utils.answer(rc, res)
   end
end

-- #################################

return alert_rest_utils
