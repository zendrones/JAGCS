#include "gps_handler.h"

// MAVLink
#include <mavlink.h>

// Qt
#include <QVariant>
#include <QGeoCoordinate>
#include <QDateTime>

// Internal
#include "mavlink_protocol_helpers.h"

#include "service_registry.h"
#include "telemetry_service.h"
#include "telemetry_portion.h"

using namespace comm;
using namespace domain;

GpsHandler::GpsHandler(MavLinkCommunicator* communicator):
    AbstractMavLinkHandler(communicator),
    m_telemetryService(ServiceRegistry::telemetryService())
{} // TODO: handle GPS_STATUS

void GpsHandler::processMessage(const mavlink_message_t& message)
{
    if (message.msgid != MAVLINK_MSG_ID_GPS_RAW_INT) return;

    TelemetryPortion portion(m_telemetryService->mavNode(message.sysid));

    mavlink_gps_raw_int_t gps;
    mavlink_msg_gps_raw_int_decode(&message, &gps);

    QGeoCoordinate coordinate(decodeLatLon(gps.lat), decodeLatLon(gps.lon),
                              decodeAltitude(gps.alt));

    portion.setParameter({ Telemetry::Satellite, Telemetry::Fix }, gps.fix_type);
    portion.setParameter({ Telemetry::Satellite, Telemetry::Coordinate },
                         QVariant::fromValue(coordinate));
    portion.setParameter({ Telemetry::Satellite, Telemetry::Groundspeed },
                         decodeGroundSpeed(gps.vel));
    portion.setParameter({ Telemetry::Satellite, Telemetry::Course },
                         decodeCourse(gps.cog));
    portion.setParameter({ Telemetry::Satellite, Telemetry::Altitude },
                         decodeAltitude(gps.alt));
    portion.setParameter({ Telemetry::Satellite, Telemetry::Eph }, gps.eph);
    portion.setParameter({ Telemetry::Satellite, Telemetry::Epv }, gps.epv);
    portion.setParameter({ Telemetry::Satellite, Telemetry::Time },
                         QDateTime::fromMSecsSinceEpoch(gps.time_usec));
    portion.setParameter({ Telemetry::Satellite, Telemetry::SatellitesVisible },
                         gps.satellites_visible);
}
