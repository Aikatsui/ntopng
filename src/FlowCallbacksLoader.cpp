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

void FlowCallbacksLoader::reloadFlowCallbacks() {
  json_object *obj;
  enum json_tokener_error jerr = json_tokener_success;
  char key[CONST_MAX_LEN_REDIS_KEY], *value;

  if((value = (char *) malloc(POOL_MAX_SERIALIZED_LEN)) == NULL) {
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
    enabled_flow_callbacks.push_back(new BlacklistedFlowCallback(obj));

    /* Free the json */
    json_object_put(obj);
  }

  free(value);
}

/* **************************************************** */

list<FlowCallback*> FlowCallbacksLoader::getFlowCallbacks(NetworkInterface *iface, FlowLuaCall flow_lua_call) {
  list<FlowCallback*> res;

  for(list<FlowCallback*>::const_iterator it = enabled_flow_callbacks.begin(); it != enabled_flow_callbacks.end(); ++it) {
    /* Check conditions on iface, e.g., iface->isPacketInterface() */
    res.push_back(NULL /* An Instance */);
  }

  return res;
}
