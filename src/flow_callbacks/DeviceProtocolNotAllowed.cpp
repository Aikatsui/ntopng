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

void DeviceProtocolNotAllowed::protocolDetected(Flow *f) {

  if (!f->isDeviceAllowedProtocol()) {
    u_int16_t c_score, s_score, f_score = 80;
    const IpAddress *attacker, *victim;

    if (!f->isCliDeviceAllowedProtocol()) {
      c_score = 80;
      s_score = 5;
      attacker = f->get_cli_ip_addr();
      victim = f->get_srv_ip_addr();
    } else {
      c_score = 5;
      s_score = 80;
      attacker = f->get_srv_ip_addr();
      victim = f->get_cli_ip_addr();
    }

    /* TODO
     * char buf[64];
     * attacker->print(buf, sizeof(buf);
     * victim->print(buf, sizeof(buf)
     * set_attacker(attacker)
     * set_victim(victim)
     */

    f->triggerAlert(this, getSeverity(), f_score, c_score, s_score);
  }
}

/* ***************************************************** */

ndpi_serializer *DeviceProtocolNotAllowed::getAlertJSON(ndpi_serializer* serializer, Flow *f) {
  Host *cli = f->get_cli_host(), *srv = f->get_srv_host();
  DeviceType cli_dev_type = device_unknown, srv_dev_type = device_unknown; 

  if(serializer == NULL)
    return NULL;

  if (cli && cli->getMac()) cli_dev_type = cli->getMac()->getDeviceType();
  if (srv && srv->getMac()) srv_dev_type = srv->getMac()->getDeviceType();

  ndpi_serialize_string_int32(serializer, "cli.devtype", cli_dev_type);
  ndpi_serialize_string_int32(serializer, "srv.devtype", srv_dev_type);

  if (!f->isCliDeviceAllowedProtocol()) {
    ndpi_serialize_string_string(serializer, "devproto_forbidden_peer", "cli");
    ndpi_serialize_string_int32(serializer, "devproto_forbidden_id", f->getCliDeviceDisallowedProtocol());
  } else {
    ndpi_serialize_string_string(serializer, "devproto_forbidden_peer", "srv");
    ndpi_serialize_string_int32(serializer, "devproto_forbidden_id", f->getSrvDeviceDisallowedProtocol());
  }
  
  return serializer;
}

/* ***************************************************** */

