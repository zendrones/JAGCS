#ifndef MISSION_ITEM_H
#define MISSION_ITEM_H

// Internal
#include "command.h"

namespace data_source
{
    class MissionItem
    {
    public:
        MissionItem(int id);

        int id() const;

        int sequence() const;
        void setSequence(int sequence);

        Command command() const;
        void setCommand(Command command);

        float altitude() const;
        void setAltitude(float altitude);

        bool isAltitudeRelative() const;
        void setAltitudeRelative(bool altitudeRelative);

        double latitude() const;
        void setLatitude(double latitude);

        double longitude() const;
        void setLongitude(double longitude);

        float radius() const;
        void setRadius(float radius);

        float pitch() const;
        void setPitch(float pitch);

        int turns() const;
        void setTurns(int turns);

        bool operator ==(const MissionItem& other) const;

    private:
        const int m_id;
        int m_sequence;
        Command m_command;
        float m_altitude;
        bool m_altitudeRelative;
        double m_latitude;
        double m_longitude;
        float m_radius;
        float m_pitch;
        int m_turns;
    };
}

#endif // MISSION_ITEM_H
