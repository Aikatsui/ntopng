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

#include "ntop_includes.h"

/* Keep these constants in sync with control_groups.lua */
#define CONTROL_GROUP_MAX_CONTROL_GROUP_ID 127 /* Constrained by class Bitmap */
#define CONTROL_GROUP_IDS_KEY "ntopng.prefs.control_groups.control_group_ids"
#define CONTROL_GROUP_DETAILS_KEY "ntopng.prefs.control_groups.control_group_id_%d.details"

/* *************************************** */

HostsControl::HostsControl() {
  init_tstamp = time(NULL);
  host_filters = new (std::nothrow) AddressTree();
  loadConfiguration();
}

/* *************************************** */

static void free_ptree_bitmap(void *data) {
  if(data) {
    Bitmap *host_filter = static_cast<Bitmap*>(data);  
    delete host_filter;
  }   
}

/* *************************************** */

HostsControl::~HostsControl() {
  if (host_filters) {
    host_filters->cleanup(free_ptree_bitmap);
    delete host_filters;
  }
}

/* *************************************** */

bool HostsControl::addHostDisabledFlowAlert(char *host, FlowAlertType disabled_flow_alert_type) {
  Bitmap *host_filter = NULL;
  void *host_data;
  bool success = false;

  if (host_filters == NULL)
    return false;

  /* Check if there is already a bitmap for the host */
  host_data = host_filters->matchAndGetData(host);

  if (host_data) {
    host_filter = static_cast<Bitmap*>(host_data);
  } else {
    /* Accolate a bitmap for the host */
    host_filter = new (std::nothrow) Bitmap();
    if (host_filter)
      host_filters->addAddressAndData(host, host_filter);
  }

  if (host_filter) {
    /* Add filter to the bitmap */
    host_filter->setBit(disabled_flow_alert_type);
    success = true;
  }

  return success;
}

/* *************************************** */

Bitmap *HostsControl::getDisabledFlowAlertsBitmap(Host *h) const {
  Bitmap *host_filter = NULL;
  IpAddress *addr = h->get_ip();
  void *host_data;

  if (host_filters == NULL || addr == NULL)
    return NULL;

  host_data = host_filters->matchAndGetData(addr);

  if (host_data)
    host_filter = static_cast<Bitmap*>(host_data);

  return host_filter;
}

/* *************************************** */

void HostsControl::loadConfiguration() {
  char kname[CONST_MAX_LEN_REDIS_KEY];
  Redis *redis = ntop->getRedis();

  ntop->getTrace()->traceEvent(TRACE_NORMAL, "Reloading host filters");

  //TODO
}

/* *************************************** */

bool HostsControl::checkChange(time_t *last_change) const {
  bool changed = false;

  if(*last_change < init_tstamp)
    changed = true, *last_change = init_tstamp;

  return changed;
}

/* *************************************** */
