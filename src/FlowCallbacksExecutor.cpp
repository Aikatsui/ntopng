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

    if(alert_type_to_callback.find(alert_type) != alert_type_to_callback.end()) {
      /* Entry already existing, check if this is the same callback */
      if(alert_type_to_callback[alert_type] != *it) /* Another callback providing the same alert */
	ntop->getTrace()->traceEvent(TRACE_ERROR, "Duplicate callback for the same status [%s][was: %s]", (*it)->getName().c_str(), alert_type_to_callback[alert_type]->getName().c_str());
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
  std::map<FlowAlertType, FlowCallback*>::const_iterator it = alert_type_to_callback.find(fat);

  if(it != alert_type_to_callback.end())
    return it->second->getCategory();

  return script_category_other;
}

/* **************************************************** */

char *FlowCallbacksExecutor::getAlertJSON(FlowAlertType fat, Flow *f) const {
  std::map<FlowAlertType, FlowCallback*>::const_iterator it = alert_type_to_callback.find(fat);

  if(it != alert_type_to_callback.end())
    return it->second->getAlertJSONStr(f);

  /* TODO */
  return NULL;
}
