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

#ifndef _UNEXPECTED_DNS_SERVER_H_
#define _UNEXPECTED_DNS_SERVER_H_

#include "ntop_includes.h"

class UnexpectedDNSServer : public UnexpectedServer {
protected:
  bool isAllowedProto(Flow *f)          { return(f->isDNS());                  }
  const IpAddress* getServerIP(Flow *f) { return(f->get_dns_srv_ip_addr());    }
  FlowAlertType getAlertType() const    { return(alert_unexpected_dns_server); }
  
 public:
  UnexpectedDNSServer() : UnexpectedServer() {};
  ~UnexpectedDNSServer() {};
  
  std::string getName()          const { return(std::string("unexpected_dns")); }
};

#endif /* _UNEXPECTED_DNS_SERVER_H_ */
