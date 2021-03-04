/*
 *
 * (C) 2013-21 - ntop.org
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 */

#include "ntop_includes.h"

FlowCallbacksLoader::FlowCallbacksLoader(){
  registerFlowCallbacks();
  reloadFlowCallbacks();
}

/* **************************************************** */

FlowCallbacksLoader::~FlowCallbacksLoader() {
  for(list<FlowCallback*>::const_iterator it = cb_all.begin(); it != cb_all.end(); ++it)
    delete *it;
}

/* **************************************************** */

template<typename T> void FlowCallbacksLoader::registerFlowCallback(json_object *json) {
  T *t = new (nothrow) T(json);

  if(t) {
    cb_all.push_back(t);

    if(t->hasCallback(flow_lua_call_protocol_detected)) cb_protocol_detected.push_back(t);
    if(t->hasCallback(flow_lua_call_periodic_update))   cb_periodic_update.push_back(t);
    if(t->hasCallback(flow_lua_call_idle))              cb_idle.push_back(t);
  }
}

/* **************************************************** */

void FlowCallbacksLoader::registerFlowCallbacks() {
  /* Add lines here for each new callback added under flow_callbacks/ */
  cb_registrable["blacklisted"] = &FlowCallbacksLoader::registerFlowCallback<BlacklistedFlowCallback>;
  cb_registrable["long_lived"] = &FlowCallbacksLoader::registerFlowCallback<LongLivedFlowCallback>;
  cb_registrable["low_goodput"] = &FlowCallbacksLoader::registerFlowCallback<LowGoodputFlowCallback>;
}

/* **************************************************** */

void FlowCallbacksLoader::reloadFlowCallbacks() {
  json_object *json = NULL, *json_config, *json_config_flow;
  struct json_object_iterator it;
  struct json_object_iterator itEnd;
  enum json_tokener_error jerr = json_tokener_success;
  char *value = NULL;
  registerFunction register_function;
  u_int actual_len = ntop->getRedis()->len(FLOW_CALLBACKS_CONFIG);

  if((value = (char *) malloc(actual_len + 1)) == NULL) {
    ntop->getTrace()->traceEvent(TRACE_ERROR, "Unable to allocate memory to deserialize %s", FLOW_CALLBACKS_CONFIG);
    goto out;
  }

  if(ntop->getRedis()->get((char*)FLOW_CALLBACKS_CONFIG, value, actual_len + 1) != 0) {
    ntop->getTrace()->traceEvent(TRACE_ERROR, "Unable to find configuration %s", FLOW_CALLBACKS_CONFIG);
    goto out;
  }

  if((json = json_tokener_parse_verbose(value, &jerr)) == NULL) {
    ntop->getTrace()->traceEvent(TRACE_ERROR, "JSON Parse error [%s] %s [len: %u][strlen: %u]", json_tokener_error_desc(jerr), value, actual_len, strlen(value));
    goto out;
  }

  if(!json_object_object_get_ex(json, "config", &json_config)) { /* 'config' section inside the JSON */
    ntop->getTrace()->traceEvent(TRACE_ERROR, "'config' not found in JSON");
    goto out;
  }

  if(!json_object_object_get_ex(json_config, "flow", &json_config_flow)) { /* 'flow' section inside 'config' JSON */
    ntop->getTrace()->traceEvent(TRACE_ERROR, "'flow' not found in 'config' JSON");
    goto out;
  }

  /*
    Iterate over all script configurations
  */
  it = json_object_iter_begin(json_config_flow);
  itEnd = json_object_iter_end(json_config_flow);

  while(!json_object_iter_equal(&it, &itEnd)) {
    /* Configuration load, i.e., if script_key is enabled, create instance */
    const char *callback_key = json_object_iter_peek_name(&it);
    json_object *callback_conf = json_object_iter_peek_value(&it);

    json_object *json_hook_all, *json_enabled, *json_script_conf;
    bool enabled = false;
    if(json_object_object_get_ex(callback_conf, "all", &json_hook_all)        /*  Hook 'all' present                     */
       && json_object_object_get_ex(json_hook_all, "enabled", &json_enabled)  /* 'enabled' present in hook configuration */
       && (enabled = json_object_get_boolean(json_enabled))) {
      json_object_object_get_ex(json_hook_all, "script_conf", &json_script_conf);

      if(cb_registrable.find(callback_key) != cb_registrable.end()) {
	register_function = cb_registrable[callback_key];
	(this->*register_function)(json_script_conf);
	ntop->getTrace()->traceEvent(TRACE_NORMAL, "Enabled callback: %s", callback_key);
      }
    }
    /* Move to the next element */
    json_object_iter_next(&it);
  }

 out:
  /* Free the json */
  if(json)  json_object_put(json);
  if(value) free(value);
}

/* **************************************************** */

list<FlowCallback*> FlowCallbacksLoader::getFlowCallbacks(NetworkInterface *iface, FlowLuaCall flow_lua_call) {
  list<FlowCallback*> res;
  list<FlowCallback*> *selected_list;

  switch(flow_lua_call) {
  case flow_lua_call_protocol_detected:
    selected_list = &cb_protocol_detected;
  case flow_lua_call_periodic_update:
    selected_list = &cb_periodic_update;
  case flow_lua_call_idle:
    selected_list = &cb_idle;
  default:
    selected_list = NULL;
  }

  if(selected_list) {
    for(list<FlowCallback*>::const_iterator it = selected_list->begin(); it != selected_list->end(); ++it) {
      /* Check conditions on iface, e.g., iface->isPacketInterface() */
      res.push_back(*it);
    }
  }

  return res;
}

/* **************************************************** */

void FlowCallbacksLoader::printCallbacks() {
  ntop->getTrace()->traceEvent(TRACE_NORMAL, "Registrable Callbacks:");
  for(cb_map_t::const_iterator it = cb_registrable.begin(); it != cb_registrable.end(); ++it) {
    ntop->getTrace()->traceEvent(TRACE_NORMAL, "\t[%s][%p]", it->first.c_str(), it->second);
  }

  ntop->getTrace()->traceEvent(TRACE_NORMAL,
			       "Registered Callbacks [all: %u][protocol_detected: %u][periodic_update: %u][idle: %u]",
			       cb_all.size(), cb_protocol_detected.size(), cb_periodic_update.size(), cb_idle.size());
}

/* **************************************************** */

