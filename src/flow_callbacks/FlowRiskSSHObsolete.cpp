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

ndpi_serializer *FlowRiskSSHObsolete::getAlertJSON(Flow *f) {
  ndpi_serializer *serializer;
  ndpi_risk_enum risk_id = NDPI_SSH_OBSOLETE_SERVER_VERSION_OR_CIPHER; /* TODO also handle NDPI_SSH_OBSOLETE_CLIENT_VERSION_OR_CIPHER */

  serializer = (ndpi_serializer *) malloc(sizeof(ndpi_serializer));
  
  if (serializer == NULL)
    return NULL;

  if (ndpi_init_serializer(serializer, ndpi_serialization_format_json) == -1) {
    free(serializer);
    return NULL;
  }

  ndpi_serialize_string_int32(serializer, "risk_id", risk_id);

  return serializer;
}

/* ***************************************************** */

