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

void TLSCertificateExpired::protocolDetected(Flow *f) {
  ntop->getTrace()->traceEvent(TRACE_NORMAL, "%s()", __FUNCTION__);
  
  if (f->isThreeWayHandshakeOK() 
      && f->isTLS()
      && f->getTLSNotBefore()
      && f->getTLSNotAfter()) {
    time_t now = time(NULL);

    if (now < f->getTLSNotBefore()
        || now > f->getTLSNotAfter()) {
      u_int16_t c_score = 5;
      u_int16_t s_score = 50;
      u_int16_t f_score = 40;

      //TODO pass flow.getTLSInfo()

      f->setAlert(this, getSeverity(), f_score, c_score, s_score);
    }
  }
}

/* ***************************************************** */

ndpi_serializer *TLSCertificateExpired::getAlertJSON(Flow *f) {
  ndpi_serializer *serializer;
 
   serializer = (ndpi_serializer *) malloc(sizeof(ndpi_serializer));
  
  if (serializer == NULL)
    return NULL;

  if (ndpi_init_serializer(serializer, ndpi_serialization_format_json) == -1) {
    free(serializer);
    return NULL;
  }

  f->getTLSInfo(serializer);
  
  return serializer;
}
