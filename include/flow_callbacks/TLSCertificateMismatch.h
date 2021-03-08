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

#ifndef _TLS_CERTIFICATE_MISMATCH_FLOW_CALLBACK_H_
#define _TLS_CERTIFICATE_MISMATCH_FLOW_CALLBACK_H_

#include "ntop_includes.h"

class TLSCertificateMismatch : public FlowCallback {
 private:
  
 public:
 TLSCertificateMismatch() : FlowCallback(ntopng_edition_community,
					 true /* Packet-interfaces only */, false /* Don't exclude for nEdge */, false /* NOT only for nEdge */,
					 true /* has_protocol_detected */, false /* has_periodic_update */, false /* has_flow_end */) {};
  ~TLSCertificateMismatch() {};

  bool loadConfiguration(json_object *config) { return(true); }
  void protocolDetected(Flow *f);
  
  std::string getName()          const { return(std::string("tls_certificate_mismatch")); }
  ScriptCategory getCategory()   const { return script_category_security;                 }
  FlowAlertType getAlertType() const { return alert_tls_certificate_mismatch;          }
};

#endif /* _TLS_CERTIFICATE_MISMATCH_FLOW_CALLBACK_H_ */
