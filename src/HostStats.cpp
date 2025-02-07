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

/* *************************************** */

HostStats::HostStats(Host *_host) : GenericTrafficElement() {
  host = _host;
  iface = _host->getInterface();

  alerted_flows_as_client = alerted_flows_as_server = 0;
  unreachable_flows_as_client = unreachable_flows_as_server = 0;
  host_unreachable_flows_as_client = host_unreachable_flows_as_server = 0;
  udp_sent_unicast = udp_sent_non_unicast = 0;
  total_num_flows_as_client = total_num_flows_as_server = 0;
  total_alerts = tot_num_anomalies = 0;
  num_flow_alerts = 0;
  periodicUpdate = 0;
  
  /* NOTE: deleted by ~GenericTrafficElement */
  ndpiStats = new (std::nothrow) nDPIStats();
  //printf("SIZE: %lu, %lu, %lu\n", sizeof(nDPIStats), MAX_NDPI_PROTOS, NDPI_PROTOCOL_NUM_CATEGORIES);

  af_cli_anomaly = false, af_srv_anomaly = false, score_cli_anomaly = false, score_srv_anomaly = false;
  af_cli_value = af_srv_value = 0, af_srv_prediction = 0, af_srv_lower_bound = 0, af_srv_upper_bound = 0;
  af_cli_prediction = 0, af_cli_lower_bound = 0, af_cli_upper_bound = 0;
  score_cli_value = score_srv_value  = 0, score_srv_prediction = 0, score_srv_lower_bound = 0, score_srv_upper_bound = 0;
  score_cli_prediction = 0, score_cli_lower_bound = 0, score_cli_upper_bound = 0;
  
  dscpStats = new (std::nothrow) DSCPStats();

  last_epoch_update = 0;
  total_activity_time = 0;

#ifdef NTOPNG_PRO
  quota_enforcement_stats = quota_enforcement_stats_shadow = NULL;
#endif

  memset(&checkpoints, 0, sizeof(checkpoints));
}

/* *************************************** */

HostStats::~HostStats() {
#ifdef NTOPNG_PRO
  if(quota_enforcement_stats)        delete quota_enforcement_stats;
  if(quota_enforcement_stats_shadow) delete quota_enforcement_stats_shadow;
#endif
}

/* *************************************** */

void HostStats::updateStats(const struct timeval *tv) {
  GenericTrafficElement::updateStats(tv);

  if(tv->tv_sec >= periodicUpdate) {
    u_int32_t num_anomalies = tot_num_anomalies; /* Save old values */
    
    updateActiveFlowsBehaviour();
    updateScoreBehaviour();

    num_anomalies = tot_num_anomalies-num_anomalies; /* Calculate the difference */
    
    if(num_anomalies > 0) {
      if(host->isLocalHost())
	iface->incHostAnomalies(num_anomalies, 0);
      else
	iface->incHostAnomalies(0, num_anomalies);
    }
    
    periodicUpdate = tv->tv_sec + HOST_SITES_REFRESH; /* 5 min */
  }
}

/* *************************************** */

void HostStats::luaActiveFlowsBehaviour(lua_State* vm) {
  lua_newtable(vm);
  
  lua_push_bool_table_entry(vm, "as_client.anomaly",
			    score_cli_anomaly);
  lua_push_int32_table_entry(vm, "as_client.value",
			     score_cli_value); 
  lua_push_int32_table_entry(vm, "as_client.prediction",
			     score_cli_prediction); 
  lua_push_int32_table_entry(vm, "as_client.lower_bound",
			     score_cli_lower_bound);			     
  lua_push_int32_table_entry(vm, "as_client.upper_bound",
			     score_cli_upper_bound);
  
  lua_push_bool_table_entry(vm, "as_server.anomaly",
			    score_srv_anomaly);
  lua_push_int32_table_entry(vm, "as_server.value",
			     score_srv_value);
  lua_push_int32_table_entry(vm, "as_server.prediction",
			     score_srv_prediction);  
  lua_push_int32_table_entry(vm, "as_server.lower_bound",
			     score_srv_lower_bound);
  lua_push_int32_table_entry(vm, "as_server.upper_bound",
			     score_srv_upper_bound);

  lua_pushstring(vm, "active_flows_behaviour");
  lua_insert(vm, -2);
  lua_settable(vm, -3);  
}

