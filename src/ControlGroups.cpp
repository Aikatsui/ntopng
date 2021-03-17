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

/* *************************************** */

ControlGroups::ControlGroups() {
  init_tstamp = time(NULL);
  member_groups = NULL;
  loadConfiguration();
}

/* *************************************** */

ControlGroups::~ControlGroups() {
}

/* *************************************** */

void ControlGroups::loadGroupMember(u_int group_id, const char * const member) {
}

/* *************************************** */

void ControlGroups::loadGroupDisabledFlowAlert(u_int group_id, FlowAlertType disabled_flow_alert_type) {
}

/* *************************************** */

void ControlGroups::loadConfiguration() {
  ntop->getTrace()->traceEvent(TRACE_NORMAL, "Reloading control groups");
}

/* *************************************** */

bool ControlGroups::checkChange(time_t *last_change) const {
  bool changed = false;

  if(*last_change < init_tstamp)
    changed = true, *last_change = init_tstamp;

  return changed;
}

/* *************************************** */

Bitmap ControlGroups::getDisabledFlowAlertsBitmap(Host *h) const {
  Bitmap res;

  return res;
}


/* *************************************** */
