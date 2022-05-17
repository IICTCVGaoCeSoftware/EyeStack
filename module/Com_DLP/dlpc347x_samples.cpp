/*------------------------------------------------------------------------------
 * Copyright (c) 2019 Texas Instruments Incorporated - http://www.ti.com/
 *------------------------------------------------------------------------------
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * \file
 * \brief  Sample code for generating internal pattern data and communicating
 *         with the DLP2010-LC and DLP3010-LC EVMs.
 */

#include "Eyestack/Com_DLP/dlpc347x_samples.hpp"
#include <iostream>

static uint8_t s_HorizontalPatternData[TOTAL_HORIZONTAL_PATTERNS][MAX_HEIGHT];
static uint8_t s_VerticalPatternData[TOTAL_VERTICAL_PATTERNS][MAX_WIDTH];
static DLPC347X_INT_PAT_PatternData_s
  s_Patterns[TOTAL_HORIZONTAL_PATTERNS + TOTAL_VERTICAL_PATTERNS];
static DLPC347X_INT_PAT_PatternSet_s s_PatternSets[NUM_PATTERN_SETS];
static DLPC347X_INT_PAT_PatternOrderTableEntry_s
  s_PatternOrderTable[NUM_PATTERN_ORDER_TABLE_ENTRIES];

static uint8_t s_WriteBuffer[MAX_WRITE_CMD_PAYLOAD];
static uint8_t s_ReadBuffer[MAX_READ_CMD_PAYLOAD];

static bool s_StartProgramming;
static uint8_t s_FlashProgramBuffer[FLASH_WRITE_BLOCK_SIZE];
static uint16_t s_FlashProgramBufferPtr;
static uint8_t HorzPatternIdx = 0;
static uint8_t VertPatternIdx = 0;
static uint8_t PatternIdx = 0;
static uint8_t PatternSetIdx = 0;
static uint32_t PatternOrderTableIdx = 0;
// uint32_t PatternSetIdx = 0;

static FILE* s_FilePointer;

/**
 * Implement the I2C write transaction here. The sample code here sends
 * data to the controller via the Cypress USB-Serial adapter.
 */
uint32_t
WriteI2C(uint16_t WriteDataLength,
         uint8_t* WriteData,
         DLPC_COMMON_CommandProtocolData_s* ProtocolData)
{
  if (!CYPRESS_I2C_WriteI2C(WriteDataLength, WriteData)) {
    return FAIL;
  }

  return SUCCESS;
}

/**
 * Implement the I2C write/read transaction here. The sample code here
 * receives data from the controller via the Cypress USB-Serial adapter.
 */
