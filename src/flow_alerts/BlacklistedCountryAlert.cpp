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

#include "flow_callbacks_includes.h"

ndpi_serializer* BlacklistedCountryAlert::getAlertJSON(ndpi_serializer* serializer, Flow *f) {
  Host *cli_host, *srv_host;
  bool cli_blacklisted, srv_blacklisted;
  char cli_buf[3], srv_buf[3];

  if(serializer == NULL)
    return NULL;

  cli_buf[0] = '\0', srv_buf[0] = '\0';
  cli_host = f->get_cli_host(), srv_host = f->get_srv_host();

  if(cli_host) cli_host->get_country(cli_buf, sizeof(cli_buf));
  if(srv_host) srv_host->get_country(srv_buf, sizeof(srv_buf));

  f->fcb_get_blacklisted(&cli_blacklisted, &srv_blacklisted);

  ndpi_serialize_string_string(serializer, "cli_country", cli_buf);
  ndpi_serialize_string_string(serializer, "srv_country", srv_buf);
  ndpi_serialize_string_boolean(serializer, "cli_blacklisted", cli_blacklisted);
  ndpi_serialize_string_boolean(serializer, "srv_blacklisted", srv_blacklisted);

  return serializer;
}

