#include "Arduino.h"                                                          // Arduino data type definitions    //
#ifndef INA226_Class_h                                                        // Guard code definition            //
  #define debug_Mode                                                          // Comment out when not needed      //
  #define INA226__Class_h                                                     // Define the name inside guard code//
 /*************************************************************************************************************************************
  ** Defines применяемые в этом классе                                                                                                **
  *************************************************************************************************************************************/
  #define AVERAGING_1                       B000                                //  усреднение 1
  #define AVERAGING_4                       B001                                //  усреднение 4
  #define AVERAGING_16                      B010                                //  усреднение 16
  #define AVERAGING_64                      B011                                //  усреднение 64
  #define AVERAGING_128                     B100                                //  усреднение 128
  #define AVERAGING_256                     B101                                //  усреднение 256
  #define AVERAGING_512                     B110                                //  усреднение 512
  #define AVERAGING_1024                    B111                                //  усреднение 1024
  #define CONFERSION_TIME_140uS             B000                                //  время преобразования АЦП 140 мксек
  #define CONFERSION_TIME_204uS             B001                                //  время преобразования АЦП 204 мксек
  #define CONFERSION_TIME_332uS             B010                                //  время преобразования АЦП 332 мксек
  #define CONFERSION_TIME_588uS             B011                                //  время преобразования АЦП 588 мксек
  #define CONFERSION_TIME_1100uS            B100                                //  время преобразования АЦП 1,1 мсек
  #define CONFERSION_TIME_2116uS            B101                                //  время преобразования АЦП 2,116 мсек
  #define CONFERSION_TIME_4156uS            B110                                //  время преобразования АЦП 4,156 мсек
  #define CONFERSION_TIME_8244uS            B111                                //  время преобразования АЦП 8,244 мсек
  #define MODE_POWERDOWN__TRIGGER           B000                                //  "спящий режим"
  #define MODE_SHUNT_TRIGGER                B001                                //  измерение напряжения шунта, триггерный
  #define MODE_BUS_TRIGGER                  B010                                //  измерение напряжения шины, триггерный
  #define MODE_SHUNT_BUS_TRIGGER            B011                                //  измерение напряжения шунта и шины, триггерный
  #define MODE_POWERDOWN_CONTINUOUS         B100                                //  "спящий режим"
  #define MODE_SHUNT_CONTINUOUS             B101                                //  измерение напряжения шунта, непрерывный
  #define MODE_BUS_CONTINUOUS               B110                                //  измерение напряжения шины, непрерывный
  #define MODE_SHUNT_BUS_CONTINUOUS         B111                                //  измерение напряжения шунта и шины, непрерывный
  #define SHUNT_VOLTAGE_OVER                B10000                              //  разрешить тревогу при превышении Ush limit
  #define SHUNT_VOLTAGE_UNDER               B01000                              //  разрешить тревогу при снижении Ush ниже limit
  #define BUS_VOLTAGE_OVER                  B00100                              //  разрешить тревогу при превышении Ubus limit
  #define BUS_VOLTAGE_UNDER                 B00010                              //  разрешить тревогу при снижении Ubus ниже limit
  #define POWER_OVER                        B00001                              //  разрешить тревогу при превышении Power limit
  #define CalibConst                        12500                               // Калибровочная константа (в идеале должна быть равна 12500)
  #define RealRefAccur                      CalibConst/12500                    //< Коэффициент кривости ИОН-а АЦП (при уменьшении этого значения показания вольтметра растут)
  /*****************************************************************************************************************
  ** Объявить структуры, используемые в классе                                                                         **
  *****************************************************************************************************************/
  typedef struct {                                                            // Structure of values per device   //
    uint8_t  address;                                                         // I2C Address of device            //
    uint16_t calibration;                                                     // Calibration register value       //
    uint32_t current_LSB;                                                     // Amperage LSB                     //
    uint32_t power_LSB;                                                       // Wattage LSB                      //
    uint8_t  operatingMode;                                                   // Default continuous mode operation//
  } inaDet; // of structure                                                   //                                  //
  /*****************************************************************************************************************
  ** Объявить константы, используемые в классе                                                                         **
  *****************************************************************************************************************/
  const uint8_t  I2C_DELAY                    =     10;                       // Microsecond delay on write       //
  const uint8_t  INA_CONFIGURATION_REGISTER   =      0;                       // Registers common to all INAs     //
  const uint8_t  INA_SHUNT_VOLTAGE_REGISTER   =      1;                       //                                  //
  const uint8_t  INA_BUS_VOLTAGE_REGISTER     =      2;                       //                                  //
  const uint8_t  INA_POWER_REGISTER           =      3;                       //                                  //
  const uint8_t  INA_CURRENT_REGISTER         =      4;                       //                                  //
  const uint8_t  INA_CALIBRATION_REGISTER     =      5;                       //                                  //
  const uint8_t  INA_MASK_ENABLE_REGISTER     =      6;                       //                                  //
  const uint8_t  INA_MANUFACTURER_ID_REGISTER =   0xFE;                       //                                  //
  const uint16_t INA_RESET_DEVICE             = 0x8000;                       // Write to configuration to reset  //
  const uint16_t INA_DEFAULT_CONFIGURATION    = 0x4127;                       // Default configuration register   //
  const uint16_t INA_BUS_VOLTAGE_LSB          =    125;                       // LSB in uV *100 1.25mV            //
  const uint16_t INA_SHUNT_VOLTAGE_LSB        =     25;                       // LSB in uV *10  2.5uV             //
  const uint16_t INA_CONFIG_AVG_MASK          = 0x0E00;                       // Bits 9-11                        //
  const uint16_t INA_CONFIG_BUS_TIME_MASK     = 0x01C0;                       // Bits 6-8                         //
  const uint16_t INA_CONFIG_SHUNT_TIME_MASK   = 0x0038;                       // Bits 3-5                         //
  const uint16_t INA_CONVERSION_READY_MASK    = 0x0080;                       // Bit 4                            //
  const uint16_t INA_CONFIG_MODE_MASK         = 0x0007;                       // Bits 0-3                         //
  const uint8_t  INA_MODE_TRIGGERED_SHUNT     =   B001;                       // Triggered shunt, no bus          //
  const uint8_t  INA_MODE_TRIGGERED_BUS       =   B010;                       // Triggered bus, no shunt          //
  const uint8_t  INA_MODE_TRIGGERED_BOTH      =   B011;                       // Triggered bus and shunt          //
  const uint8_t  INA_MODE_POWER_DOWN          =   B100;                       // shutdown or power-down           //
  const uint8_t  INA_MODE_CONTINUOUS_SHUNT    =   B101;                       // Continuous shunt, no bus         //
  const uint8_t  INA_MODE_CONTINUOUS_BUS      =   B110;                       // Continuous bus, no shunt         //
  const uint8_t  INA_MODE_CONTINUOUS_BOTH     =   B111;                       // Both continuous, default value   //
  /*****************************************************************************************************************
  ** Объявить заголовок класса                                                                                         **
  *****************************************************************************************************************/
  class INA226_Class {                                                        // Class definition                 //
    public:                                                                   // Publicly visible methods         //
      INA226_Class();                                                         // Class constructor                //
      ~INA226_Class();                                                        // Class destructor                 //
      uint32_t  begin(const uint32_t  maxBusAmps,                               // Class initializer                //
                     const uint32_t microOhmR,                                //                                  //
                     const uint8_t  deviceNumber = UINT8_MAX );               //                                  //
      uint16_t getBusMilliVolts(const bool waitSwitch=false,                  // Retrieve Bus voltage in mV       //
                                const uint8_t deviceNumber=0);                //                                  //
      int16_t  getShuntMicroVolts(const bool waitSwitch=false,                // Retrieve Shunt voltage in uV     //
                                  const uint8_t deviceNumber=0);              //                                  //
      int32_t  getBusMicroAmps(const uint8_t deviceNumber=0);                 // Retrieve micro-amps              //
      int32_t  getBusMicroWatts(const uint8_t deviceNumber=0);                // Retrieve micro-watts             //
      void     reset(const uint8_t deviceNumber=0);                           // Reset the device                 //
      void     setMode(const uint8_t mode,const uint8_t devNumber=UINT8_MAX); // Set the monitoring mode          //
      uint8_t  getMode(const uint8_t devNumber=UINT8_MAX);                    // Get the monitoring mode          //
      void     setAveraging(const uint16_t averages,                          // Set the number of averages taken //
                            const uint8_t deviceNumber=UINT8_MAX);            //                                  //
      void     setBusConversion(uint8_t convTime,                             // Set timing for Bus conversions   //
                                const uint8_t deviceNumber=UINT8_MAX);        //                                  //
      void     setShuntConversion(uint8_t convTime,                           // Set timing for Shunt conversions //
                                  const uint8_t deviceNumber=UINT8_MAX);      //                                  //
      void     waitForConversion(const uint8_t deviceNumber=UINT8_MAX);       // wait for conversion to complete  //
      void     setAlertPinOnConversion(const bool alertState,                 // Enable pin change on conversion  //
                                       const uint8_t deviceNumber=UINT8_MAX); //                                  //
    private:                                                                  // Private variables and methods    //
      uint8_t  readByte(const uint8_t addr, const uint8_t deviceAddress);     // Read a byte from an I2C address  //
      int16_t  readWord(const uint8_t addr, const uint8_t deviceAddress);     // Read a word from an I2C address  //
      void     writeByte(const uint8_t addr, const uint8_t data,              // Write a byte to an I2C address   //
                         const uint8_t deviceAddress);                        //                                  //
      void     writeWord(const uint8_t addr, const uint16_t data,             // Write two bytes to an I2C address//
                         const uint8_t deviceAddress);                        //                                  //
      uint8_t  _TransmissionStatus = 0;                                       // Return code for I2C transmission //
      uint8_t  _DeviceCount        = 0;                                       // Number of INA226s detected       //
  }; // of INA226_Class definition                                            //                                  //
#endif                                                                        //----------------------------------//
