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

static const u_int16_t min_pkt_threshold   = 10;
static const u_int16_t normal_issues_ratio = 10; // 1/10
static const u_int16_t severe_issues_ratio = 3;  // 1/3

/* ******************************************** */

void TCPIssues::checkFlow(Flow *f, bool *is_client, bool *is_server, bool *is_severe, bool trigger_alert) {
  if(f->get_protocol() != IPPROTO_TCP) return; /* Non TCP traffic */

  if(f->getCliTcpIssues() > min_pkt_threshold) {
    u_int64_t pkts = f->get_packets_cli2srv();
        
    if((f->getCliTcpIssues() * severe_issues_ratio) > pkts)
      *is_client = true, *is_severe = true;
    else if((f->getCliTcpIssues() * normal_issues_ratio) > pkts)  
      *is_client = true;
  }
  
  if(f->getSrvTcpIssues() > min_pkt_threshold) {
    u_int64_t pkts = f->get_packets_srv2cli();
        
    if((f->getSrvTcpIssues() * severe_issues_ratio) > pkts)
      *is_server = true, *is_severe = true;
    else if((f->getSrvTcpIssues() * normal_issues_ratio) > pkts)  
      *is_server = true;
  }

  if(trigger_alert) {
    if(*is_client || *is_server) {
      u_int16_t fs_score = *is_severe ? 20 : 10;
      
      f->triggerAlert(this, *is_severe ? severity_id : alert_level_info, fs_score /* f_score */, fs_score /* c_score */, fs_score /* s_score */);
    }
  }
}

/* ******************************************** */

void TCPIssues::periodicUpdate(Flow *f) {
  bool is_client, is_server, is_severe;

  checkFlow(f, &is_client, &is_server, &is_severe, true);
}

/* ******************************************** */

void TCPIssues::flowEnd(Flow *f) {
  bool is_client, is_server, is_severe;

  checkFlow(f, &is_client, &is_server, &is_severe, true);
}

/* ******************************************** */

ndpi_serializer* TCPIssues::getAlertJSON(Flow *f) {
  ndpi_serializer *serializer = (ndpi_serializer*)malloc(sizeof(ndpi_serializer));
  bool is_client, is_server, is_severe;
  FlowTrafficStats *stats = f->getTrafficStats();
  const ndpi_analyze_struct *cli2srv_bytes_stats, *srv2cli_bytes_stats;
  
  if(!serializer) return(NULL);

  if(ndpi_init_serializer(serializer, ndpi_serialization_format_json) == -1) {
    free(serializer);
    return(NULL);
  }

  checkFlow(f, &is_client, &is_server, &is_severe, false);
  
  ndpi_serialize_start_of_block(serializer,   "tcp_stats");
  cli2srv_bytes_stats = stats->get_analize_struct(true), srv2cli_bytes_stats = stats->get_analize_struct(false);
  
  ndpi_serialize_string_int64(serializer, "cli2srv.retransmissions", stats->get_cli2srv_tcp_retr());
  ndpi_serialize_string_int64(serializer, "cli2srv.out_of_order",    stats->get_cli2srv_tcp_ooo());
  ndpi_serialize_string_int64(serializer, "cli2srv.lost",            stats->get_cli2srv_tcp_lost());
  ndpi_serialize_string_int64(serializer, "srv2cli.retransmissions", stats->get_srv2cli_tcp_retr());
  ndpi_serialize_string_int64(serializer, "srv2cli.out_of_order",    stats->get_srv2cli_tcp_ooo());
  ndpi_serialize_string_int64(serializer, "srv2cli.lost",            stats->get_srv2cli_tcp_lost());
  ndpi_serialize_end_of_block(serializer);
  
  ndpi_serialize_string_int32(serializer,   "cli2srv_pkts",  f->get_packets_cli2srv());
  ndpi_serialize_string_int32(serializer,   "srv2cli_pkts",  f->get_packets_srv2cli());
  ndpi_serialize_string_boolean(serializer, "is_severe",     is_severe);
  ndpi_serialize_string_boolean(serializer, "client_issues", is_client);
  ndpi_serialize_string_boolean(serializer, "server_issues", is_server);

  return(serializer);
}
