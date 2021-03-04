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

#ifndef _FLOW_CALLBACK_H_
#define _FLOW_CALLBACK_H_

#include "ntop_includes.h"

class FlowCallback {
 private:
  bool packet_interface_only, nedge_exclude, nedge_only;

 protected:
 public:
  FlowCallback(json_object *json_config, bool _packet_interface_only, bool _nedge_exclude, bool _nedge_only);
  virtual ~FlowCallback();

  bool isCallbackForInterface(NetworkInterface *iface);
  virtual bool hasCallback(FlowLuaCall flow_lua_call) const { return false; };

  virtual void protocolDetected(Flow *f) {};
  virtual void periodicUpdate(Flow *f) {};
  virtual void flowEnd(Flow *f) {};
};

#endif /* _FLOW_CALLBACK_H_ */
