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

#ifndef _FLOW_CALLBACKS_INCLUDES_H_
#define _FLOW_CALLBACKS_INCLUDES_H_

#include "flow_callbacks/BlacklistedFlow.h"
#include "flow_callbacks/DNSDataExfiltration.h"
#include "flow_callbacks/DataExfiltration.h"
#include "flow_callbacks/DeviceProtocolNotAllowed.h"
#include "flow_callbacks/ElephantFlow.h"
#include "flow_callbacks/ExternalAlertCheck.h"
#include "flow_callbacks/FlowRisks.h"
#include "flow_callbacks/IEC60870_5_104.h"
#include "flow_callbacks/LowGoodputFlow.h"
#include "flow_callbacks/NotPurged.h"
#include "flow_callbacks/PotentiallyDangerousFlow.h"
#include "flow_callbacks/RemoteToLocalInsecureProto.h"
#include "flow_callbacks/RemoteToRemote.h"
#include "flow_callbacks/TCPIssues.h"
#include "flow_callbacks/TCPZeroWindow.h"
#include "flow_callbacks/TLSCertificateExpired.h"
#include "flow_callbacks/TLSCertificateMismatch.h"
#include "flow_callbacks/TLSOldProtocolVersion.h"
#include "flow_callbacks/TLSUnsafeCiphers.h"
#include "flow_callbacks/UDPUnidirectional.h"
#include "flow_callbacks/WebMining.h"

#ifdef NTOPNG_PRO
#include "flow_callbacks/InvalidDNSQuery.h"
#include "flow_callbacks/LongLivedFlow.h"
#include "flow_callbacks/NedgeBlockedFlow.h"
#include "flow_callbacks/PotentiallyDangerous.h"
#include "flow_callbacks/TCPProbing.h"
#include "flow_callbacks/TLSCertificateSelfSigned.h"
#include "flow_callbacks/TLSMaliciousSignature.h"
#endif

#endif /* _FLOW_CALLBACKS_INCLUDES_H_ */
