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
#include "flow_alerts_include.h"

/* **************************************************** */

FlowAlertsLoader::FlowAlertsLoader(){
  memset(flow_alerts, 0, sizeof(flow_alerts));
  
  registerFlowAlerts();
  loadConfiguration();
}

/* **************************************************** */

FlowAlertsLoader::~FlowAlertsLoader() {
  for(u_int i=0; i<MAX_DEFINED_FLOW_ALERT_TYPE; i++) {
    if(flow_alerts[i] != NULL) {
      delete flow_alerts[i];
    }
  }
}

/* **************************************************** */

void FlowAlertsLoader::defineAlert(FlowAlert *fa) {
  if(!fa)
    return;
  else {
    FlowAlertType alert_type = fa->getAlertType();

    if(flow_alerts[alert_type] == NULL) {
      flow_alerts[alert_type] = fa;
      ntop->getTrace()->traceEvent(TRACE_NORMAL, "Loaded alert %s [alert_type: %u]",
				   fa->getName().c_str(), alert_type);
    } else {
      ntop->getTrace()->traceEvent(TRACE_WARNING, "Skipping alert redefinition %s [alert_type: %u]",
				   fa->getName().c_str(), alert_type);
      delete fa;
    }
  }
}

/* **************************************************** */

void FlowAlertsLoader::registerFlowAlerts() {
  /* TODO: implement dynamic loading */

  defineAlert(new BlacklistedFlowAlert());
  
  for(u_int i=0; i<MAX_DEFINED_FLOW_ALERT_TYPE; i++) {
    if(flow_alerts[i] == NULL)
      ntop->getTrace()->traceEvent(TRACE_WARNING, "Missing definition for flow alert type: %u", i);
  }
}

/* **************************************************** */

void FlowAlertsLoader::loadConfiguration() {
#if 0
  json_object *json = NULL, *json_config, *json_config_flow;
  struct json_object_iterator it;
  struct json_object_iterator itEnd;
  enum json_tokener_error jerr = json_tokener_success;
  char *value = NULL;
  u_int actual_len = ntop->getRedis()->len(FLOW_ALERTS_CONFIG);

  if((value = (char *) malloc(actual_len + 1)) == NULL) {
    ntop->getTrace()->traceEvent(TRACE_ERROR, "Unable to allocate memory to deserialize %s", FLOW_ALERTS_CONFIG);
    goto out;
  }

  if(ntop->getRedis()->get((char*)FLOW_ALERTS_CONFIG, value, actual_len + 1) != 0) {
    ntop->getTrace()->traceEvent(TRACE_ERROR, "Unable to find configuration %s", FLOW_ALERTS_CONFIG);
    goto out;
  }

  if((json = json_tokener_parse_verbose(value, &jerr)) == NULL) {
    ntop->getTrace()->traceEvent(TRACE_ERROR, "JSON Parse error [%s] %s [len: %u][strlen: %u]",
				 json_tokener_error_desc(jerr), value, actual_len, strlen(value));
    goto out;
  }

  if(!json_object_object_get_ex(json, "config", &json_config)) {
    /* 'config' section inside the JSON */
    ntop->getTrace()->traceEvent(TRACE_ERROR, "'config' not found in JSON");
    goto out;
  }

  if(!json_object_object_get_ex(json_config, "flow", &json_config_flow)) {
    /* 'flow' section inside 'config' JSON */
    ntop->getTrace()->traceEvent(TRACE_ERROR, "'flow' not found in 'config' JSON");
    goto out;
  }

  /*
    Iterate over all script configurations
  */
  it = json_object_iter_begin(json_config_flow);
  itEnd = json_object_iter_end(json_config_flow);

  while(!json_object_iter_equal(&it, &itEnd)) {
    const char *alert_key   = json_object_iter_peek_name(&it);
    json_object *alert_config = json_object_iter_peek_value(&it);
    json_object *json_script_conf, *json_hook_all;

    if(json_object_object_get_ex(alert_config, "all", &json_hook_all)) {
      json_object *json_enabled;
      bool enabled;

      if(cb_all.find(alert_key) != cb_all.end()) {
	FlowAlert *cb = cb_all[alert_key];

	if(json_object_object_get_ex(json_hook_all, "enabled", &json_enabled))
	  enabled = json_object_get_boolean(json_enabled);
	else
	  enabled = false;

	if(enabled) {
	  /* Script enabled */
	  if(json_object_object_get_ex(json_hook_all, "script_conf", &json_script_conf)) {
	    if(cb_all.find(alert_key) != cb_all.end()) {
	      FlowAlert *cb = cb_all[alert_key];

	      if(cb->loadConfiguration(json_script_conf)) {
		ntop->getTrace()->traceEvent(TRACE_NORMAL, "Successfully enabled alert %s", alert_key);
	      } else {
		ntop->getTrace()->traceEvent(TRACE_WARNING, "Error while loading alert %s configuration",
					     alert_key);
	      }

	      cb->enable();
	      cb->scriptEnable(); 
	    }
	  }
	} else {
	  /* Script disabled */
	  cb->scriptDisable(); 
	}
      } else {
	if(strcmp(alert_key, "new_flow_api_demo") == 0
	   || strcmp(alert_key, "flow_logger") == 0)
	  ; /* No noise for demos */
	else
	  ntop->getTrace()->traceEvent(TRACE_WARNING, "Unable to find flow alert  %s", alert_key);
      }
    }

    /* Move to the next element */
    json_object_iter_next(&it);
  } /* while */

 out:
  /* Free the json */
  if(json)  json_object_put(json);
  if(value) free(value);
#endif
}

