#include "INA226.h"                                                           // Include the header definition    //
#include <Wire.h>                                                             // I2C Library definition           //
#include <EEPROM.h>                                                           // Include the EEPROM library       //
INA226_Class::INA226_Class()  {}                                              // Class constructor                //
INA226_Class::~INA226_Class() {}                                              // Unused class destructor          //
/*******************************************************************************************************************
** Метод begin () устанавливает детали конфигурации INA226, без которых не могут быть сделаны значимые чтения. 
* Если он вызывается без параметра option deviceNumber, тогда настройки применяются ко всем устройствам, 
* в противном случае - только это конкретное устройство.                                                          **
*******************************************************************************************************************/
uint32_t INA226_Class::begin(const uint32_t maxBusAmps,                         // Class initializer                //
                            const uint32_t microOhmR,                         //                                  //
                            const uint8_t deviceNumber ) {                    //                                  //
  inaDet ina;                                                                 // Hold device details in structure //
  uint16_t manufacturerId;                                                    // Read manufacturerId from device  //
  if (_DeviceCount==0) {                                                      // Enumerate devices in first call  //
    Wire.begin();                                                             // Start the I2C wire subsystem     //
    for(uint8_t deviceAddress = 64;deviceAddress<79;deviceAddress++) {        // Loop for each possible address   //
      Wire.beginTransmission(deviceAddress);                                  // See if something is at address   //
      if (Wire.endTransmission() == 0) {                                      // by checking the return error     //
        if (readWord(INA_MANUFACTURER_ID_REGISTER,deviceAddress)==0x5449) {   // Check hard-coded manufacturerId  //
          writeWord(INA_CONFIGURATION_REGISTER,INA_RESET_DEVICE,deviceAddress);// Force INAs to reset             //
          delay(I2C_DELAY);                                                   // Wait for INA to finish resetting //
          if (readWord(INA_CONFIGURATION_REGISTER,deviceAddress)              // Yes, we've found an INA226!      //
              ==INA_DEFAULT_CONFIGURATION) {                                  //                                  //
            ina.address       = deviceAddress;                                // Store device address             //
            ina.operatingMode = B111;                                         // Default to continuous mode       //
            if ((_DeviceCount*sizeof(ina))<EEPROM.length()) {                 // If there's space left in EEPROM  //
              EEPROM.put(_DeviceCount*sizeof(ina),ina);                       // Add the structure                //
              _DeviceCount++;                                                 // Increment the device counter     //
            } // of if-then the values will fit into EEPROM                   //                                  //
          } // of if-then we have identified a INA226                         //                                  //
        } // of if-then we have identified a INA226 manufacturer code         //                                  //
      } // of if-then we have found a live device                             //                                  //
    } // for-next each possible I2C address                                   //                                  //
  } // of if-then first call with no devices found                            //                                  //
  EEPROM.get((deviceNumber%_DeviceCount)*sizeof(ina),ina);                    // Read EEPROM values for device    //
  ina.current_LSB = (uint64_t)maxBusAmps*1000000000/32767;                    // Get the best possible LSB in nA  //
  ina.calibration = (uint64_t)51200000 / ((uint64_t)ina.current_LSB *         // Compute calibration register     //
                    (uint64_t)microOhmR / (uint64_t)100000);                  // using 64 bit numbers throughout  //
  ina.power_LSB   = (uint32_t)25*ina.current_LSB;                             // Fixed multiplier for INA219      //
  #ifdef debug_Mode                                                           // Display values when debugging    //
  Serial.print(F("current_LSB = ")); Serial.println(ina.current_LSB);         //                                  //
  Serial.print(F("calibration =1,5 ")); Serial.println(ina.calibration);         //  Кажись здесь                                //
  Serial.print(F("power_LSB   = ")); Serial.println(ina.power_LSB);           //                                  //
  #endif                                                                      // end of conditional compile code  //
  if (deviceNumber==UINT8_MAX) {                                              // If default value, then set all   //
    for(uint8_t i=0;i<_DeviceCount;i++) {                                     // For each device write data       //
      EEPROM.put(i*sizeof(ina),ina);                                          // Write value to address           //
      writeWord(INA_CALIBRATION_REGISTER,ina.calibration,ina.address);        // Write the calibration value      //
    } // of for each device                                                   //                                  //
  } else {                                                                    //                                  //
    EEPROM.put((deviceNumber%_DeviceCount)*sizeof(ina),ina);                  // Write struct, cater for overflow //
    writeWord(INA_CALIBRATION_REGISTER,ina.calibration,ina.address);          // Write the calibration value      //
  } // of if-then-else set one or all devices                                 //                                  //
  return _DeviceCount;                                                        // Return number of devices found   //
} // of method begin()                                                        //                                  //
/*******************************************************************************************************************
** Метод readByte читает 1 байт с указанного адреса                                                      **
*******************************************************************************************************************/
uint8_t INA226_Class::readByte(const uint8_t addr,const uint8_t deviceAddr){  //                                  //
  Wire.beginTransmission(deviceAddr);                                         // Address the I2C device           //
  Wire.write(addr);                                                           // Send the register address to read//
  _TransmissionStatus = Wire.endTransmission();                               // Close transmission               //
  delayMicroseconds(I2C_DELAY);                                               // delay required for sync          //
  Wire.requestFrom(deviceAddr, (uint8_t)1);                                   // Request 1 byte of data           //
  return Wire.read();                                                         // read it and return it            //
} // of method readByte()                                                     //                                  //
/*******************************************************************************************************************
** Метод readWord читает 2 байта с указанного адреса                                                      **
*******************************************************************************************************************/
int16_t INA226_Class::readWord(const uint8_t addr,const uint8_t deviceAddr){  //                                  //
  int16_t returnData;                                                         // Store return value               //
  Wire.beginTransmission(deviceAddr);                                         // Address the I2C device           //
  Wire.write(addr);                                                           // Send the register address to read//
  _TransmissionStatus = Wire.endTransmission();                               // Close transmission               //
  delayMicroseconds(I2C_DELAY);                                               // delay required for sync          //
  Wire.requestFrom(deviceAddr, (uint8_t)2);                                   // Request 2 consecutive bytes      //
  returnData = Wire.read();                                                   // Read the msb                     //
  returnData = returnData<<8;                                                 // shift the data over              //
  returnData|= Wire.read();                                                   // Read the lsb                     //
  return returnData;                                                          // read it and return it            //
} // of method readWord()                                                     //                                  //
/*******************************************************************************************************************
** Метод writeByte записывает 1 байт по указанному адресу                                                      **
*******************************************************************************************************************/
void INA226_Class::writeByte(const uint8_t addr, const uint8_t data,          //                                  //
                             const uint8_t deviceAddr) {                      //                                  //
  Wire.beginTransmission(deviceAddr);                                         // Address the I2C device           //
  Wire.write(addr);                                                           // Send register address to write   //
  Wire.write(data);                                                           // Send the data to write           //
  _TransmissionStatus = Wire.endTransmission();                               // Close transmission               //
} // of method writeByte()                                                    //                                  //
/*******************************************************************************************************************
** Метод writeWord записывает 2 байта по указанному адресу                                                       **
*******************************************************************************************************************/
void INA226_Class::writeWord(const uint8_t addr, const uint16_t data,         //                                  //
                            const uint8_t deviceAddr) {                       //                                  //
  Wire.beginTransmission(deviceAddr);                                         // Address the I2C device           //
  Wire.write(addr);                                                           // Send register address to write   //
  Wire.write((uint8_t)(data>>8));                                             // Write the first byte             //
  Wire.write((uint8_t)data);                                                  // and then the second              //
  _TransmissionStatus = Wire.endTransmission();                               // Close transmission               //
} // of method writeWord()                                                    //                                  //
/*******************************************************************************************************************
** Метод getBusMilliVolts извлекает измерение напряжения на шине.                                                **
*******************************************************************************************************************/
uint16_t INA226_Class::getBusMilliVolts(const bool waitSwitch,                //                                  //
                                        const uint8_t deviceNumber) {         //                                  //
  inaDet ina;                                                                 // Хранить детали устройства в структуре //
  EEPROM.get((deviceNumber%_DeviceCount)*sizeof(ina),ina);                    // Чтение значений EEPROM               //
  if (waitSwitch) waitForConversion();                                        // дождитесь завершения преобразования  //
  uint16_t busVoltage = readWord(INA_BUS_VOLTAGE_REGISTER,ina.address);       // Получите исходное значение и примените      //
  busVoltage = (uint32_t)busVoltage*INA_BUS_VOLTAGE_LSB/100* RealRefAccur;    // mV // преобразование для получения милливольт     //
  if (!bitRead(ina.operatingMode,2) && bitRead(ina.operatingMode,1)) {        // Если режим срабатывания и шина активна //
    int16_t configRegister = readWord(INA_CONFIGURATION_REGISTER,ina.address);// Получить текущий реестр        //
    writeWord(INA_CONFIGURATION_REGISTER,configRegister,ina.address);         // Напишите, чтобы запустить следующий       //
  } // of if-then triggered mode enabled                                      //                                  //
  return(busVoltage);                                                         // вернуть вычисленные милливольт      //
} // of method getBusMilliVolts()                                             //                                  //
/*******************************************************************************************************************
** Метод getShuntMicroVolts извлекает результаты измерения напряжения шунта.                                            **
*******************************************************************************************************************/
int16_t INA226_Class::getShuntMicroVolts(const bool waitSwitch,               //                                  //
                                         const uint8_t deviceNumber) {        //                                  //
  inaDet ina;                                                                 // Hold device details in structure //
  EEPROM.get((deviceNumber%_DeviceCount)*sizeof(ina),ina);                    // Read EEPROM values               //
  if (waitSwitch) waitForConversion();                                        // wait for conversion to complete  //
  int32_t shuntVoltage = readWord(INA_SHUNT_VOLTAGE_REGISTER,ina.address);    // Get the raw value                //
Serial.print("shuntVoltageRaw = ");Serial.println(shuntVoltage);
  shuntVoltage = shuntVoltage*INA_SHUNT_VOLTAGE_LSB/10;                       // Convert to microvolts            //
  if (!bitRead(ina.operatingMode,2) && bitRead(ina.operatingMode,0)) {        // If triggered and shunt active    //
    int16_t configRegister = readWord(INA_CONFIGURATION_REGISTER,ina.address);// Get the current register         //
    writeWord(INA_CONFIGURATION_REGISTER,configRegister,ina.address);         // Write back to trigger next       //
  } // of if-then triggered mode enabled                                      //                                  //
  return((int16_t)shuntVoltage);                                              // return computed microvolts       //
} // of method getShuntMicroVolts()                                           //                                  //
/*******************************************************************************************************************
** Метод getBusMicroAmps извлекает вычисленный ток в микроамперах.                                            **
*******************************************************************************************************************/
int32_t INA226_Class::getBusMicroAmps(const uint8_t deviceNumber) {           //                                  //
  inaDet ina;                                                                 // Hold device details in structure //
  EEPROM.get((deviceNumber%_DeviceCount)*sizeof(ina),ina);                    // Read EEPROM values               //
  int32_t microAmps = readWord(INA_CURRENT_REGISTER,ina.address);             // Get the raw value                //

Serial.print("BusCurrentRaw = ");Serial.println(microAmps);
          microAmps = (int64_t)microAmps*ina.current_LSB/100000;                // Convert to microamps             //
  return(microAmps);                                                          // return computed microamps        //
} // of method getBusMicroAmps()                                              //                                  //
/*******************************************************************************************************************
** Метод getBusMicroWatts получает вычисленную мощность в милливаттах.                                           **
*******************************************************************************************************************/
int32_t INA226_Class::getBusMicroWatts(const uint8_t deviceNumber) {          //                                  //
  inaDet ina;                                                                 // Hold device details in structure //
  EEPROM.get((deviceNumber%_DeviceCount)*sizeof(ina),ina);                    // Read EEPROM values               //
  int32_t microWatts = readWord(INA_POWER_REGISTER,ina.address);              // Get the raw value                //
          microWatts = (int64_t)microWatts*ina.power_LSB/1000;                // Convert to milliwatts            //
  return(microWatts);                                                         // return computed milliwatts       //
} // of method getBusMicroWatts()                                             //                                  //
/*******************************************************************************************************************
** Сброс метода сбрасывает INA226 с использованием первого бита в регистре конфигурации.                        **
*******************************************************************************************************************/
void INA226_Class::reset(const uint8_t deviceNumber) {                        // Reset the INA226                 //
  inaDet ina;                                                                 // Hold device details in structure //
  int16_t configRegister;                                                     // Hold configuration register      //
  for(uint8_t i=0;i<_DeviceCount;i++) {                                       // Loop for each device found       //
    if(deviceNumber==UINT8_MAX || deviceNumber%_DeviceCount==i ) {            // If this device needs setting     //
      EEPROM.get(i*sizeof(ina),ina);                                          // Read EEPROM values               //
      writeWord(INA_CONFIGURATION_REGISTER,0x8000,ina.address);               // Set most significant bit         //
      delay(I2C_DELAY);                                                       // Let the INA226 reboot            //
    } // of if this device needs to be set                                    //                                  //
  } // for-next each device loop                                              //                                  //
} // of method reset                                                          //                                  //
/*******************************************************************************************************************
** Метод getMode возвращает текущий режим мониторинга выбранного устройства                                     **
*******************************************************************************************************************/
uint8_t INA226_Class::getMode(const uint8_t deviceNumber ) {                  // Return the monitoring mode       //
  inaDet ina;                                                                 // Hold device details in structure //
  uint8_t tempDevice = deviceNumber+1;                                        // Temporary device number storage  //
  EEPROM.get(tempDevice*sizeof(ina),ina);                                     // Read EEPROM values               //
  return(ina.operatingMode);                                                  // Return stored value              //
} // of method getMode()                                                      //                                  //
/*******************************************************************************************************************
** Метод setMode позволяет устанавливать различные комбинации режимов. 
* Если параметр не задан, система возвращается в режим запуска по умолчанию.                                   **
*******************************************************************************************************************/
void INA226_Class::setMode(const uint8_t mode,const uint8_t deviceNumber ) {  // Set the monitoring mode          //
  inaDet ina;                                                                 // Hold device details in structure //
  int16_t configRegister;                                                     // Hold configuration register      //
  for(uint8_t i=0;i<_DeviceCount;i++) {                                       // Loop for each device found       //
    if(deviceNumber==UINT8_MAX || deviceNumber%_DeviceCount==i ) {            // If this device needs setting     //
      EEPROM.get(i*sizeof(ina),ina);                                          // Read EEPROM values               //
      configRegister = readWord(INA_CONFIGURATION_REGISTER,ina.address);      // Get the current register         //
      configRegister &= ~INA_CONFIG_MODE_MASK;                                // zero out the mode bits           //
      ina.operatingMode = B00001111 & mode;                                   // Mask off unused bits             //
      EEPROM.put((deviceNumber%_DeviceCount)*sizeof(ina),ina);                // Write new EEPROM values          //
      configRegister |= ina.operatingMode;                                    // shift in the mode settings       //
      writeWord(INA_CONFIGURATION_REGISTER,configRegister,ina.address);       // Save new value                   //
    } // of if this device needs to be set                                    //                                  //
  } // for-next each device loop                                              //                                  //
} // of method setMode()                                                      //                                  //
/*******************************************************************************************************************
** Набор методов: усреднение устанавливает аппаратное усреднение для различных устройств.                       **
*******************************************************************************************************************/
void INA226_Class::setAveraging(const uint16_t averages,                      // Set the number of averages taken //
                                const uint8_t deviceNumber ) {                //                                  //
  uint8_t averageIndex;                                                       // Store indexed value for register //
  int16_t configRegister;                                                     // Configuration register contents  //
  inaDet ina;                                                                 // Hold device details in structure //
  EEPROM.get((deviceNumber%_DeviceCount)*sizeof(ina),ina);                    // Read EEPROM values               //
  for(uint8_t i=0;i<_DeviceCount;i++) {                                       // Loop for each device found       //
    if(deviceNumber==UINT8_MAX || deviceNumber%_DeviceCount==i ) {            // If this device needs setting     //
      configRegister = readWord(INA_CONFIGURATION_REGISTER,ina.address);      // Get the current register         //
      if      (averages>=1024) averageIndex = 7;                              // setting depending upon range     //
      else if (averages>= 512) averageIndex = 6;                              //                                  //
      else if (averages>= 256) averageIndex = 5;                              //                                  //
      else if (averages>= 128) averageIndex = 4;                              //                                  //
      else if (averages>=  64) averageIndex = 3;                              //                                  //
      else if (averages>=  16) averageIndex = 2;                              //                                  //
      else if (averages>=   4) averageIndex = 1;                              //                                  //
      else                     averageIndex = 0;                              //                                  //
      configRegister &= ~INA_CONFIG_AVG_MASK;                                 // zero out the averages part       //
      configRegister |= (uint16_t)averageIndex << 9;                          // shift in the averages to register//
      writeWord(INA_CONFIGURATION_REGISTER,configRegister,ina.address);       // Save new value                   //
    } // of if this device needs to be set                                    //                                  //
  } // for-next each device loop                                              //                                  //
} // of method setAveraging()                                                 //                                  //
/*******************************************************************************************************************
** Метод setBusConversion определяет коэффициент преобразования (8 различных значений см. В таблице) для шины   **
*******************************************************************************************************************/
void INA226_Class::setBusConversion(uint8_t convTime,                         // Set timing for Bus conversions   //
                                    const uint8_t deviceNumber ) {            //                                  //
  inaDet ina;                                                                 // Hold device details in structure //
  EEPROM.get((deviceNumber%_DeviceCount)*sizeof(ina),ina);                    // Read EEPROM values               //
  int16_t configRegister;                                                     // Store configuration register     //
  for(uint8_t i=0;i<_DeviceCount;i++) {                                       // Loop for each device found       //
    if(deviceNumber==UINT8_MAX || deviceNumber%_DeviceCount==i ) {            // If this device needs setting     //
      if (convTime>7) convTime=7;                                             // Use maximum value allowed        //
      configRegister = readWord(INA_CONFIGURATION_REGISTER,ina.address);      // Get the current register         //
      configRegister &= ~INA_CONFIG_BUS_TIME_MASK;                            // zero out the Bus conversion part //
      configRegister |= (uint16_t)convTime << 6;                              // shift in the averages to register//
      writeWord(INA_CONFIGURATION_REGISTER,configRegister,ina.address);       // Save new value                   //
    } // of if this device needs to be set                                    //                                  //
  } // for-next each device loop                                              //                                  //
} // of method setBusConversion()                                             //                                  //
/*******************************************************************************************************************
** Метод setShuntConversion определяет коэффициент преобразования (8 различных значений см. В таблице) для шунта. **
*******************************************************************************************************************/
void INA226_Class::setShuntConversion(uint8_t convTime,                       // Set timing for Bus conversions   //
                                      const uint8_t deviceNumber ) {          //                                  //
  inaDet ina;                                                                 // Hold device details in structure //
  EEPROM.get((deviceNumber%_DeviceCount)*sizeof(ina),ina);                    // Read EEPROM values               //
  int16_t configRegister;                                                     // Store configuration register     //
  for(uint8_t i=0;i<_DeviceCount;i++) {                                       // Loop for each device found       //
    if(deviceNumber==UINT8_MAX || deviceNumber%_DeviceCount==i ) {            // If this device needs setting     //
      if (convTime>7) convTime=7;                                             // Use maximum value allowed        //
      configRegister = readWord(INA_CONFIGURATION_REGISTER,ina.address);      // Get the current register         //
      configRegister &= ~INA_CONFIG_SHUNT_TIME_MASK;                          // zero out the Bus conversion part //
      configRegister |= (uint16_t)convTime << 3;                              // shift in the averages to register//
      writeWord(INA_CONFIGURATION_REGISTER,configRegister,ina.address);       // Save new value                   //
    } // of if this device needs to be set                                    //                                  //
  } // for-next each device loop                                              //                                  //
} // of method setShuntConversion()                                           //                                  //
/*******************************************************************************************************************
** Метод waitForConversion зацикливается, пока текущее преобразование не будет отмечено как завершенное. 
* Если преобразование уже завершено, то флаг (и вывод прерывания, если он активирован) также сбрасывается.       **
*******************************************************************************************************************/
void INA226_Class::waitForConversion(const uint8_t deviceNumber) {            // Wait for current conversion      //
  uint16_t conversionBits = 0;                                                //                                  //
  inaDet ina;                                                                 // Hold device details in structure //
  EEPROM.get((deviceNumber%_DeviceCount)*sizeof(ina),ina);                    // Read EEPROM values               //
  for(uint8_t i=0;i<_DeviceCount;i++) {                                       // Loop for each device found       //
    if(deviceNumber==UINT8_MAX || deviceNumber%_DeviceCount==i ) {            // If this device needs setting     //
      conversionBits = 0;                                                     //                                  //
      while(conversionBits==0) {                                              //                                  //
        conversionBits = readWord(INA_MASK_ENABLE_REGISTER,ina.address)       //                                  //
                         &(uint16_t)8;                                        //                                  //
      } // of while the conversion hasn't finished                            //                                  //
    } // of if this device needs to be set                                    //                                  //
  } // for-next each device loop                                              //                                  //
} // of method waitForConversion()                                            //                                  //
/*******************************************************************************************************************
** Метод setAlertPinOnConversion настраивает INA226 на вывод сигнала ALERT на низкий уровень после завершения преобразования.    **
*******************************************************************************************************************/
void INA226_Class::setAlertPinOnConversion(const bool alertState,             // Enable pin change on conversion  //
                                           const uint8_t deviceNumber ) {     //                                  //
  inaDet ina;                                                                 // Hold device details in structure //
  EEPROM.get((deviceNumber%_DeviceCount)*sizeof(ina),ina);                    // Read EEPROM values               //
  uint16_t alertRegister;                                                     // Hold the alert register          //
  for(uint8_t i=0;i<_DeviceCount;i++) {                                       // Loop for each device found       //
    if(deviceNumber==UINT8_MAX || deviceNumber%_DeviceCount==i ) {            // If this device needs setting     //
      alertRegister = readWord(INA_MASK_ENABLE_REGISTER,ina.address);         // Get the current register         //
      if (!alertState) alertRegister &= ~((uint16_t)1<<10);                   // zero out the alert bit           //
                  else alertRegister |= (uint16_t)(1<<10);                    // turn on the alert bit            //
      writeWord(INA_MASK_ENABLE_REGISTER,alertRegister,ina.address);          // Write register back to device    //
    } // of if this device needs to be set                                    //                                  //
  } // for-next each device loop                                              //                                  //
} // of method setAlertPinOnConversion                                        //----------------------------------//
