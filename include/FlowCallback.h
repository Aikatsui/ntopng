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

#ifndef _FLOW_CALLBACK_H_
#define _FLOW_CALLBACK_H_

#include "ntop_includes.h"

class FlowCallback {
 private:
  NtopngEdition plugin_edition;
  u_int8_t has_protocol_detected:1, has_periodic_update:1, has_flow_end:1, packet_interface_only:1, nedge_exclude:1, nedge_only:1, enabled:1/* , _unused:1 */;
 
  bool isCallbackCompatibleWithInterface(NetworkInterface *iface);
  
 protected:
  AlertLevel severity_id;
  bool triggerAlert(Flow *f, FlowAlertType status, AlertLevel severity, u_int16_t alert_score, const char *alert_json);

 public:
  FlowCallback(NtopngEdition _edition, bool _packet_interface_only, bool _nedge_exclude, bool _nedge_only,
	       bool _has_protocok_detected, bool _has_periodic_update, bool _has_flow_end);
  virtual ~FlowCallback();

  /* Callback hooks */
  virtual void protocolDetected(Flow *f) {};
  virtual void periodicUpdate(Flow *f)   {};
  virtual void flowEnd(Flow *f)          {};

  inline void enable()    { enabled = 1;  }
  inline bool isEnabled() { return(enabled ? true : false); }
  inline AlertLevel getSeverity() { return severity_id; }

  void addCallback(std::list<FlowCallback*> *l, NetworkInterface *iface, FlowCallbacks callback);
  virtual bool loadConfiguration(json_object *config);
  
  virtual std::string getName()          const = 0;
  virtual ScriptCategory getCategory()   const = 0;
  virtual FlowAlertType getAlertType() const = 0;
  virtual ndpi_serializer* getAlertJSON(Flow *f) { return(NULL); }
};

#endif /* _FLOW_CALLBACK_H_ */
