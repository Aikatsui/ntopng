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

#ifndef _FLOW_ALERTS_INCLUDES_H_
#define _FLOW_ALERTS_INCLUDES_H_

#include "flow_alerts/BlacklistedFlowAlert.h"
#include "flow_alerts/BlacklistedCountryAlert.h"
#include "flow_alerts/LowGoodputFlowAlert.h"
#include "flow_alerts/NotPurgedAlert.h"
#include "flow_alerts/PotentiallyDangerousFlowAlert.h"
#include "flow_alerts/RemoteToLocalInsecureProtoAlert.h"
#include "flow_alerts/RemoteToRemoteAlert.h"
#include "flow_alerts/TCPIssuesAlert.h"
#include "flow_alerts/TCPZeroWindowAlert.h"
#include "flow_alerts/UDPUnidirectionalAlert.h"
#include "flow_alerts/UnexpectedServerAlert.h"
#include "flow_alerts/UnexpectedDHCPServerAlert.h"
#include "flow_alerts/UnexpectedDNSServerAlert.h"
#include "flow_alerts/UnexpectedNTPServerAlert.h"
#include "flow_alerts/UnexpectedSMTPServerAlert.h"
#include "flow_alerts/UnexpectedServerAlert.h"
#include "flow_alerts/WebMiningAlert.h"

#endif /* _FLOW_ALERTS_INCLUDES_H_ */
