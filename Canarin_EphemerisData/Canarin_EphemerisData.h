
#ifndef CANARIN_EPHEMERIS_H_
#define CANARIN_EPHEMERIS_H_

#include <TinyGPS++.h>
#include <HardwareSerial.h>

class EphemerisData {
public:
  // Define ephemeris data fields (add more as needed)
  double toe; // Time of ephemeris
  double rootA; // Square root of the semi-major axis
  double e; // Eccentricity
  double deltaN; // Mean motion difference
  double i0; // Inclination angle at reference time
  // ... Add more ephemeris data fields as required

  // Constructor
  EphemerisData() {
    // Initialize ephemeris data fields
    toe = 0.0;
    rootA = 0.0;
    e = 0.0;
    deltaN = 0.0;
    i0 = 0.0;
    // ... Initialize other ephemeris data fields
  }
  
  // Function to update ephemeris data based on received data
  bool update(const TinyGPSEphemeris& ephData) {
    // Check if ephemeris data is valid
    if (!ephData.isUpdated()) {
      return false;
    }

    // Update ephemeris data fields
    toe = ephData.toe();
    rootA = ephData.rootA();
    e = ephData.e();
    deltaN = ephData.deltaN();
    i0 = ephData.i0();
    // ... Update other ephemeris data fields as needed

    return true;
  }

  // Function to check if ephemeris data is valid
  bool isValid() const {
    // You can implement your own validity checks here
    // For example, check if the data is within a valid range or has been updated recently
    // Return true if the data is considered valid, otherwise false
    return true; // Replace with your logic
  }
};

#endif