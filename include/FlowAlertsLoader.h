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

#ifndef _FLOW_ALERTS_LOADER_H_
#define _FLOW_ALERTS_LOADER_H_

#include "ntop_includes.h"

/* Singleton, i.e. a single instance */

class FlowAlertsLoader {
 private:
  FlowAlert* flow_alerts[MAX_FLOW_ALERT_TYPE];
  
  void registerFlowAlerts(); /* Method called at runtime to register all alerts */
  void loadConfiguration();
  void defineAlert(FlowAlert *fa);
  
public:
  FlowAlertsLoader();
  ~FlowAlertsLoader();
  
  void reloadFlowAlerts();

  AlertCategory getAlertCategory(FlowAlertType fat) const;
  char* getAlertJSON(FlowAlertType fat, Flow *f) const;
  ndpi_serializer *getAlertSerializer(FlowAlertType fat, Flow *f) const; 
};

#endif /* _FLOW_ALERTS_LOADER_H_ */
