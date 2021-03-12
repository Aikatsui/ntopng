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

#ifndef _FLOW_ALERT_H_
#define _FLOW_ALERT_H_

#include "ntop_includes.h"

class FlowAlert {
 private:
  AlertLevel severity_id;
  std::string name;
  FlowAlertType alert_type;
  AlertCategory category;
  
 public:
  FlowAlert(const char* _name, FlowAlertType _alert_type, AlertCategory _category) {
    name.assign(_name);
    alert_type = _alert_type;
    category = _category;
  }
  virtual ~FlowAlert() { };
  
  inline AlertLevel        getSeverity()  { return(severity_id); }  
  inline std::string       getName()      { return(name);        }
  inline AlertCategory     getCategory()  { return(category);    }
  inline FlowAlertType     getAlertType() { return(alert_type);  }
  
  virtual ndpi_serializer* getAlertJSON(ndpi_serializer* serializer, Flow *f)  { return serializer; }
};

#endif /* _FLOW_ALERT_H_ */
