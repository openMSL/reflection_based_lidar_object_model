//
// Copyright 2022 Technical University of Darmstadt - FZD
// SPDX-License-Identifier: MPL-2.0
//

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif

#include "osibinsensorviewoutput/OSIBinSensorViewOutput.hpp"
#include <fstream>
#include <iostream>
#include <vector>
#include <ctime>

#ifdef _WIN32
    #include <math.h>
    #include <direct.h>
#else
    #include <cmath>
    #include <sys/stat.h>
#endif

using namespace model;
using namespace osi3;

void model::OSIBinSensorViewOutput::apply(SensorData &sensor_data) {
    log("Starting SensorView to .bin output");

    if (sensor_data.sensor_view_size() == 0) {
        log("No SensorView received for SensorView to .bin output");
        return;
    }

    if (sensor_data.sensor_view(0).has_global_ground_truth()) {
        /// Create folder on first call
        if (first_call) {
            #include <osibinsensorviewoutput/set_bin_file_path_sensorview.cpp>
            first_call = false;
        }
        write_data_to_bin(file_path_tracefile, sensor_data.sensor_view(0));
    }
}

void OSIBinSensorViewOutput::write_data_to_bin(const std::string& path, const SensorView &sensor_view) {
    typedef unsigned int message_size_t;
    std::ofstream bin_file (path,std::ios::binary | std::ios_base::app);

    std::string  osiMsgStringOnly = sensor_view.SerializeAsString();
    message_size_t message_size = osiMsgStringOnly.size();
    unsigned char ch[4];
    memcpy(ch, (char*)&message_size, sizeof(message_size_t));

    std::string  osiMsgString;
    osiMsgString += ch[0];
    osiMsgString += ch[1];
    osiMsgString += ch[2];
    osiMsgString += ch[3];
    osiMsgString += osiMsgStringOnly;

    bin_file.write(osiMsgString.c_str(), osiMsgString.length());
    bin_file.close();
    osiMsgString.clear();
}
