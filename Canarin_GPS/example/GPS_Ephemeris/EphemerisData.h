#include <TinyGPSPlus.h>
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
  // Check if ephemeris data is valid based on specific criteria
  // For example, you can check if the time of ephemeris (toe) is within a reasonable range
  // and if other essential data fields are within acceptable limits
  
  // Check if the time of ephemeris is within a reasonable range (e.g., not too far in the past or future)
  if (ephData.toe < MIN_TOE || ephData.toe > MAX_TOE) {
    return false;
  }

  // Check other criteria as needed
  // For example, you can check the validity of other ephemeris data fields here

  // If all criteria pass, consider the ephemeris data as valid
  return true;
  }
};