/* *************************************** */

void HostStats::luaScoreBehaviour(lua_State* vm) {
  /* Client score behaviour */

  lua_newtable(vm);
  lua_push_bool_table_entry(vm, "as_client.anomaly",
			    af_cli_anomaly);
  lua_push_int32_table_entry(vm, "as_client.value",
			     af_cli_value); 
  lua_push_int32_table_entry(vm, "as_client.prediction",
			     af_cli_prediction); 
  lua_push_int32_table_entry(vm, "as_client.lower_bound",
			     af_cli_lower_bound);
  lua_push_int32_table_entry(vm, "as_client.upper_bound",
			     af_cli_upper_bound);

  /* Server score behaviour */
  lua_push_bool_table_entry(vm, "as_server.anomaly",
			    af_srv_anomaly);
  lua_push_int32_table_entry(vm, "as_server.value",
			     af_srv_value); 
  lua_push_int32_table_entry(vm, "as_server.prediction",
			     af_srv_prediction); 
  lua_push_int32_table_entry(vm, "as_server.lower_bound",
			     af_srv_lower_bound);
  lua_push_int32_table_entry(vm, "as_server.upper_bound",
			     af_srv_upper_bound);

  lua_push_int32_table_entry(vm, "tot_num_anomalies", tot_num_anomalies);
  
  lua_pushstring(vm, "score_behaviour");
  lua_insert(vm, -2);
  lua_settable(vm, -3);  
}

/* *************************************** */

void HostStats::luaHostBehaviour(lua_State* vm) {
  luaScoreBehaviour(vm);
  luaActiveFlowsBehaviour(vm);
}

/* *************************************** */

/* NOTE: this function is used by Lua to create the minute-by-minute host top talkers,
   both for remote and local hosts. Top talkerts are created by doing a checkpoint
   of the current value. */
void HostStats::checkpoint(lua_State* vm) {
  u_int64_t new_val;

  lua_newtable(vm);
  lua_newtable(vm);

  lua_push_uint64_table_entry(vm, "sent", (new_val = getNumBytesSent()) - checkpoints.sent_bytes);
  checkpoints.sent_bytes = new_val;

  lua_push_uint64_table_entry(vm, "rcvd", (new_val = getNumBytesRcvd()) - checkpoints.rcvd_bytes);
  checkpoints.rcvd_bytes = new_val;

  lua_pushstring(vm, "delta");
  lua_insert(vm, -2);
  lua_settable(vm, -3);
}

/* *************************************** */

/* NOTE: check out LocalHostStats for deserialization */
void HostStats::getJSONObject(json_object *my_object, DetailsLevel details_level) {
  if(details_level >= details_high) {
    json_object_object_add(my_object, "flows.as_client", json_object_new_int(getTotalNumFlowsAsClient()));
    json_object_object_add(my_object, "flows.as_server", json_object_new_int(getTotalNumFlowsAsServer()));
    json_object_object_add(my_object, "alerted_flows.as_client", json_object_new_int(getTotalAlertedNumFlowsAsClient()));
    json_object_object_add(my_object, "alerted_flows.as_server", json_object_new_int(getTotalAlertedNumFlowsAsServer()));
    json_object_object_add(my_object, "unreachable_flows.as_client", json_object_new_int(unreachable_flows_as_client));
    json_object_object_add(my_object, "unreachable_flows.as_server", json_object_new_int(unreachable_flows_as_server));
    json_object_object_add(my_object, "host_unreachable_flows.as_client", json_object_new_int(host_unreachable_flows_as_client));
    json_object_object_add(my_object, "host_unreachable_flows.as_server", json_object_new_int(host_unreachable_flows_as_server));

    json_object_object_add(my_object, "total_activity_time", json_object_new_int(total_activity_time));
    GenericTrafficElement::getJSONObject(my_object, iface);

    /* TCP stats */
    if(tcp_packet_stats_sent.seqIssues())
      json_object_object_add(my_object, "tcpPacketStats.sent", tcp_packet_stats_sent.getJSONObject());
    if(tcp_packet_stats_rcvd.seqIssues())
      json_object_object_add(my_object, "tcpPacketStats.recv", tcp_packet_stats_rcvd.getJSONObject());
  }
}

