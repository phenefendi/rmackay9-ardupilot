// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-
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

#include "AP_Beacon.h"
#include "AP_Beacon_Backend.h"
#include "AP_Beacon_Pozyx.h"

extern const AP_HAL::HAL &hal;

// table of user settable parameters
const AP_Param::GroupInfo AP_Beacon::var_info[] = {

    // @Param: _TYPE
    // @DisplayName: Beacon based position estimation device type
    // @Description: What type of beacon based position estimation device is connected
    // @Values: 0:None,1:Pozyx
    // @User: Advanced
    AP_GROUPINFO("_TYPE",    0, AP_Beacon, _type, 0),

    // @Param: _LATITUDE
    // @DisplayName: Beacon origin's latitude
    // @Description: Beacon origin's latitude
    // @Units: degrees
    // @Increment: 0.000001
    // @Range: -90 90
    // @User: Advanced
    AP_GROUPINFO("_LATITUDE", 1, AP_Beacon, origin_lat, 0),

    // @Param: _LONGITUDE
    // @DisplayName: Beacon origin's longitude
    // @Description: Beacon origin's longitude
    // @Units: degrees
    // @Increment: 0.000001
    // @Range: -180 180
    // @User: Advanced
    AP_GROUPINFO("_LONGITUDE", 2, AP_Beacon, origin_lon, 0),

    // @Param: _ALT
    // @DisplayName: Beacon origin's altitude above sealevel in meters
    // @Description: Beacon origin's altitude above sealevel in meters
    // @Units: meters
    // @Increment: 1
    // @Range: 0 10000
    // @User: Advanced
    AP_GROUPINFO("_ALT", 3, AP_Beacon, origin_alt, 0),

    AP_GROUPEND
};

AP_Beacon::AP_Beacon(AP_SerialManager &_serial_manager) :
    serial_manager(_serial_manager)
{
    AP_Param::setup_object_defaults(this, var_info);
}

// initialise the AP_Beacon class
void AP_Beacon::init(void)
{
    if (_driver != NULL) {
        // init called a 2nd time?
        return;
    }

    // create backend
    if (_type == AP_BeaconType_Pozyx) {
        _driver = new AP_Beacon_Pozyx(*this, serial_manager);
    }
}

// update state. This should be called often from the main loop
void AP_Beacon::update(void)
{
    if (_driver == NULL || _type == AP_BeaconType_None) {
        return;
    }
    _driver->update();
}

// return origin of position estimate system
bool AP_Beacon::get_origin(Location &origin_loc) const
{
    if (_driver == NULL || _type == AP_BeaconType_None) {
        return false;
    }

    // check for unitialised origin
    if (is_zero(origin_lat) && is_zero(origin_lon) && is_zero(origin_alt)) {
        return false;
    }

    // return origin
    origin_loc.lat = origin_lat * 10e7;
    origin_loc.lng = origin_lon * 10e7;
    origin_loc.alt = origin_alt * 100;
    origin_loc.options = 0; // all flags to zero meaning alt-above-sea-level

    return true;
}

// return position in NED from position estimate system's origin
bool AP_Beacon::get_vehicle_position_ned(Vector3f &position, float& accuracy_estimate) const
{
    if (_driver == NULL || _type == AP_BeaconType_None) {
        return false;
    }

    // check for timeout
    if (AP_HAL::millis() - veh_pos_update_ms > AP_BEACON_TIMEOUT_MS) {
        return false;
    }

    // return position
    position = veh_pos_ned;
    accuracy_estimate = veh_pos_accuracy;
    return true;
}

// return the number of beacons
uint8_t AP_Beacon::count() const
{
    if (_driver == NULL || _type == AP_BeaconType_None) {
        return 0;
    }
    return num_beacons;
}

// return all beacon data
bool AP_Beacon::get_beacon_data(uint8_t beacon_instance, struct BeaconState& state) const
{
    if (_driver == NULL || _type == AP_BeaconType_None || beacon_instance >= num_beacons) {
        return false;
    }
    state = beacon_state[beacon_instance];
    return true;
}
