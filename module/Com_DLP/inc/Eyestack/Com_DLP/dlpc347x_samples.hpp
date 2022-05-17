#pragma once

#include "cypress_i2c.hpp"
#include "dlpc347x.hpp"
#include "dlpc347x_internal_patterns.hpp"
#include "dlpc_common.hpp"
#include "math.h"
#include "stdio.h"
#include "time.h"

#define SUCCESS                           0
#define FAIL                              1

#define MAX_WIDTH                         DLP3010_WIDTH
#define MAX_HEIGHT                        DLP3010_HEIGHT

#define NUM_PATTERN_SETS                  4
#define NUM_PATTERN_ORDER_TABLE_ENTRIES   4
#define NUM_ONE_BIT_HORIZONTAL_PATTERNS   4
#define NUM_EIGHT_BIT_HORIZONTAL_PATTERNS 4
#define NUM_ONE_BIT_VERTICAL_PATTERNS     4
#define NUM_EIGHT_BIT_VERTICAL_PATTERNS   4
#define TOTAL_HORIZONTAL_PATTERNS         (NUM_ONE_BIT_HORIZONTAL_PATTERNS + NUM_EIGHT_BIT_HORIZONTAL_PATTERNS)
#define TOTAL_VERTICAL_PATTERNS           (NUM_ONE_BIT_VERTICAL_PATTERNS + NUM_EIGHT_BIT_VERTICAL_PATTERNS)

#define FLASH_WRITE_BLOCK_SIZE            1024
#define FLASH_READ_BLOCK_SIZE             256

#define MAX_WRITE_CMD_PAYLOAD             (FLASH_WRITE_BLOCK_SIZE + 8)
#define MAX_READ_CMD_PAYLOAD (FLASH_READ_BLOCK_SIZE + 8)

uint32_t
WriteI2C(uint16_t WriteDataLength,
         uint8_t* WriteData,
         DLPC_COMMON_CommandProtocolData_s* ProtocolData);

uint32_t
ReadI2C(uint16_t WriteDataLength,
        uint8_t* WriteData,
        uint16_t ReadDataLength,
        uint8_t* ReadData,
        DLPC_COMMON_CommandProtocolData_s* ProtocolData);

void InitConnectionAndCommandLayer();
void WaitForSeconds(uint32_t Seconds);
void PopulateOneBitPatternData(uint16_t Length, uint8_t* Data, uint16_t NumBars);
void PopulateEightBitPatternData(uint16_t Length, uint8_t* Data, uint16_t NumBars);
void PopulatePatternSetData(uint16_t DMDWidth, uint16_t DMDHeight);
void
Populate1BitHorizonPattern(uint16_t DMDWidth,
                           uint16_t DMDHeight,
                           QString filename);
void
Populate1BitVerticalPattern(uint16_t DMDWidth,
                            uint16_t DMDHeight,
                            QString filename);

void PopulatePatternTableData();
void
PopulatePatternTableData_Demo();
void CopyDataToFlashProgramBuffer(uint8_t* Length, uint8_t** DataPtr);
void ProgramFlashWithDataInBuffer(uint16_t Length);
void WriteDataToFile(uint8_t Length, uint8_t* Data);
void GenerateAndWritePatternDataToFile(DLPC347X_INT_PAT_DMD_e DMD, char* FilePath);
void BufferPatternDataAndProgramToFlash(uint8_t Length, uint8_t* Data);
void GenerateAndProgramPatternData(DLPC347X_INT_PAT_DMD_e DMD);
void LoadPatternOrderTableEntryfromFlash();
void LoadPatternOrderTableEntry(uint8_t PatternSetIndex);
void WriteTestPatternGridLines();
void WriteLookSelect(uint8_t LookNumber);
void LoadPreBuildPatternData();


