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

void TCPIssues::checkFlow(Flow *f) {
  u_int16_t min_pkt_threshold = 10;
  u_int16_t normal_issues_ratio = 10; // 1/10
  u_int16_t severe_issues_ratio = 3;  // 1/3
  bool is_client = false, is_server = false, is_severe = false;
  
  ntop->getTrace()->traceEvent(TRACE_NORMAL, "%s()", __FUNCTION__);

  if(f->get_protocol() != IPPROTO_TCP) return; /* Non TCP traffic */

  if(f->getCliTcpIssues() > min_pkt_threshold) {
    u_int64_t pkts = f->get_packets_cli2srv();
        
    if((f->getCliTcpIssues() * severe_issues_ratio) > pkts)
      is_client = true, is_severe = true;
    else if((f->getCliTcpIssues() * normal_issues_ratio) > pkts)  
      is_client = true;
  }
  
  if(f->getSrvTcpIssues() > min_pkt_threshold) {
    u_int64_t pkts = f->get_packets_srv2cli();
        
    if((f->getSrvTcpIssues() * severe_issues_ratio) > pkts)
      is_server = true, is_severe = true;
    else if((f->getSrvTcpIssues() * normal_issues_ratio) > pkts)  
      is_server = true;
  }

  if(is_client || is_server) {
    u_int16_t fs_score = is_severe ? 20 : 10;

    /* TODO: Missing JSON */
    f->setStatus(this, is_severe ? severity_id : alert_level_info, fs_score /* f_score */, fs_score /* c_score */, fs_score /* s_score */);
  }
}

void TCPIssues::periodicUpdate(Flow *f) { checkFlow(f); }
void TCPIssues::flowEnd(Flow *f)        { checkFlow(f); }
