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

FlowCallbacksExecutor::FlowCallbacksExecutor(FlowCallbacksLoader *flow_callbacks_loader, NetworkInterface *_iface) {
  iface = _iface;

  list_protocol_detected = flow_callbacks_loader->getFlowCallbacks(iface, flow_lua_call_protocol_detected),
    list_periodic_update = flow_callbacks_loader->getFlowCallbacks(iface, flow_lua_call_periodic_update),
    list_idle = flow_callbacks_loader->getFlowCallbacks(iface, flow_lua_call_idle);
};

/* **************************************************** */

FlowCallbacksExecutor::~FlowCallbacksExecutor() {
};

/* **************************************************** */

void FlowCallbacksExecutor::reloadFlowCallbacks() {
  FlowCallbacksLoader fcl; /* This will be inside class Ntop, e.g., ntop->getFlowCallbacksLoader() */

  list_protocol_detected = fcl.getFlowCallbacks(iface, flow_lua_call_protocol_detected);
  list_periodic_update = fcl.getFlowCallbacks(iface, flow_lua_call_periodic_update);
  list_idle = fcl.getFlowCallbacks(iface, flow_lua_call_idle);
}

/* **************************************************** */

FlowLuaCallExecStatus FlowCallbacksExecutor::listExecute(Flow *f, list<FlowCallback*> *l) {
  for(list<FlowCallback*>::const_iterator it = l->begin(); it != l->end(); ++it)
    ; /* Do the execution */

  return flow_lua_call_exec_status_ok;
}

/* **************************************************** */

FlowLuaCallExecStatus FlowCallbacksExecutor::execute(Flow *f, FlowLuaCall flow_lua_call) {
  switch(flow_lua_call) {
  case flow_lua_call_protocol_detected:
    return listExecute(f, &list_protocol_detected);
  case flow_lua_call_periodic_update:
    return listExecute(f, &list_periodic_update);
  case flow_lua_call_idle:
    return listExecute(f, &list_idle);
  default:
    return flow_lua_call_exec_status_not_executed_unknown_call;
  }
}
