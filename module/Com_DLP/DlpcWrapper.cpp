#include "Eyestack/Com_DLP/DlpcWrapper.hpp"

namespace Eyestack::Com_DLP {

void
DlpcWrapper::initDlpc()
{
  InitConnectionAndCommandLayer();
  CYPRESS_I2C_RequestI2CBusAccess();
  /* ***** Change to look 0 ***** */
  WriteLookSelect(0);
}

uint32_t
DlpcWrapper::WriteI2C(uint16_t WriteDataLength,
                      uint8_t* WriteData,
                      DLPC_COMMON_CommandProtocolData_s* ProtocolData)
{
  if (!CYPRESS_I2C_WriteI2C(WriteDataLength, WriteData)) {
    return FAIL;
  }

  return SUCCESS;
}

uint32_t
DlpcWrapper::ReadI2C(uint16_t WriteDataLength,
                     uint8_t* WriteData,
                     uint16_t ReadDataLength,
                     uint8_t* ReadData,
                     DLPC_COMMON_CommandProtocolData_s* ProtocolData)
{
  if (!CYPRESS_I2C_WriteI2C(WriteDataLength, WriteData)) {
    return FAIL;
  }

  if (!CYPRESS_I2C_ReadI2C(ReadDataLength, ReadData)) {
    return FAIL;
  }

  return SUCCESS;
}

/**
 * Initialize the command layer by setting up the read/write buffers and
 * callbacks.
 */
void
DlpcWrapper::InitConnectionAndCommandLayer()
{
  DLPC_COMMON_InitCommandLibrary(s_WriteBuffer,
                                 sizeof(s_WriteBuffer),
                                 s_ReadBuffer,
                                 sizeof(s_ReadBuffer),
                                 WriteI2C,
                                 ReadI2C);

  CYPRESS_I2C_ConnectToCyI2C();
}

void
DlpcWrapper::WaitForSeconds(uint32_t Seconds)
{
  uint32_t retTime = (uint32_t)(time(0)) + Seconds; // Get finishing time.
  while (time(0) < retTime)
    ; // Loop until it arrives.
}

void
DlpcWrapper::WriteLookSelect(uint8_t LookNumber)
{
  /* Read Current Operating Mode Selected */
  DLPC347X_OperatingMode_e OperatingMode;
  DLPC347X_ReadOperatingModeSelect(&OperatingMode);

  /* Write RGB LED Current (based on Flash data) */
  DLPC347X_WriteRgbLedCurrent(0x03E8, 0x03E8, 0x03E8);

  /* Select Look */
  DLPC347X_WriteLookSelect(LookNumber);

  /* Submit Write Splash Screen Execute if in Splash Mode */
  if ((OperatingMode == DLPC347X_OM_SPLASH_SCREEN) ||
      (OperatingMode == DLPC347X_OM_SENS_SPACE_CODED_PATTERN)) {
    DLPC347X_WriteSplashScreenExecute();
  }
  WaitForSeconds(5);
}

void
DlpcWrapper::WriteTestPatternChessBoard(unsigned short HorizontalCheckerCount,
                                        unsigned short VerticalCheckerCount)
{
  /* Write Input Image Size */
  DLPC347X_WriteInputImageSize(DLP3010_WIDTH, DLP3010_HEIGHT);

  /* Write Image Crop */
  DLPC347X_WriteImageCrop(0, 0, DLP3010_WIDTH, DLP3010_HEIGHT);

  /* Write Display Size */
  DLPC347X_WriteDisplaySize(0, 0, DLP3010_WIDTH, DLP3010_HEIGHT);

  /* Write Grid Lines */
  DLPC347X_Checkerboard_s GridLines;
  GridLines.Border = DLPC347X_BE_DISABLE;
  GridLines.BackgroundColor = DLPC347X_C_BLACK;
  GridLines.ForegroundColor = DLPC347X_C_BLUE;
  GridLines.HorizontalCheckerCount = HorizontalCheckerCount;
  GridLines.VerticalCheckerCount = VerticalCheckerCount;
  DLPC347X_WriteCheckerboard(&GridLines);
  DLPC347X_WriteOperatingModeSelect(DLPC347X_OM_TEST_PATTERN_GENERATOR);
  WaitForSeconds(5);
}

void
DlpcWrapper::RunOnce()
{
  DLPC347X_WriteOperatingModeSelect(DLPC347X_OM_SENS_INTERNAL_PATTERN);
  DLPC347X_WriteInternalPatternControl(DLPC347X_PC_START, 0x1);
}

void
DlpcWrapper::Stop()
{
  DLPC347X_WriteInternalPatternControl(DLPC347X_PC_STOP, 0);
}

}
