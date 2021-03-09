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

#include "ntop_includes.h"
#include "flow_callbacks_includes.h"

/* ***************************************************** */

void ElephantFlow::checkElephantFlow(Flow *f) {
  u_int16_t c_score = 0, s_score = 0, f_score = 0;
  u_int64_t l2r_bytes, r2l_bytes;

  if(f->isLocalToRemote())
    l2r_bytes = f->get_bytes_cli2srv(),
      r2l_bytes = f->get_bytes_srv2cli();
  else if(f->isRemoteToLocal())
    l2r_bytes = f->get_bytes_srv2cli(),
      r2l_bytes = f->get_bytes_cli2srv();
  else
    return; /* Nothing to do */

  if(l2r_bytes < l2r_bytes_threshold
     && r2l_bytes < r2l_bytes_threshold)
    return; /* Thresholds not exceeded */

  if(l2r_bytes > l2r_bytes_threshold) {
    if(f->isLocalToRemote()) c_score += 20, s_score += 5; else c_score += 5, s_score += 20;
    f_score += 20;
  }

  if(r2l_bytes > r2l_bytes_threshold) {
    if(f->isRemoteToLocal()) c_score += 20, s_score += 5; else c_score += 5, s_score += 20;
    f_score += 20;
  }

  f->setAlert(this,
	      getSeverity(),
	      f_score, c_score, s_score);
}

/* ***************************************************** */

void ElephantFlow::periodicUpdate(Flow *f) {
  ntop->getTrace()->traceEvent(TRACE_NORMAL, "%s()", __FUNCTION__);

  checkElephantFlow(f);
}

/* ***************************************************** */

void ElephantFlow::flowEnd(Flow *f) {
  ntop->getTrace()->traceEvent(TRACE_NORMAL, "%s()", __FUNCTION__);

  checkElephantFlow(f);
}

/* ***************************************************** */

bool ElephantFlow::loadConfiguration(json_object *config) {
  json_object *json_bytes;

  FlowCallback::loadConfiguration(config); /* Parse parameters in common */

  /*
    Parse additional threshold parameters. Example:

    {
    "r2l_bytes_value": 1073741824,
    "l2r_bytes_value": 1073741824,
    "severity": {
        "syslog_severity": 3,
        "i18n_title": "alerts_dashboard.error",
        "label": "badge-danger",
        "severity_id": 5,
        "icon": "fas fa-exclamation-triangle text-danger"
	}
    }
  */

  /* Remote to local threshold */
  if(json_object_object_get_ex(config, "r2l_bytes_value", &json_bytes))
    r2l_bytes_threshold = json_object_get_int64(json_bytes);

  /* Local to remote threshold */
  if(json_object_object_get_ex(config, "l2r_bytes_value", &json_bytes))
    l2r_bytes_threshold = json_object_get_int64(json_bytes);

  // ntop->getTrace()->traceEvent(TRACE_NORMAL, "%s [%u][%u]", json_object_to_json_string(config), r2l_bytes_threshold, l2r_bytes_threshold);

  return(true);
}

/* ***************************************************** */

ndpi_serializer *ElephantFlow::getAlertJSON(Flow *f) {
  ndpi_serializer *serializer;

  serializer = (ndpi_serializer *) malloc(sizeof(ndpi_serializer));
  
  if(serializer == NULL)
    return NULL;

  if(ndpi_init_serializer(serializer, ndpi_serialization_format_json) == -1) {
    free(serializer);
    return NULL;
  }

  ndpi_serialize_string_uint64(serializer, "l2r_bytes", f->isLocalToRemote() ? f->get_bytes_cli2srv() : f->get_bytes_srv2cli());
  ndpi_serialize_string_uint64(serializer, "r2l_bytes", f->isRemoteToLocal() ? f->get_bytes_cli2srv() : f->get_bytes_srv2cli());
  ndpi_serialize_string_uint64(serializer, "l2r_threshold", l2r_bytes_threshold);
  ndpi_serialize_string_uint64(serializer, "r2l_threshold", r2l_bytes_threshold);

  return serializer;
}
