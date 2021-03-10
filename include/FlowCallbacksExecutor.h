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

#ifndef _FLOW_CALLBACKS_EXECUTOR_H_
#define _FLOW_CALLBACKS_EXECUTOR_H_

#include "ntop_includes.h"

class Flow;

class FlowCallbacksExecutor { /* One instance per ntopng Interface */
 private:
  NetworkInterface *iface;
  FlowCallback* alert_type_to_callback[MAX_FLOW_ALERT_TYPE];
  std::list<FlowCallback*> *protocol_detected, *periodic_update, *flow_end;

  void loadFlowCallbacksAlerts(std::list<FlowCallback*> *cb_list);
  void loadFlowCallbacks(FlowCallbacksLoader *fcl);
    
 public:
  FlowCallbacksExecutor(FlowCallbacksLoader *fcl, NetworkInterface *_iface);
  virtual ~FlowCallbacksExecutor();

  inline void execProtocolDetectedCallback(Flow *f) {
    for(list<FlowCallback*>::iterator it = protocol_detected->begin(); it != protocol_detected->end(); ++it)
      (*it)->protocolDetected(f);
  }

  inline void execPeriodicUpdateCallback(Flow *f) {
    for(list<FlowCallback*>::iterator it = periodic_update->begin(); it != periodic_update->end(); ++it)
      (*it)->periodicUpdate(f);
  }

  inline void execFlowEndCallback(Flow *f) {
    for(list<FlowCallback*>::iterator it = flow_end->begin(); it != flow_end->end(); ++it)
      (*it)->flowEnd(f);
  }

  ScriptCategory getAlertCategory(FlowAlertType fat) const;
  char* getAlertJSON(FlowAlertType fat, Flow *f) const;
};

#endif /* _FLOW_CALLBACKS_EXECUTOR_H_ */
