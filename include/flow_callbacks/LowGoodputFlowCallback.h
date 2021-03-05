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
					  false /* has_protocol_detected */, true /* has_periodic_update */, false /* has_flow_end */) {};
  virtual ~LowGoodputFlowCallback() {};

  void periodicUpdate(Flow *f);

  std::string getName()          const { return(std::string("low_goodput")); }
  ScriptCategory getCategory()   const { return script_category_network; }
  FlowCallbackStatus getStatus() const { return status_low_goodput;      }
};

#endif /* _LOWGOODPUT_FLOW_CALLBACK_H_ */