uint32_t
ReadI2C(uint16_t WriteDataLength,
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
InitConnectionAndCommandLayer()
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
WaitForSeconds(uint32_t Seconds)
{
  uint32_t retTime = (uint32_t)(time(0)) + Seconds; // Get finishing time.
  while (time(0) < retTime)
    ; // Loop until it arrives.
}
/**
 * A sample function that generates a 1-bit (binary) 1-D pattern
 * The function fills the byte array Data. Each byte in the in array corresponds
 * to a pixel. For a 1-bit pattern the value of each byte should be 1 or 0.
 */
void
PopulateOneBitPatternData(uint16_t Length, uint8_t* Data, uint16_t NumBars)
{
  uint16_t PixelPos = 0;
  uint16_t BarPos = 0;
  uint16_t BarWidth = Length / NumBars;
  uint8_t PixelData = 0;

  for (; PixelPos < Length; PixelPos++) {
    Data[PixelPos] = PixelData;

    BarPos++;
    if (BarPos >= BarWidth) {
      BarPos = 0;
      PixelData = (PixelData == 0 ? 1 : 0);
    }
  }
}

/**
 * A sample function that generates an 8-bit (gray scale) 1-D pattern
 * The function fills the byte array Data. Each byte in the in array corresponds
 * to a pixel. For an 8-bit pattern the value of each byte can be 0 - 255.
 */
void
PopulateEightBitPatternData(uint16_t Length, uint8_t* Data, uint16_t NumBars)
{
  uint16_t PixelPos = 0;
  uint16_t BarPos = 0;
  uint16_t BarWidth = Length / (2 * NumBars);
  uint8_t PixelData = 0;
  int16_t PixelDataInc = (int16_t)ceil(255.0 / BarWidth);

  for (; PixelPos < Length; PixelPos++) {
    Data[PixelPos] = PixelData;

    BarPos++;
    if (BarPos >= BarWidth) {
      BarPos = 0;
      PixelDataInc = -PixelDataInc;
    }

    PixelData = (uint8_t)(PixelData + PixelDataInc);
  }
}

/**
 * Populates an array of DLPC347X_INT_PAT_PatternSet_s
 */
void
PopulatePatternSetData(uint16_t DMDWidth, uint16_t DMDHeight)
{
  uint8_t HorzPatternIdx = 0;
  uint8_t VertPatternIdx = 0;
  uint8_t PatternIdx = 0;
  uint8_t PatternSetIdx = 0;
  uint8_t Index;
  uint16_t NumBars;
  DLPC347X_INT_PAT_PatternSet_s* PatternSet;

  /* Create a 1-bit (binary) Horizontal Pattern Set */
  PatternSet = &s_PatternSets[PatternSetIdx++];
  PatternSet->BitDepth = DLPC347X_INT_PAT_BITDEPTH_ONE;
  PatternSet->Direction = DLPC347X_INT_PAT_DIRECTION_HORIZONTAL;
  PatternSet->PatternCount = NUM_ONE_BIT_HORIZONTAL_PATTERNS;
  PatternSet->PatternArray = &s_Patterns[PatternIdx];
  for (Index = 0; Index < NUM_ONE_BIT_HORIZONTAL_PATTERNS; Index++) {
    NumBars = 2 * (Index + 1);
    PopulateOneBitPatternData(
      DMDHeight, s_HorizontalPatternData[HorzPatternIdx], NumBars);
    s_Patterns[PatternIdx].PixelArray = s_HorizontalPatternData[HorzPatternIdx];
    s_Patterns[PatternIdx].PixelArrayCount = DMDHeight;
    PatternIdx++;
    HorzPatternIdx++;
  }

  /* Create a 1-bit (binary) Vertical Pattern Set */
  PatternSet = &s_PatternSets[PatternSetIdx++];
  PatternSet->BitDepth = DLPC347X_INT_PAT_BITDEPTH_ONE;
  PatternSet->Direction = DLPC347X_INT_PAT_DIRECTION_VERTICAL;
  PatternSet->PatternCount = NUM_ONE_BIT_VERTICAL_PATTERNS;
  PatternSet->PatternArray = &s_Patterns[PatternIdx];
  for (Index = 0; Index < NUM_ONE_BIT_VERTICAL_PATTERNS; Index++) {
    NumBars = 2 * (Index + 1);
    PopulateOneBitPatternData(
      DMDWidth, s_VerticalPatternData[VertPatternIdx], NumBars);
    s_Patterns[PatternIdx].PixelArray = s_VerticalPatternData[VertPatternIdx];
    s_Patterns[PatternIdx].PixelArrayCount = DMDWidth;
    PatternIdx++;
    VertPatternIdx++;
  }

  /* Create an 8-bit (grayscale) Horizontal Pattern Set */
  PatternSet = &s_PatternSets[PatternSetIdx++];
  PatternSet->BitDepth = DLPC347X_INT_PAT_BITDEPTH_EIGHT;
  PatternSet->Direction = DLPC347X_INT_PAT_DIRECTION_HORIZONTAL;
  PatternSet->PatternCount = NUM_EIGHT_BIT_HORIZONTAL_PATTERNS;
  PatternSet->PatternArray = &s_Patterns[PatternIdx];
  for (Index = 0; Index < NUM_EIGHT_BIT_HORIZONTAL_PATTERNS; Index++) {
    NumBars = 2 * (Index + 1);
    PopulateEightBitPatternData(
      DMDHeight, s_HorizontalPatternData[HorzPatternIdx], NumBars);
    s_Patterns[PatternIdx].PixelArray = s_HorizontalPatternData[HorzPatternIdx];
    s_Patterns[PatternIdx].PixelArrayCount = DMDHeight;
    PatternIdx++;
    HorzPatternIdx++;
  }

  /* Create an 8-bit (grayscale) Vertical Pattern Set */
  PatternSet = &s_PatternSets[PatternSetIdx++];
  PatternSet->BitDepth = DLPC347X_INT_PAT_BITDEPTH_EIGHT;
  PatternSet->Direction = DLPC347X_INT_PAT_DIRECTION_VERTICAL;
  PatternSet->PatternCount = NUM_EIGHT_BIT_VERTICAL_PATTERNS;
  PatternSet->PatternArray = &s_Patterns[PatternIdx];
  for (Index = 0; Index < NUM_EIGHT_BIT_VERTICAL_PATTERNS; Index++) {
    NumBars = 2 * (Index + 1);
    PopulateEightBitPatternData(
      DMDWidth, s_VerticalPatternData[VertPatternIdx], NumBars);
    s_Patterns[PatternIdx].PixelArray = s_VerticalPatternData[VertPatternIdx];
    s_Patterns[PatternIdx].PixelArrayCount = DMDWidth;
    PatternIdx++;
    VertPatternIdx++;
  }
}

void
Populate1BitHorizonPattern(uint16_t DMDWidth,
                           uint16_t DMDHeight,
                           QString filename)
{

  DLPC347X_INT_PAT_PatternSet_s* PatternSet;
  /* Create a 1-bit (binary) Horizontal Pattern Set */
  PatternSet = &s_PatternSets[PatternSetIdx++];
  PatternSet->BitDepth = DLPC347X_INT_PAT_BITDEPTH_ONE;
  PatternSet->Direction = DLPC347X_INT_PAT_DIRECTION_HORIZONTAL;
  PatternSet->PatternCount = NUM_ONE_BIT_HORIZONTAL_PATTERNS;
  PatternSet->PatternArray = &s_Patterns[PatternIdx];

  cv::Mat image = cv::imread(filename.toStdString(), 0);
  uint8_t* array = new unsigned char[image.rows * image.cols];
  for (uint8_t i = 0; i < DMDHeight; i++) {
    s_HorizontalPatternData[HorzPatternIdx][i] = array[i];
  }
  s_Patterns[PatternIdx].PixelArray = array;
  s_Patterns[PatternIdx].PixelArrayCount = DMDHeight;

  std::cout << s_HorizontalPatternData[HorzPatternIdx][300];
  PatternIdx++;
  HorzPatternIdx++;
}

void
Populate1BitVerticalPattern(uint16_t DMDWidth,
                            uint16_t DMDHeight,
                            QString filename)
{

  DLPC347X_INT_PAT_PatternSet_s* PatternSet;
  /* Create a 1-bit (binary) Vertical Pattern Set */
  PatternSet = &s_PatternSets[PatternSetIdx++];
  PatternSet->BitDepth = DLPC347X_INT_PAT_BITDEPTH_ONE;
  PatternSet->Direction = DLPC347X_INT_PAT_DIRECTION_VERTICAL;
  PatternSet->PatternCount = NUM_ONE_BIT_VERTICAL_PATTERNS;
  PatternSet->PatternArray = &s_Patterns[PatternIdx];

  cv::Mat image = cv::imread(filename.toStdString(), 0);
  uint8_t* array = new unsigned char[image.rows * image.cols];
  for (unsigned short i = 0; i < DMDWidth; i++) {
    s_VerticalPatternData[VertPatternIdx][i] = array[i];
    //
  }
  s_Patterns[PatternIdx].PixelArray = s_VerticalPatternData[VertPatternIdx];
  s_Patterns[PatternIdx].PixelArrayCount = DMDWidth;

  qDebug() << s_VerticalPatternData[VertPatternIdx][1000];
  qDebug() << array[1000];
  PatternIdx++;
  HorzPatternIdx++;
}
/**
 * Populates an array of DLPC347X_INT_PAT_PatternOrderTableEntry_s
 */
void
PopulatePatternTableData()
{

  DLPC347X_INT_PAT_PatternOrderTableEntry_s* PatternOrderTableEntry;

  /* Pattern Table Entry 0 - uses Pattern Set 0 */
  PatternOrderTableEntry = &s_PatternOrderTable[PatternOrderTableIdx++];
  PatternOrderTableEntry->PatternSetIndex = PatternSetIdx;
  PatternOrderTableEntry->NumDisplayPatterns =
    s_PatternSets[PatternSetIdx++].PatternCount;
  PatternOrderTableEntry->IlluminationSelect =
    DLPC347X_INT_PAT_ILLUMINATION_RED;
  PatternOrderTableEntry->InvertPatterns = false;
  PatternOrderTableEntry->IlluminationTimeInMicroseconds = 5000;
  PatternOrderTableEntry->PreIlluminationDarkTimeInMicroseconds = 250;
  PatternOrderTableEntry->PostIlluminationDarkTimeInMicroseconds = 1000;

  /* Pattern Table Entry 1 - uses Pattern Set 1 */
  PatternOrderTableEntry = &s_PatternOrderTable[PatternOrderTableIdx++];
  PatternOrderTableEntry->PatternSetIndex = PatternSetIdx;
  PatternOrderTableEntry->NumDisplayPatterns =
    s_PatternSets[PatternSetIdx++].PatternCount;
  PatternOrderTableEntry->IlluminationSelect =
    DLPC347X_INT_PAT_ILLUMINATION_GREEN;
  PatternOrderTableEntry->InvertPatterns = false;
  PatternOrderTableEntry->IlluminationTimeInMicroseconds = 5000;
  PatternOrderTableEntry->PreIlluminationDarkTimeInMicroseconds = 250;
  PatternOrderTableEntry->PostIlluminationDarkTimeInMicroseconds = 1000;

  /* Pattern Table Entry 2 - uses Pattern Set 2 */
  PatternOrderTableEntry = &s_PatternOrderTable[PatternOrderTableIdx++];
  PatternOrderTableEntry->PatternSetIndex = PatternSetIdx;
  PatternOrderTableEntry->NumDisplayPatterns =
    s_PatternSets[PatternSetIdx++].PatternCount;
  PatternOrderTableEntry->IlluminationSelect =
    DLPC347X_INT_PAT_ILLUMINATION_BLUE;
  PatternOrderTableEntry->InvertPatterns = false;
  PatternOrderTableEntry->IlluminationTimeInMicroseconds = 5000;
  PatternOrderTableEntry->PreIlluminationDarkTimeInMicroseconds = 250;
  PatternOrderTableEntry->PostIlluminationDarkTimeInMicroseconds = 1000;

  /* Pattern Table Entry 3 - uses Pattern Set 3 */
  PatternOrderTableEntry = &s_PatternOrderTable[PatternOrderTableIdx++];
  PatternOrderTableEntry->PatternSetIndex = PatternSetIdx;
  PatternOrderTableEntry->NumDisplayPatterns =
    s_PatternSets[PatternSetIdx++].PatternCount;
  PatternOrderTableEntry->IlluminationSelect =
    DLPC347X_INT_PAT_ILLUMINATION_RGB;
  PatternOrderTableEntry->InvertPatterns = false;
  PatternOrderTableEntry->IlluminationTimeInMicroseconds = 11000;
  PatternOrderTableEntry->PreIlluminationDarkTimeInMicroseconds = 250;
  PatternOrderTableEntry->PostIlluminationDarkTimeInMicroseconds = 1000;
}

void
PopulatePatternTableData_Demo()
{
  DLPC347X_INT_PAT_PatternOrderTableEntry_s* PatternOrderTableEntry;

  /* Pattern Table Entry 0 - uses Pattern Set 0 */
  PatternOrderTableEntry = &s_PatternOrderTable[PatternOrderTableIdx++];
  PatternOrderTableEntry->PatternSetIndex = PatternSetIdx;
  PatternOrderTableEntry->NumDisplayPatterns =
    s_PatternSets[PatternSetIdx++].PatternCount;
  PatternOrderTableEntry->IlluminationSelect =
    DLPC347X_INT_PAT_ILLUMINATION_RED;
  PatternOrderTableEntry->InvertPatterns = false;
  PatternOrderTableEntry->IlluminationTimeInMicroseconds = 5000;
  PatternOrderTableEntry->PreIlluminationDarkTimeInMicroseconds = 250;
  PatternOrderTableEntry->PostIlluminationDarkTimeInMicroseconds = 1000;
}

void
CopyDataToFlashProgramBuffer(uint8_t* Length, uint8_t** DataPtr)
{
  while ((*Length >= 1) &&
         (s_FlashProgramBufferPtr < sizeof(s_FlashProgramBuffer))) {
    s_FlashProgramBuffer[s_FlashProgramBufferPtr] = **DataPtr;
    s_FlashProgramBufferPtr++;
    (*DataPtr)++;
    (*Length)--;
  }
}

void
ProgramFlashWithDataInBuffer(uint16_t Length)
{
  s_FlashProgramBufferPtr = 0;

  if (s_StartProgramming) {
    s_StartProgramming = false;
    DLPC347X_WriteFlashStart(Length, s_FlashProgramBuffer);
  } else {
    DLPC347X_WriteFlashContinue(Length, s_FlashProgramBuffer);
  }
}

void
WriteDataToFile(uint8_t Length, uint8_t* Data)
{
  fwrite(Data, 1, Length, s_FilePointer);
}

void
GenerateAndWritePatternDataToFile(DLPC347X_INT_PAT_DMD_e DMD, char* FilePath)
{
  s_FilePointer = fopen(FilePath, "wb");

  /* Generate pattern data and write it to the flash.
   * The DLPC347X_INT_PAT_GeneratePatternDataBlock() function will call the
   * WriteDataToFile() function several times while it packs sections of the
   * pattern data.
   */
  DLPC347X_INT_PAT_GeneratePatternDataBlock(DMD,
                                            NUM_PATTERN_SETS,
                                            s_PatternSets,
                                            NUM_PATTERN_ORDER_TABLE_ENTRIES,
                                            s_PatternOrderTable,
                                            WriteDataToFile);

  fclose(s_FilePointer);
}

void
BufferPatternDataAndProgramToFlash(uint8_t Length, uint8_t* Data)
{
  /* Copy data that can fit in the flash programming buffer */
  CopyDataToFlashProgramBuffer(&Length, &Data);

  /* Write data to flash if the buffer is full */
  if (s_FlashProgramBufferPtr >= sizeof(s_FlashProgramBuffer)) {
    ProgramFlashWithDataInBuffer((uint16_t)sizeof(s_FlashProgramBuffer));
  }

  /* Copy remaining data (if any) to the flash programming buffer */
  CopyDataToFlashProgramBuffer(&Length, &Data);
}

void
GenerateAndProgramPatternData(DLPC347X_INT_PAT_DMD_e DMD)
{
  s_StartProgramming = true;
  s_FlashProgramBufferPtr = 0;

  /* Let the controller know that we're going to program pattern data */
  DLPC347X_WriteFlashDataTypeSelect(DLPC347X_FDTS_ENTIRE_SENS_PATTERN_DATA);

  /* Erase the flash sectors that store pattern data */
  DLPC347X_WriteFlashErase();

  /* Read Short Status to make sure Erase is completed */
  DLPC347X_ShortStatus_s ShortStatus;
  do {
    DLPC347X_ReadShortStatus(&ShortStatus);
  } while (ShortStatus.FlashEraseComplete == DLPC347X_FE_NOT_COMPLETE);

  /* To program the flash, send blocks of data of up to 1024 bytes
   * to the controller at a time. Repeat the process until the entire
   * data is programmed to the flash.
   * Let the controller know the size of a data block that will be
   * transferred at a time.
   */
  DLPC347X_WriteFlashDataLength(sizeof(s_FlashProgramBuffer));

  /* Generate pattern data and program it to the flash.
   *
   * The DLPC347X_INT_PAT_GeneratePatternDataBlock() function calls the
   * BufferPatternDataAndProgramToFlash() function several times while it
   * generates pattern data.
   *
   * The BufferPatternDataAndProgramToFlash() function buffers data received,
   * programming the buffer content only when it is full. This is done in an
   * effort to make flash writes more efficient, overall greatly reducing the
   * time it takes to program the pattern data.
   *
   * After returning from the DLPC347X_INT_PAT_GeneratePatternBlock() function,
   * check if there is any data left in the buffer and program it. This needs
   * to be done since the BufferPatternDataAndProgramToFlash() function only
   * programs the buffer content if full.
   */
  DLPC347X_INT_PAT_GeneratePatternDataBlock(DMD,
                                            NUM_PATTERN_SETS,
                                            s_PatternSets,
                                            NUM_PATTERN_ORDER_TABLE_ENTRIES,
                                            s_PatternOrderTable,
                                            BufferPatternDataAndProgramToFlash);
  if (s_FlashProgramBufferPtr > 0) {
    /* Resend the block size since it could be less than
     * the previously specified size
     */
    DLPC347X_WriteFlashDataLength(s_FlashProgramBufferPtr);

    ProgramFlashWithDataInBuffer(s_FlashProgramBufferPtr);
  }
}

void
LoadPatternOrderTableEntryfromFlash()
{
  DLPC347X_PatternOrderTableEntry_s PatternOrderTableEntry;

  /* Reload from Flash */
  DLPC347X_WritePatternOrderTableEntry(DLPC347X_WC_RELOAD_FROM_FLASH,
                                       &PatternOrderTableEntry);
}

void
LoadPatternOrderTableEntry(uint8_t PatternSetIndex)
{
  DLPC347X_PatternOrderTableEntry_s PatternOrderTableEntry;

  /* Set PatternOrderTableEntry to select specific Pattern Set and configure
   * settings */
  PatternOrderTableEntry.PatSetIndex = 1;
  PatternOrderTableEntry.NumberOfPatternsToDisplay = PatternSetIndex;
  PatternOrderTableEntry.RedIlluminator = DLPC347X_IE_DISABLE;
  PatternOrderTableEntry.GreenIlluminator = DLPC347X_IE_DISABLE;
  PatternOrderTableEntry.BlueIlluminator = DLPC347X_IE_ENABLE;
  PatternOrderTableEntry.PatternInvertLsword = 0;
  PatternOrderTableEntry.PatternInvertMsword = 0;
  PatternOrderTableEntry.IlluminationTime = 2000;
  PatternOrderTableEntry.PreIlluminationDarkTime = 250;
  PatternOrderTableEntry.PostIlluminationDarkTime = 60;
  DLPC347X_WritePatternOrderTableEntry(DLPC347X_WC_START,
                                       &PatternOrderTableEntry);
}

void
WriteTestPatternGridLines()
{
  /* Write Input Image Size */
  DLPC347X_WriteInputImageSize(DLP3010_WIDTH, DLP3010_HEIGHT);

  /* Write Image Crop */
  DLPC347X_WriteImageCrop(0, 0, DLP3010_WIDTH, DLP3010_HEIGHT);

  /* Write Display Size */
  DLPC347X_WriteDisplaySize(0, 0, DLP3010_WIDTH, DLP3010_HEIGHT);

  /* Write Grid Lines */
  DLPC347X_GridLines_s GridLines;
  GridLines.Border = DLPC347X_BE_ENABLE;
  GridLines.BackgroundColor = DLPC347X_C_GREEN;
  GridLines.ForegroundColor = DLPC347X_C_MAGENTA;
  GridLines.HorizontalForegroundLineWidth = 0xF;
  GridLines.HorizontalBackgroundLineWidth = 0xF;
  GridLines.VerticalForegroundLineWidth = 0xF;
  GridLines.VerticalBackgroundLineWidth = 0xF;
  DLPC347X_WriteGridLines(&GridLines);
  DLPC347X_WriteOperatingModeSelect(DLPC347X_OM_TEST_PATTERN_GENERATOR);
  WaitForSeconds(5);
}

// void
// WriteTestPatternColorBar()
//{
//   /* Write Input Image Size */
//   DLPC347X_WriteInputImageSize(MAX_WIDTH, MAX_HEIGHT);

//  /* Write Image Crop */
//  DLPC347X_WriteImageCrop(0, 0, MAX_WIDTH, MAX_HEIGHT);

//  /* Write Display Size */
//  DLPC347X_WriteDisplaySize(0, 0, MAX_WIDTH, MAX_HEIGHT);

//  /* Write Color Bar & Select Test Pattern */
//  DLPC34XX_WriteColorbars(DLPC34XX_BE_ENABLE);

//  DLPC34XX_WriteOperatingModeSelect(DLPC34XX_OM_TEST_PATTERN_GENERATOR);
//  WaitForSeconds(10);
//}

void
WriteLookSelect(uint8_t LookNumber)
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
LoadPreBuildPatternData()
{
  /* write up to 1024 bytes of data */
  uint8_t PatternDataArray[1024];

  /* Pattern File assumes to be in the \build\vs2017\dlpc347x folder */
  s_FilePointer = fopen("pattern_data_gui.bin", "rb");
  if (!s_FilePointer) {
    printf("Error opening the binary file!");
    return;
  }
  fseek(s_FilePointer, 0, SEEK_END);
  uint32_t PatternDataSize = ftell(s_FilePointer);
  fseek(s_FilePointer, 0, SEEK_SET);

  /* Select Flash Data Block and Erase the Block */
  DLPC347X_WriteFlashDataTypeSelect(DLPC347X_FDTS_ENTIRE_SENS_PATTERN_DATA);
  DLPC347X_WriteFlashErase();

  /* Read Short Status to make sure Erase is completed */
  DLPC347X_ShortStatus_s ShortStatus;
  do {
    DLPC347X_ReadShortStatus(&ShortStatus);
  } while (ShortStatus.FlashEraseComplete == DLPC347X_FE_NOT_COMPLETE);

  DLPC347X_WriteFlashDataLength(1024);
  fread(PatternDataArray, sizeof(PatternDataArray), 1, s_FilePointer);
  DLPC347X_WriteFlashStart(1024, PatternDataArray);

  int32_t BytesLeft = PatternDataSize - 1024;
  do {
    fread(PatternDataArray, sizeof(PatternDataArray), 1, s_FilePointer);
    DLPC347X_WriteFlashContinue(1024, PatternDataArray);

    BytesLeft = BytesLeft - 1024;
  } while (BytesLeft > 0);

  fclose(s_FilePointer);
}

void
setExposureTime(unsigned int Ill, unsigned int preIll, unsigned int postIll)
{
  DLPC347X_INT_PAT_PatternOrderTableEntry_s* PatternOrderTableEntry;

  /* Pattern Table Entry 0 - uses Pattern Set 0 */
  PatternOrderTableEntry = &s_PatternOrderTable[PatternOrderTableIdx++];
  PatternOrderTableEntry->PatternSetIndex = PatternSetIdx;
  PatternOrderTableEntry->NumDisplayPatterns =
    s_PatternSets[PatternSetIdx++].PatternCount;
  PatternOrderTableEntry->IlluminationSelect =
    DLPC347X_INT_PAT_ILLUMINATION_RED;
  PatternOrderTableEntry->InvertPatterns = false;
  PatternOrderTableEntry->IlluminationTimeInMicroseconds = Ill;
  PatternOrderTableEntry->PreIlluminationDarkTimeInMicroseconds = preIll;
  PatternOrderTableEntry->PostIlluminationDarkTimeInMicroseconds = postIll;
}
