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

FlowCallbacksExecutor::FlowCallbacksExecutor(FlowCallbacksLoader *fcl, NetworkInterface *_iface) {
  iface = _iface;
  memset(alert_type_to_callback, 0, sizeof(alert_type_to_callback));
  loadFlowCallbacks(fcl);
};

/* **************************************************** */

FlowCallbacksExecutor::~FlowCallbacksExecutor() {
  if(protocol_detected) delete protocol_detected;
  if(periodic_update)   delete periodic_update;
  if(flow_end)          delete flow_end;
};

/* **************************************************** */

void FlowCallbacksExecutor::loadFlowCallbacksAlerts(std::list<FlowCallback*> *cb_list) {
  for(std::list<FlowCallback*>::const_iterator it = cb_list->begin(); it != cb_list->end(); ++it) {
    FlowAlertType alert_type = (*it)->getAlertType();

    if(alert_type_to_callback[alert_type] != NULL) {
      /* Entry already existing, check if this is the same callback */
      if(alert_type_to_callback[alert_type] != *it) /* Another callback providing the same alert */
	ntop->getTrace()->traceEvent(TRACE_ERROR, "Duplicate callback for the same status [%s][was: %s]",
				     (*it)->getName().c_str(), alert_type_to_callback[alert_type]->getName().c_str());
    } else {
      /* New entry */
      alert_type_to_callback[alert_type] = *it;
    }
  }
}

/* **************************************************** */

void FlowCallbacksExecutor::loadFlowCallbacks(FlowCallbacksLoader *fcl) {
  protocol_detected = fcl->getProtocolDetectedCallbacks(iface);
  periodic_update   = fcl->getPeriodicUpdateCallbacks(iface);
  flow_end          = fcl->getFlowEndCallbacks(iface);

  loadFlowCallbacksAlerts(protocol_detected),
    loadFlowCallbacksAlerts(periodic_update),
    loadFlowCallbacksAlerts(flow_end);
}

/* **************************************************** */

ScriptCategory FlowCallbacksExecutor::getAlertCategory(FlowAlertType fat) const {
  if(alert_type_to_callback[fat] != NULL)
    return alert_type_to_callback[fat]->getCategory();

  return(script_category_other);
}

/* **************************************************** */

/* NOTE: memory MUST be freed by the caller */
char* FlowCallbacksExecutor::getAlertJSON(FlowAlertType fat, Flow *f) const {
  ndpi_serializer *alert_json_serializer = NULL;
  char *json_string = NULL;
  u_int32_t json_string_len;
  FlowCallback *fc = alert_type_to_callback[fat];

  if(!fc)
    return NULL; /* Callback not found */

  alert_json_serializer = fc->getSerializedAlert(f);

  if(alert_json_serializer) {
    json_string = ndpi_serializer_get_buffer(alert_json_serializer, &json_string_len);
    json_string = json_string ? strdup(json_string) : NULL; /* Allocate memory */
    ndpi_term_serializer(alert_json_serializer);
  } else {
    json_string = fc->getAlertJSONStr(f); /* Already allocated */
  }

  /* Always allocated in memory (must be freed) */
  return(json_string);
}
