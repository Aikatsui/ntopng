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
  virtual void protocolDetected(Flow *f) {};
  virtual void periodicUpdate(Flow *f) {};
  virtual void flowEnd(Flow *f) {};
 protected:
 public:
  FlowCallback(json_object *json_config);
  virtual ~FlowCallback();

  virtual bool hasCallback(FlowLuaCall flow_lua_call) const { return false; };
  void execute(FlowLuaCall flow_lua_call, Flow *f);
};

#endif /* _FLOW_CALLBACK_H_ */
