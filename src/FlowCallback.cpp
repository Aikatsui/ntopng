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

FlowCallback::FlowCallback(bool _packet_interface_only, bool _nedge_exclude, bool _nedge_only,
			   bool _has_protocol_detected, bool _has_periodic_update, bool _has_flow_end) {

  if(_packet_interface_only)  packet_interface_only = 1;
  if(_nedge_exclude)          nedge_exclude = 1;
  if(_nedge_only)             nedge_only = 1;
  if(_has_protocol_detected)  has_protocol_detected = 1;
  if(_has_periodic_update)    has_periodic_update = 1;
  if(_has_flow_end)           has_flow_end = 1;
  
  enabled = 0;
};

/* **************************************************** */

FlowCallback::~FlowCallback() {
};

/* **************************************************** */

bool FlowCallback::isCallbackCompatibleWithInterface(NetworkInterface *iface) {
  if(packet_interface_only && (!iface->isPacketInterface())) return(false);
  if(nedge_only && (!ntop->getPrefs()->is_nedge_edition()))  return(false);
  if(nedge_exclude && ntop->getPrefs()->is_nedge_edition())  return(false);

  return(true);
}

/* **************************************************** */

void FlowCallback::addCallback(std::list<FlowCallback*> *l, NetworkInterface *iface, FlowCallbacks callback) {
  if(!isCallbackCompatibleWithInterface(iface)) return;

  switch(callback) {
  case flow_callback_protocol_detected:
    if(has_protocol_detected) l->push_back(this);
    break;
    
  case flow_callback_periodic_update:
    if(has_periodic_update) l->push_back(this);
    break;
    
  case flow_callback_flow_end:
    if(has_flow_end) l->push_back(this);
    break;
  }
}
