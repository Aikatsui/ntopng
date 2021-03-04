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

#ifndef _LOWGOODPUT_FLOW_CALLBACK_H_
#define _LOWGOODPUT_FLOW_CALLBACK_H_

#include "ntop_includes.h"

class LowGoodputFlowCallback : public FlowCallback {
 private:
 public:
  LowGoodputFlowCallback() : FlowCallback(false /* All interfaces */, true /* Exclude for nEdge */, false /* Only for nEdge */,
					  true /* has_protocol_detected */, false /* has_periodic_update */, false /* has_flow_end */) {};
  virtual ~LowGoodputFlowCallback() {};

  void periodicUpdate(Flow *f);
  static std::string getName()                { return(std::string("low_goodput")); }
  bool loadConfiguration(json_object *config) { return(true); }
};

#endif /* _LOWGOODPUT_FLOW_CALLBACK_H_ */
