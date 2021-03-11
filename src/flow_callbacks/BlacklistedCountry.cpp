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

bool BlacklistedCountry::hasBlacklistedCountry(Host *h) const {
  char buf[3], *country;

  if(!h) return false;

  country = h->get_country(buf, sizeof(buf));

  return blacklisted_countries.find(country) != blacklisted_countries.end();
}

/* ***************************************************** */

void BlacklistedCountry::protocolDetected(Flow *f) {
  Host *cli_host, *srv_host;
  u_int16_t c_score = 0, s_score = 0;

  if(blacklisted_countries.size() == 0)
    return; /* Callback enabled but no blacklisted country is configured */

  cli_host = f->get_cli_host(), srv_host = f->get_srv_host();

  if(hasBlacklistedCountry(f->get_cli_host()))
    c_score += 60, s_score += 10;

  if(hasBlacklistedCountry(f->get_srv_host()))
    s_score += 60, c_score += 10;

  if(c_score || s_score)
    f->triggerAlert(this, getSeverity(), c_score + s_score, c_score, s_score);
}

/* ***************************************************** */

ndpi_serializer *BlacklistedCountry::getAlertJSON(ndpi_serializer* serializer, Flow *f) {
  Host *cli_host, *srv_host;
  char cli_buf[3], srv_buf[3];
  if(serializer == NULL)
    return NULL;

  cli_buf[0] = '\0', srv_buf[0] = '\0';
  cli_host = f->get_cli_host(), srv_host = f->get_srv_host();

  if(cli_host) cli_host->get_country(cli_buf, sizeof(cli_buf));
  if(srv_host) srv_host->get_country(srv_buf, sizeof(srv_buf));

  ndpi_serialize_string_string(serializer, "cli_country", cli_buf);
  ndpi_serialize_string_string(serializer, "srv_country", srv_buf);
  ndpi_serialize_string_boolean(serializer, "cli_blacklisted", hasBlacklistedCountry(cli_host));
  ndpi_serialize_string_boolean(serializer, "srv_blacklisted", hasBlacklistedCountry(srv_host));

  return serializer;
}

/* ***************************************************** */

bool BlacklistedCountry::loadConfiguration(json_object *config) {
  FlowCallback::loadConfiguration(config); /* Parse parameters in common */
  json_object *countries_json, *country_json;

  /*
    Parse additional parameters. Example (countries are under "items"):

    { "items": [ "IT", "FR", "DE", "CN" ], "severity": ...
  */

  /* Iterathe through the items array with country codes */
  if(json_object_object_get_ex(config, "items", &countries_json)) {
    for (int i = 0; i < json_object_array_length(countries_json); i++) {
      country_json = json_object_array_get_idx(countries_json, i);

      /* Add each country code to the set of blacklisted countries */
      std::string country = json_object_get_string(country_json);
      blacklisted_countries.insert(country);
    }
  }

  /*

    ntop->getTrace()->traceEvent(TRACE_NORMAL, "%s", json_object_to_json_string(config));

    std::set<string>::iterator it;
    for(it = blacklisted_countries.begin(); it != blacklisted_countries.end(); ++it)
    ntop->getTrace()->traceEvent(TRACE_NORMAL, "Parsed: %s", (*it).c_str());
  */


  return(true);
}
