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

#ifndef _TLS_CERTIFICATE_EXPIRED_ALERT_H_
#define _TLS_CERTIFICATE_EXPIRED_ALERT_H_

#include "ntop_includes.h"

class TLSCertificateExpiredAlert : public FlowAlert {
 private:
  ndpi_serializer *getAlertJSON(ndpi_serializer* serializer, Flow *f);

 public:
  TLSCertificateExpiredAlert() : FlowAlert("alert_tls_certificate_expired", alert_tls_certificate_expired, alert_category_security) { };
  ~TLSCertificateExpiredAlert() { };
};

#endif /* _TLS_CERTIFICATE_EXPIRED_ALERT_H_ */
