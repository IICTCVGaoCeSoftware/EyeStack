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

void
InitConnectionAndCommandLayer();

void
WaitForSeconds(uint32_t Seconds);

/**
 * @brief 生成1位水平PatternSet
 */
void
Populate1BitHorizonPatternSet(uint16_t DMDWidth, uint16_t DMDHeight);

/**
 * @brief 生成8位水平PatternSet
 */
void
Populate8BitHorizonPatternSet(uint16_t DMDWidth, uint16_t DMDHeight);

/**
 * @brief 生成1位垂直PatternSet
 */
void
Populate1BitVerticalPatternSet(uint16_t DMDWidth, uint16_t DMDHeight);

/**
 * @brief 生成8位垂直PatternSet
 */
void
Populate8BitVerticalPatternSet(uint16_t DMDWidth, uint16_t DMDHeight);

/**
 * @brief 删除PatternSet
 */
void
DeletePatternSet(int i);

void
Populate1BitHorizonPattern(uint16_t DMDWidth,
                           uint16_t DMDHeight,
                           QString filename);
void
Populate1BitVerticalPattern(uint16_t DMDWidth,
                            uint16_t DMDHeight,
                            QString filename);

/**
 * @brief 1位水平集的配置
 */
void
Populate1BitHorizonPatternEntry(QString sel = "R",
                                unsigned int ill = 5000,
                                unsigned int pre = 250,
                                unsigned int post = 1000);
/**
 * @brief 8位水平集的配置
 */
void
Populate8BitHorizonPatternEntry(QString sel = "R",
                                unsigned int ill = 5000,
                                unsigned int pre = 250,
                                unsigned int post = 1000);
/**
 * @brief 1位垂直集的配置
 */
void
Populate1BitVerticalPatternEntry(QString sel = "R",
                                 unsigned int ill = 5000,
                                 unsigned int pre = 250,
                                 unsigned int post = 1000);
/**
 * @brief 8位垂直集的配置
 */
void
Populate8BitVerticalPatternEntry(QString sel = "R",
                                 unsigned int ill = 5000,
                                 unsigned int pre = 250,
                                 unsigned int post = 1000);

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

void
MergeH1PatternData(uint8_t seq);
void
MergeV1PatternData(uint8_t seq);
void
MergeH8PatternData(uint8_t seq);
void
MergeV8PatternData(uint8_t seq);
