/*
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

#include <AP_Common/AP_Common.h>
#include <AP_HAL/AP_HAL.h>
#include "AP_Proximity.h"

#define PROXIMITY_SECTORS_MAX   12  // maximum number of sectors

class AP_Proximity_Backend
{
public:
    // constructor. This incorporates initialisation as well.
	AP_Proximity_Backend(AP_Proximity &_frontend, AP_Proximity::Proximity_State &_state);

    // we declare a virtual destructor so that Proximity drivers can
    // override with a custom destructor if need be
    virtual ~AP_Proximity_Backend(void) {}

    // update the state structure
    virtual void update() = 0;

    // get distance in meters in a particular direction in degrees (0 is forward, clockwise)
    // returns true on successful read and places distance in distance
    bool get_horizontal_distance(float angle_deg, float &distance) const;

    // get distance and angle to closest object (used for pre-arm check)
    //   returns true on success, false if no valid readings
    bool get_closest_object(float& angle_deg, float &distance) const;

protected:

    // set status and update valid_count
    void set_status(AP_Proximity::Proximity_Status status);

    // find which sector a given angle falls into
    bool convert_angle_to_sector(float angle_degrees, uint8_t &sector) const;

    AP_Proximity &frontend;
    AP_Proximity::Proximity_State &state;   // reference to this instances state

    // sectors
    uint8_t _num_sectors = 8;
    uint16_t _sector_middle_deg[PROXIMITY_SECTORS_MAX] = {0, 45, 90, 135, 180, 225, 270, 315, 0, 0, 0, 0};  // middle angle of each sector
    uint8_t _sector_width_deg[PROXIMITY_SECTORS_MAX] = {45, 45, 45, 45, 45, 45, 45, 45, 0, 0, 0, 0};        // width (in degrees) of each sector

    // sensor data
    float _angle[PROXIMITY_SECTORS_MAX];            // angle to closest object within each sector
    float _distance[PROXIMITY_SECTORS_MAX];         // distance to closest object within each sector
    bool _distance_valid[PROXIMITY_SECTORS_MAX];    // true if a valid distance received for each sector
};
