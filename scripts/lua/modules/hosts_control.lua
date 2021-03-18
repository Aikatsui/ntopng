--
-- (C) 2017-21 - ntop.org
--
-- Module to keep things in common across hosts_control of various type

local dirs = ntop.getDirs()
package.path = dirs.installdir .. "/scripts/lua/modules/?.lua;" .. package.path

require "lua_utils"
local alert_consts = require "alert_consts"
local json = require "dkjson"

-- ##############################################

local hosts_control = {}

-- ##############################################

local function _get_hosts_control_prefix_key()
   local key = string.format("ntopng.prefs.hosts_control")

   return key
end

-- ##############################################

local function _get_hosts_control_lock_key()
   local key = string.format("ntopng.cache.hosts_control.hosts_control_lock")

   return key
end

-- ##############################################

local function _get_hosts_control_disabled_alert_key(host_ip, alert_key)
   local key = string.format("%s.host_%s.disabled_alert_%u", _get_hosts_control_prefix_key(), host_ip, alert_key)

   return key
end

-- ##############################################

local function _lock()
   local max_lock_duration = 5 -- seconds
   local max_lock_attempts = 5 -- give up after at most this number of attempts
   local lock_key = _get_hosts_control_lock_key()

   for i = 1, max_lock_attempts do
      local value_set = ntop.setnxCache(lock_key, "1", max_lock_duration)

      if value_set then
	 return true -- lock acquired
      end

      ntop.msleep(1000)
   end

   return false -- lock not acquired
end

-- ##############################################

local function _unlock()
   ntop.delCache(_get_hosts_control_lock_key())
end

-- ##############################################

local function _check_host_ip_alert_key(host_ip, alert_key)
   if not isIPv4(host_ip) and not isIPv6(host_ip) then
      -- Invalid host submitted
      return false
   end

   if not alert_consts.alertTypeRaw(tonumber(alert_key)) then
      -- Invalid alert key submitted
      return false
   end

   return true
end

-- ##############################################

-- @brief Returns true if `host_ip` has the alert identified with `alert_key` disabled
function hosts_control.has_disabled_alert(host_ip, alert_key)
   local control_key = _get_hosts_control_disabled_alert_key(host_ip, alert_key)

   return ntop.getPref(control_key) == "1"
end

-- ##############################################

local function _disable_alert(host_ip, alert_key)
   local control_key = _get_hosts_control_disabled_alert_key(host_ip, alert_key)

   ntop.setPref(control_key, "1") -- Add the preference
   ntop.reloadControlGroups()     -- Tell ntopng to reload
end

-- ##############################################

local function _enable_alert(host_ip, alert_key)
   local control_key = _get_hosts_control_disabled_alert_key(host_ip, alert_key)

   ntop.delCache(control_key) -- Remove the preference
   ntop.reloadControlGroups() -- Tell ntopng to reload
end

-- ##############################################

--@brief Marks an alert as disabled for a given host identified with `host_ip`
--@return True, if alert is disabled with success, false otherwise
function hosts_control.disable_alert(host_ip, alert_key)
   local ret = false

   if not _check_host_ip_alert_key(host_ip, alert_key) then
      -- Invalid params submitted
      return false
   end

   local locked = _lock()

   -- If here, host_ip has been found
   if locked then
      if not hosts_control.has_disabled_alert(host_ip, alert_key) then
	 _disable_alert(host_ip, alert_key)
      end

      ret = true
      _unlock()
   end

   return ret
end

-- ##############################################

--@brief Marks an alert as disabled for a given control group identified with `host_ip`
--@return True, if alert is disabled with success, false otherwise
function hosts_control.enable_alert(host_ip, alert_key)
   local ret = false

   if not _check_host_ip_alert_key(host_ip, alert_key) then
      -- Invalid params submitted
      return false
   end

   local locked = _lock()

   -- If here, host_ip has been found
   if locked then
      if hosts_control.has_disabled_alert(host_ip, alert_key) then
	 _enable_alert(host_ip, alert_key)
      end

      ret = true
      _unlock()
   end

   return ret
end

-- ##############################################

-- @brief Delete all hosts_control
function hosts_control.cleanup()
   local locked = _lock()

   if locked then
      local keys = ntop.getKeysCache(_get_hosts_control_prefix_key()..".*")

      for k in pairs(keys or {}) do
	 ntop.delCache(k)
      end

      _unlock()

      ntop.reloadControlGroups() -- Tell ntopng to reload
   end
end

-- ##############################################

return hosts_control
