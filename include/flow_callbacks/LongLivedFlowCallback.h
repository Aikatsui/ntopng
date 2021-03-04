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

#ifndef _LONGLIVED_FLOW_CALLBACK_H_
#define _LONGLIVED_FLOW_CALLBACK_H_

#include "ntop_includes.h"

class LongLivedFlowCallback : public FlowCallback {
 private:
 public:
  LongLivedFlowCallback() :  FlowCallback(false /* All interfaces */, false /* Don't exclude for nEdge */, false /* NOT only for nEdge */,
					  true /* has_protocol_detected */, false /* has_periodic_update */, false /* has_flow_end */) {};
  virtual ~LongLivedFlowCallback() {};

  void periodicUpdate(Flow *f);
  bool loadConfiguration(json_object *config) { return(true); }
  static std::string getName() { return(std::string("long_lived")); }
};

#endif /* _LONGLIVED_FLOW_CALLBACK_H_ */
