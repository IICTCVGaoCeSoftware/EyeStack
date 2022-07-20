#pragma once

#include "cypress_i2c.hpp"
#include "dlpc347x.hpp"
#include "dlpc347x_internal_patterns.hpp"
#include "dlpc_common.hpp"

#define SUCCESS 0
#define FAIL 1
#define FLASH_WRITE_BLOCK_SIZE 1024
#define FLASH_READ_BLOCK_SIZE 256
#define MAX_WRITE_CMD_PAYLOAD (FLASH_WRITE_BLOCK_SIZE + 8)
#define MAX_READ_CMD_PAYLOAD (FLASH_READ_BLOCK_SIZE + 8)
static uint8_t s_WriteBuffer[MAX_WRITE_CMD_PAYLOAD];
static uint8_t s_ReadBuffer[MAX_READ_CMD_PAYLOAD];

namespace Eyestack::Com_DLP {
class EYESTACK_COM_DLP_EXPORT DlpcWrapper final
{
  using _T = DlpcWrapper;

  DlpcWrapper(const DlpcWrapper& other) = delete;
  DlpcWrapper(DlpcWrapper&& other) = delete;

public:
  /**
   * @brief 共享指针类别名
   */
  using Shared = QSharedPointer<DlpcWrapper>;

public:
  DlpcWrapper() = default;
  ~DlpcWrapper() noexcept;

public:
  void initDlpc();
  static uint32_t WriteI2C(uint16_t WriteDataLength,
                           uint8_t* WriteData,
                           DLPC_COMMON_CommandProtocolData_s* ProtocolData);

  static uint32_t ReadI2C(uint16_t WriteDataLength,
                          uint8_t* WriteData,
                          uint16_t ReadDataLength,
                          uint8_t* ReadData,
                          DLPC_COMMON_CommandProtocolData_s* ProtocolData);

  void InitConnectionAndCommandLayer();

  void WaitForSeconds(uint32_t Seconds);

  void WriteLookSelect(uint8_t LookNumber);

  void WriteTestPatternChessBoard(unsigned short HorizontalCheckerCount,
                                  unsigned short VerticalCheckerCount);

  void RunOnce();

  void Stop();
};
}
