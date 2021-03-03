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

/* **************************************************** */

FlowCallbacksLoader::FlowCallbacksLoader() {
};

/* **************************************************** */

FlowCallbacksLoader::~FlowCallbacksLoader() {
};

/* **************************************************** */

template<typename T> void FlowCallbacksLoader::registerFlowCallback(json_object *obj) {
  T *t = new (nothrow) T(obj);

  if(t) {
    cb_all.push_back(t);

    if(t->hasCallback(flow_lua_call_protocol_detected)) cb_protocol_detected.push_back(t);
    if(t->hasCallback(flow_lua_call_periodic_update))   cb_periodic_update.push_back(t);
    if(t->hasCallback(flow_lua_call_idle))              cb_idle.push_back(t);
  }
}

/* **************************************************** */

void FlowCallbacksLoader::reloadFlowCallbacks() {
  json_object *obj;
  enum json_tokener_error jerr = json_tokener_success;
  char key[CONST_MAX_LEN_REDIS_KEY], *value;

  if((value = (char *) calloc(1, POOL_MAX_SERIALIZED_LEN)) == NULL) {
    ntop->getTrace()->traceEvent(TRACE_ERROR, "Unable to allocate memory to deserialize %s", key);
    return;
  }

  if((obj = json_tokener_parse_verbose(value, &jerr)) == NULL) {
    ntop->getTrace()->traceEvent(TRACE_WARNING, "JSON Parse error [%s] key: %s: %s",
				 json_tokener_error_desc(jerr),
				 key, value);
  } else {
    /* Configuration load, i.e., if script_key is enabled, create instance */

    /* e.g., only for enabled flows */
    // registerFlowCallback<FlowCallback>(obj);
    // registerFlowCallback<BlacklistedFlowCallback>(obj);

    /* Example, to be made parametric */
    registerFunction rf = cb_registrable["blacklisted"];
    (this->*rf)(obj);

    /* Free the json */
    json_object_put(obj);
  }

  /* Example, to be removed when config is loaded */
  registerFunction rf = cb_registrable["blacklisted"];
  (this->*rf)(obj);
  rf = cb_registrable["long_lived"];
  (this->*rf)(obj);
  rf = cb_registrable["low_goodput"];
  (this->*rf)(obj);

  free(value);
}

/* **************************************************** */

list<FlowCallback*> FlowCallbacksLoader::getFlowCallbacks(NetworkInterface *iface, FlowLuaCall flow_lua_call) {
  list<FlowCallback*> res;
  list<FlowCallback*> *selected_list;

  switch(flow_lua_call) {
  case flow_lua_call_protocol_detected:
    selected_list = &cb_protocol_detected;
    /* TODO add other cases */
  default:
    selected_list = NULL;
  }

  if(selected_list) {
    for(list<FlowCallback*>::const_iterator it = selected_list->begin(); it != selected_list->end(); ++it) {
      /* Check conditions on iface, e.g., iface->isPacketInterface() */
      res.push_back(NULL /* An Instance */);
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

FlowCallbacksLoader::cb_map_t FlowCallbacksLoader::initFlowCallbacks() {
  FlowCallbacksLoader::cb_map_t cbs;

  /* Add lines here for each new callback added under flow_callbacks/ */
  cbs["blacklisted"] = &FlowCallbacksLoader::registerFlowCallback<BlacklistedFlowCallback>;
  cbs["long_lived"] = &FlowCallbacksLoader::registerFlowCallback<LongLivedFlowCallback>;
  cbs["low_goodput"] = &FlowCallbacksLoader::registerFlowCallback<LowGoodputFlowCallback>;

  return cbs;
}

/* **************************************************** */

FlowCallbacksLoader::cb_map_t FlowCallbacksLoader::cb_registrable = FlowCallbacksLoader::initFlowCallbacks();