/* *************************************** */

/* NOTE: this method is also called by Host::lua
 * Return only the minimal information needed by the timeseries
 * to avoid slowing down the periodic scripts too much! */
void HostStats::luaStats(lua_State* vm, NetworkInterface *iface, bool host_details, bool verbose, bool tsLua) {
  /* NOTE: this class represents a (previously saved) timeseries point. Do not access Host data and push it directly here! */
  lua_push_uint64_table_entry(vm, "bytes.sent", sent.getNumBytes());
  lua_push_uint64_table_entry(vm, "bytes.rcvd", rcvd.getNumBytes());
  lua_push_uint64_table_entry(vm, "total_flows.as_client", total_num_flows_as_client);
  lua_push_uint64_table_entry(vm, "total_flows.as_server", total_num_flows_as_server);

  if(verbose) {
    if(ndpiStats) ndpiStats->lua(iface, vm, true, tsLua);
    if(dscpStats) dscpStats->lua(iface, vm, tsLua);
  }

  if(host_details) {
    lua_push_uint64_table_entry(vm, "alerted_flows.as_client", getTotalAlertedNumFlowsAsClient());
    lua_push_uint64_table_entry(vm, "alerted_flows.as_server", getTotalAlertedNumFlowsAsServer());
    lua_push_uint64_table_entry(vm, "unreachable_flows.as_client", unreachable_flows_as_client);
    lua_push_uint64_table_entry(vm, "unreachable_flows.as_server", unreachable_flows_as_server);
    lua_push_uint64_table_entry(vm, "host_unreachable_flows.as_client", host_unreachable_flows_as_client);
    lua_push_uint64_table_entry(vm, "host_unreachable_flows.as_server", host_unreachable_flows_as_server);
    lua_push_uint64_table_entry(vm, "total_alerts", getTotalAlerts());
    lua_push_uint64_table_entry(vm, "num_flow_alerts", num_flow_alerts);

    l4stats.luaStats(vm);
    lua_push_uint64_table_entry(vm, "udpBytesSent.unicast", udp_sent_unicast);
    lua_push_uint64_table_entry(vm, "udpBytesSent.non_unicast", udp_sent_non_unicast);
  }
}

/* *************************************** */

void HostStats::lua(lua_State* vm, bool mask_host, DetailsLevel details_level) {
  if(details_level >= details_high)
    lua_push_uint64_table_entry(vm, "bytes.ndpi.unknown", getnDPIStats() ? getnDPIStats()->getProtoBytes(NDPI_PROTOCOL_UNKNOWN) : 0);

  if(details_level >= details_max) {
#ifdef NTOPNG_PRO
    if(custom_app_stats) custom_app_stats->lua(vm);
#endif

    sent_stats.lua(vm, "pktStats.sent");
    recv_stats.lua(vm, "pktStats.recv");
  }

  lua_push_bool_table_entry(vm, "tcp.packets.seq_problems",
			    tcp_packet_stats_sent.seqIssues() || tcp_packet_stats_rcvd.seqIssues() ? true : false);
  tcp_packet_stats_sent.lua(vm, "tcpPacketStats.sent");
  tcp_packet_stats_rcvd.lua(vm, "tcpPacketStats.rcvd");

  if(details_level >= details_higher) {
    /* Bytes anomalies */
    l4stats.luaAnomalies(vm);
  
    lua_push_uint64_table_entry(vm, "total_activity_time", total_activity_time);
    lua_push_uint64_table_entry(vm, "flows.as_client", getTotalNumFlowsAsClient());
    lua_push_uint64_table_entry(vm, "flows.as_server", getTotalNumFlowsAsServer());
  }

  if(details_level >= details_high) {
    ((GenericTrafficElement*)this)->lua(vm, details_level >= details_higher);
    luaStats(vm, iface, details_level >= details_higher, details_level >= details_max, false);
  }
}

/* *************************************** */

u_int32_t HostStats::getTotalAlerts() const {
  return total_alerts;
}

/* *************************************** */

