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

#ifndef _FLOW_CALLBACKS_LOADER_H_
#define _FLOW_CALLBACKS_LOADER_H_

#include "ntop_includes.h"

class FlowCallbacksLoader { /* A single instance inside Ntop */
 private:
  typedef void (FlowCallbacksLoader::*registerFunction)(json_object *obj); /* registerFlowCallback function pointer typedef */
  typedef std::unordered_map<std::string, registerFunction> cb_map_t;      /* Map of registrable callbacks function pointer */
  static cb_map_t cb_registrable;     /* Map with all the registrable callbacks, that is, every class under flow_callbacks/ */

  /* These are callback instances, that is classes instantiated at runtime each one with a given configuration */
  list<FlowCallback*> cb_all; /* All the callbacks instantiated */
  list<FlowCallback*> cb_protocol_detected, cb_periodic_update, cb_idle; /* Callbacks instantiated, divided by type */

  static cb_map_t initFlowCallbacks(); /* Static method to initialize all the registrable callbacks */
  template<typename T> void registerFlowCallback(json_object *obj); /* Method called at runtime to register a callback */

 public:
  FlowCallbacksLoader();
  virtual ~FlowCallbacksLoader();

  void reloadFlowCallbacks();
  list<FlowCallback*> getFlowCallbacks(NetworkInterface *iface, FlowLuaCall flow_lua_call);
  void printCallbacks();
};

/* Below are tests to check which callbacks each FlowCallback implements */

// SFINAE test for protocolDetected
template <typename T>
class has_protocolDetected {
 private:
  typedef char YesType[1];
  typedef char NoType[2];

  template <typename C> static YesType& test( decltype(&C::protocolDetected) ) ;
  template <typename C> static NoType& test(...);

 public:
  enum { value = sizeof(test<T>(0)) == sizeof(YesType) };
};

// SFINAE test for periodicUpdate
template <typename T>
class has_periodicUpdate {
 private:
  typedef char YesType[1];
  typedef char NoType[2];

  template <typename C> static YesType& test( decltype(&C::periodicUpdate) ) ;
  template <typename C> static NoType& test(...);

 public:
  enum { value = sizeof(test<T>(0)) == sizeof(YesType) };
};

// SFINAE test for flowEnd 
template <typename T>
class has_flowEnd {
 private:
  typedef char YesType[1];
  typedef char NoType[2];

  template <typename C> static YesType& test( decltype(&C::flowEnd) ) ;
  template <typename C> static NoType& test(...);

 public:
  enum { value = sizeof(test<T>(0)) == sizeof(YesType) };
};

#endif /* _FLOW_CALLBACKS_LOADER_H_ */
