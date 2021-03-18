/*
 *
 * (C) 2015-21 - ntop.org
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

#ifndef _HOSTS_CONTROL_H_
#define _HOSTS_CONTROL_H_

#include "ntop_includes.h"

class HostsControl {
 private:
  time_t init_tstamp; /* Timestamp, set when the class instance is created */
  AddressTree *host_filters; /* A ptree holding, for each host, a Bitmap with disabled flow alerts */

  /* Add a disabled flow alert for a host */
  bool addHostDisabledFlowAlert(char *host, FlowAlertType disabled_flow_alert_type);

  void loadConfiguration(); /* Read the configuration from Redis and initialize internal data structures */

 public:
  HostsControl();
  virtual ~HostsControl();

  bool checkChange(time_t *last_change) const;
  Bitmap *getDisabledFlowAlertsBitmap(Host *host) const;
  bool isFlowAlertDisabled(Host *host, FlowAlertType flow_alert_type) const;
};

#endif /* _HOSTS_CONTROL_H_ */