void HostStats::incStats(time_t when, u_int8_t l4_proto,
			 u_int ndpi_proto, ndpi_protocol_category_t ndpi_category,
			 custom_app_t custom_app,
			 u_int64_t sent_packets, u_int64_t sent_bytes, u_int64_t sent_goodput_bytes,
			 u_int64_t rcvd_packets, u_int64_t rcvd_bytes, u_int64_t rcvd_goodput_bytes,
			 bool peer_is_unicast) {
  sent.incStats(when, sent_packets, sent_bytes),
    rcvd.incStats(when, rcvd_packets, rcvd_bytes);
  
  if(ndpiStats) {
    ndpiStats->incStats(when, ndpi_proto, sent_packets, sent_bytes, rcvd_packets, rcvd_bytes),
      ndpiStats->incCategoryStats(when, ndpi_category, sent_bytes, rcvd_bytes);
  }

#ifdef NTOPNG_PRO
  if(custom_app.pen
     && (custom_app_stats || (custom_app_stats = new(std::nothrow) CustomAppStats(iface)))) {
    custom_app_stats->incStats(custom_app.remapped_app_id, sent_bytes + rcvd_bytes); 
  }
#endif

  if(when && when - last_epoch_update >= ntop->getPrefs()->get_housekeeping_frequency())
    total_activity_time += ntop->getPrefs()->get_housekeeping_frequency(), last_epoch_update = when;

  /* Packet stats sent_stats and rcvd_stats are incremented in Flow::incStats */
  l4stats.incStats(when, l4_proto, rcvd_packets, rcvd_bytes, sent_packets, sent_bytes);
}

#ifdef NTOPNG_PRO

/* *************************************** */

void HostStats::allocateQuotaEnforcementStats() {
  if(!quota_enforcement_stats) {
    quota_enforcement_stats = new (std::nothrow) HostPoolStats(iface);
    
#ifdef HOST_POOLS_DEBUG
    char buf[128];
    ntop->getTrace()->traceEvent(TRACE_NORMAL,
				 "Allocating quota stats for %s [quota_enforcement_stats: %p] [host pool: %i]",
				 ip.print(buf, sizeof(buf)), (void*)quota_enforcement_stats, host_pool_id);
#endif
  }
}

/* *************************************** */

void HostStats::deleteQuotaEnforcementStats() {
  if(quota_enforcement_stats_shadow) {
    delete quota_enforcement_stats_shadow;
    quota_enforcement_stats_shadow = NULL;

#ifdef HOST_POOLS_DEBUG
    char buf[128];
    ntop->getTrace()->traceEvent(TRACE_NORMAL,
				 "Freeing shadow pointer of longer quota stats for %s [host pool: %i]",
				 ip.print(buf, sizeof(buf)), host_pool_id);
#endif
  }

  if(quota_enforcement_stats) {
    quota_enforcement_stats_shadow = quota_enforcement_stats;
    quota_enforcement_stats = NULL;

#ifdef HOST_POOLS_DEBUG
    char buf[128];
    ntop->getTrace()->traceEvent(TRACE_NORMAL,
				 "Moving quota stats to the shadow pointer for %s [host pool: %i]",
				 ip.print(buf, sizeof(buf)), host_pool_id);
#endif
  }
}

#endif

/* *************************************** */

void HostStats::updateActiveFlowsBehaviour() {
  if((af_cli_anomaly = active_flows_cli.addObservation((af_cli_value = host->getNumOutgoingFlows()), &af_cli_prediction, &af_cli_lower_bound, &af_cli_upper_bound)) == 1) tot_num_anomalies++;
  if((af_srv_anomaly = active_flows_srv.addObservation((af_srv_value = host->getNumIncomingFlows()), &af_srv_prediction, &af_srv_lower_bound, &af_srv_upper_bound)) == 1) tot_num_anomalies++; 
}

/* *************************************** */

void HostStats::updateScoreBehaviour() {
  if((score_cli_anomaly = score_cli.addObservation((score_cli_value = host->getScoreAsClient()), &score_cli_prediction, &score_cli_lower_bound, &score_cli_upper_bound)) == 1) tot_num_anomalies++;
  if((score_srv_anomaly = score_srv.addObservation((score_srv_value = host->getScoreAsServer()), &score_srv_prediction, &score_srv_lower_bound, &score_srv_upper_bound)) == 1) tot_num_anomalies++;
}
