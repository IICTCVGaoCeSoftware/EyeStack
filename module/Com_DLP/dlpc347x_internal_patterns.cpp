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
 * \brief  Implements the APIs for creating internal pattern data for the 347x
 *         controllers (DLP2010, DLP3010, and DLP4710).
 */

#include "Eyestack/Com_DLP/dlpc347x_internal_patterns.h"
#include "math.h"
#include "stdio.h"
#include "string.h"

typedef struct
{
    uint8_t  NumberOfPatterns;
    uint8_t  PatternDirection;
    uint8_t  BitDepth;
    uint8_t  Reserved;
    uint32_t PatternDataSize;
} PatternSetHeader_s;

typedef struct
{
    char     Id[4];
    uint32_t PatternSetsStart;
    uint32_t PatternSetsSize;
    uint32_t PatternOrderTableStart;
    uint32_t PatternOrderTableSize;
} PatternBlockHeader_s;

typedef struct
{
    uint8_t  PatternSetIndex;
    uint8_t  NumDisplayPatterns;
    uint8_t  IlluminationSelect;
    uint8_t  Reserved;
    uint32_t PatternInvert0;
    uint32_t PatternInvert1;
    uint32_t IlluminationTimeInMicroseconds;
    uint32_t PreIlluminationDarkTimeInMicroseconds;
    uint32_t PostIlluminationDarkTimeInMicroseconds;
} PatternOrderTableEntry_s;

typedef struct
{
    uint32_t Count;
} PatternOrderTableHeader_s;

typedef struct
{
    uint32_t Count;
} PatternSetBlockHeader_s;

typedef struct
{
    DLPC347X_INT_PAT_DMD_e DMD;
    uint32_t               Width;
    uint32_t               Height;
    uint8_t                MirrorTopOffset;
    uint8_t                MirrorBottomOffset;
    uint8_t                MirrorLeftOffset;
    uint8_t                MirrorRightOffset;
    bool                   RequiresDualController;
} DMDInfo_s;

static DMDInfo_s                                  s_DMDInfo;
static uint32_t                                   s_PatternSetCount;
static DLPC347X_INT_PAT_PatternSet_s*             s_PatternSetArray;
static uint32_t                                   s_PatternOrderTableCount;
static DLPC347X_INT_PAT_PatternOrderTableEntry_s* s_PatternOrderTable;
static DLPC347X_INT_PAT_WritePatternDataCallback  s_WritePatternDataCallback;

uint32_t SetDMDInfo(DLPC347X_INT_PAT_DMD_e DMD)
{
    s_DMDInfo.DMD = DMD;

    switch (DMD)
    {
    case DLPC347X_INT_PAT_DMD_DLP2010:
        s_DMDInfo.Width                  = DLP2010_WIDTH;
        s_DMDInfo.Height                 = DLP2010_HEIGHT;
        s_DMDInfo.MirrorTopOffset        = 32;
        s_DMDInfo.MirrorBottomOffset     = 0;
        s_DMDInfo.MirrorRightOffset      = 10;
        s_DMDInfo.MirrorLeftOffset       = 0;
        s_DMDInfo.RequiresDualController = false;
        break;

    case DLPC347X_INT_PAT_DMD_DLP3010:
        s_DMDInfo.Width                  = DLP3010_WIDTH;
        s_DMDInfo.Height                 = DLP3010_HEIGHT;
        s_DMDInfo.MirrorTopOffset        = 48;
        s_DMDInfo.MirrorBottomOffset     = 0;
        s_DMDInfo.MirrorRightOffset      = 0;
        s_DMDInfo.MirrorLeftOffset       = 0;
        s_DMDInfo.RequiresDualController = false;
        break;

    case DLPC347X_INT_PAT_DMD_DLP4710:
        s_DMDInfo.Width                  = DLP4710_WIDTH;
        s_DMDInfo.Height                 = DLP4710_HEIGHT;
        s_DMDInfo.MirrorTopOffset        = 8;
        s_DMDInfo.MirrorBottomOffset     = 0;
        s_DMDInfo.MirrorRightOffset      = 0;
        s_DMDInfo.MirrorLeftOffset       = 0;
        s_DMDInfo.RequiresDualController = true;
        break;

    default:
        return ERR_UNSUPPORTED_DMD;
    }

    return DLPC_SUCCESS;
}

void WriteZeroBytes(uint32_t Count)
{
    static uint8_t s_ZeroByte = 0;
    uint32_t       Index;

    for (Index = 0; Index < Count; Index++)
    {
        s_WritePatternDataCallback(1, &s_ZeroByte);
    }
}

void WritePixelDataRange(DLPC347X_INT_PAT_PatternSet_s*  PatternSet,
                         DLPC347X_INT_PAT_PatternData_s* PatternData,
                         uint32_t                        StartPixel,
                         uint32_t                        EndPixel)
{
    uint8_t  PixelData;
    uint8_t  PatternDataByte;
    uint32_t PatternIndex;
    uint32_t Pixel;
    uint32_t ByteIndex = 0;
    uint32_t BitIndex;
    uint32_t BitMask;
    uint32_t StartByteOffset;
    uint32_t StartBitOffset;
    uint32_t EndByteOffset;
    uint32_t EndBitOffset;
    uint32_t StartOffset;
    uint32_t EndOffset;

    if (PatternSet->Direction == DLPC347X_INT_PAT_DIRECTION_HORIZONTAL)
    {
        StartOffset = s_DMDInfo.MirrorTopOffset;
        EndOffset   = s_DMDInfo.MirrorBottomOffset;
    }
    else
    {
        StartOffset = s_DMDInfo.MirrorLeftOffset;
        EndOffset   = s_DMDInfo.MirrorRightOffset;
    }

    StartByteOffset = StartOffset / 8;
    StartBitOffset  = StartOffset % 8;
    EndByteOffset   = EndOffset / 8;
    EndBitOffset    = EndOffset % 8;

    for (PatternIndex = 0; PatternIndex < (uint32_t)PatternSet->BitDepth; PatternIndex++)
    {
        BitMask  = 1 << PatternIndex;
        BitIndex = StartBitOffset;

        ByteIndex += StartByteOffset;
        WriteZeroBytes(StartByteOffset);

        PatternDataByte = 0;
        for (Pixel = StartPixel; Pixel < EndPixel; Pixel++)
        {
            if (PatternData->PixelArrayCount > Pixel)
            {
                PixelData = (uint8_t)((PatternData->PixelArray[Pixel] & BitMask) >> PatternIndex);
                PatternDataByte |= (uint8_t)(PixelData << BitIndex);
            }

            BitIndex++;
            if (BitIndex >= 8)
            {
                ByteIndex++;
                s_WritePatternDataCallback(1, (uint8_t*)&PatternDataByte);
                PatternDataByte = 0;
                BitIndex = 0;
            }
        }

        BitIndex += StartBitOffset;
        if (BitIndex > 0)
        {
            ByteIndex++;
            s_WritePatternDataCallback(1, (uint8_t*)&PatternDataByte);
        }

        ByteIndex += EndByteOffset;
        WriteZeroBytes(EndByteOffset);

        // Align to 4-byte word boundary for the next pattern
        if (ByteIndex % 4 != 0)
        {
            ByteIndex += (4 - (ByteIndex % 4));
            WriteZeroBytes((4 - (ByteIndex % 4)));
        }
    }
}

void WritePatternData(DLPC347X_INT_PAT_PatternSet_s*  PatternSet,
                      DLPC347X_INT_PAT_PatternData_s* PatternData)
{
    uint32_t StartPixel = 0;
    uint32_t EndPixel;

    if (s_DMDInfo.RequiresDualController)
    {
        if (PatternSet->Direction == DLPC347X_INT_PAT_DIRECTION_HORIZONTAL)
        {
            EndPixel = s_DMDInfo.Height;

            // Data for master controller
            WritePixelDataRange(PatternSet, PatternData, StartPixel, EndPixel);

            // Data for slave controller (same as master for horizontal patterns)
            WritePixelDataRange(PatternSet, PatternData, StartPixel, EndPixel);
        }
        else
        {
            // Data for master controller (left half)
            EndPixel = s_DMDInfo.Width / 2;
            WritePixelDataRange(PatternSet, PatternData, StartPixel, EndPixel);

            // Data for slave controller (right half)
            StartPixel = s_DMDInfo.Width / 2;
            EndPixel   = s_DMDInfo.Width;
            WritePixelDataRange(PatternSet, PatternData, StartPixel, EndPixel);
        }
    }
    else
    {
        EndPixel = (PatternSet->Direction == DLPC347X_INT_PAT_DIRECTION_HORIZONTAL)
                 ? s_DMDInfo.Height
                 : s_DMDInfo.Width;
        WritePixelDataRange(PatternSet, PatternData, StartPixel, EndPixel);
    }
}

uint32_t GetNumOfBytesPerPatternPerController(DLPC347X_INT_PAT_PatternSet_s* PatternSet)
{
    uint32_t NumPixels;
    uint32_t NumBytesPerPattern;
    uint32_t Width;

    if (PatternSet->Direction == DLPC347X_INT_PAT_DIRECTION_HORIZONTAL)
    {
        NumPixels = s_DMDInfo.Height + s_DMDInfo.MirrorTopOffset + s_DMDInfo.MirrorBottomOffset;
    }
    else
    {
        Width     = s_DMDInfo.Width / (s_DMDInfo.RequiresDualController ? 2 : 1);
        NumPixels = Width + s_DMDInfo.MirrorLeftOffset + s_DMDInfo.MirrorRightOffset;
    }

    NumBytesPerPattern = (uint32_t)(ceil(NumPixels / 32.0) * 4);
    return NumBytesPerPattern * (uint32_t)PatternSet->BitDepth;
}

uint32_t GetPatternDataSize(DLPC347X_INT_PAT_PatternSet_s* PatternSet)
{
    uint32_t PatternSetsDataSize = (PatternSet->PatternCount
                                 * GetNumOfBytesPerPatternPerController(PatternSet)
                                 * (s_DMDInfo.RequiresDualController ? 2 : 1));
    return PatternSetsDataSize;
}

uint32_t GetPatternSetsSize()
{
    uint32_t                       PatternSetsDataSize;
    uint32_t                       PatternSetIdx;
    DLPC347X_INT_PAT_PatternSet_s* PatternSet;

    PatternSetsDataSize = sizeof(PatternSetBlockHeader_s)
                        + (sizeof(uint32_t) * s_PatternSetCount);

    for (PatternSetIdx = 0; PatternSetIdx < s_PatternSetCount; PatternSetIdx++)
    {
        PatternSet = &s_PatternSetArray[PatternSetIdx];
        PatternSetsDataSize += sizeof(PatternSetHeader_s);
        PatternSetsDataSize += GetPatternDataSize(PatternSet);
    }

    return PatternSetsDataSize;
}

uint32_t GetPatternOrderTableSize()
{
    return sizeof(PatternOrderTableHeader_s)
         + (s_PatternOrderTableCount * sizeof(PatternOrderTableEntry_s));
}

uint32_t GetPatternSetStart()
{
    return sizeof(PatternBlockHeader_s) + GetPatternOrderTableSize();
}

uint32_t GetPatternDataBlockSize()
{
    return sizeof(PatternBlockHeader_s) 
         + GetPatternOrderTableSize() 
         + GetPatternSetsSize();
}

void WritePatternBlockHeader()
{
    PatternBlockHeader_s Header;

    memcpy(Header.Id, "PATN", 4);

    Header.PatternOrderTableStart = sizeof(PatternBlockHeader_s);
    Header.PatternOrderTableSize  = GetPatternOrderTableSize();
    Header.PatternSetsStart       = GetPatternSetStart();
    Header.PatternSetsSize        = GetPatternSetsSize();
    
    s_WritePatternDataCallback(sizeof(Header), (uint8_t*)&Header);
}

void WritePatternOrderTable()
{

    uint32_t                                   PatternSetIdx;
    PatternOrderTableHeader_s                  Header;
    PatternOrderTableEntry_s                   Entry;
    DLPC347X_INT_PAT_PatternOrderTableEntry_s* Input;

    // Write pattern order table header
    Header.Count = s_PatternOrderTableCount;
    s_WritePatternDataCallback(sizeof(Header), (uint8_t*)&Header);

    // Write pattern order table entries
    for (PatternSetIdx = 0; PatternSetIdx < s_PatternOrderTableCount; PatternSetIdx++)
    {
        Input = &s_PatternOrderTable[PatternSetIdx];
        
        Entry.PatternSetIndex                        = Input->PatternSetIndex;
        Entry.NumDisplayPatterns                     = Input->NumDisplayPatterns;
        Entry.IlluminationSelect                     = (uint8_t)Input->IlluminationSelect;
        Entry.PatternInvert0                         = Input->InvertPatterns ? 0xFFFFFFFFU : 0U;
        Entry.PatternInvert1                         = Input->InvertPatterns ? 0xFFFFFFFFU : 0U;
        Entry.Reserved                               = 0;
        Entry.IlluminationTimeInMicroseconds         = Input->IlluminationTimeInMicroseconds;
        Entry.PreIlluminationDarkTimeInMicroseconds  = Input->PreIlluminationDarkTimeInMicroseconds;
        Entry.PostIlluminationDarkTimeInMicroseconds = Input->PostIlluminationDarkTimeInMicroseconds;
        
        s_WritePatternDataCallback(sizeof(PatternOrderTableEntry_s), (uint8_t*)&Entry);
    }
}

void WritePatternSets()
{
    DLPC347X_INT_PAT_PatternSet_s*  PatternSet;
    DLPC347X_INT_PAT_PatternData_s* PatternData;
    PatternSetBlockHeader_s         BlockHeader;
    PatternSetHeader_s              SetHeader;
    uint32_t                        PatternSetIdx;
    uint32_t                        PatternIdx;
    uint32_t                        PatternSetDataStart;

    BlockHeader.Count = s_PatternSetCount;
    s_WritePatternDataCallback(sizeof(PatternSetBlockHeader_s), (uint8_t*)&BlockHeader);
    
    // Write the array of start addresses of the pattern sets
    PatternSetDataStart = GetPatternSetStart()
                        + sizeof(PatternSetBlockHeader_s)
                        + (sizeof(uint32_t) * s_PatternSetCount);
    for (PatternSetIdx = 0; PatternSetIdx < s_PatternSetCount; PatternSetIdx++)
    {
        s_WritePatternDataCallback(sizeof(uint32_t), (uint8_t*)&PatternSetDataStart);

        PatternSet = &s_PatternSetArray[PatternSetIdx];
        PatternSetDataStart += sizeof(PatternSetHeader_s);
        PatternSetDataStart += GetPatternDataSize(PatternSet);
    }

    for (PatternSetIdx = 0; PatternSetIdx < s_PatternSetCount; PatternSetIdx++)
    {
        PatternSet = &s_PatternSetArray[PatternSetIdx];

        // Write pattern set header
        SetHeader.BitDepth         = (uint8_t)PatternSet->BitDepth;
        SetHeader.NumberOfPatterns = PatternSet->PatternCount;
        SetHeader.PatternDirection = (uint8_t)PatternSet->Direction;
        SetHeader.Reserved         = 0;
        SetHeader.PatternDataSize  = GetPatternDataSize(PatternSet);
        s_WritePatternDataCallback(sizeof(PatternSetHeader_s), (uint8_t*)&SetHeader);

        // Write pattern data
        for (PatternIdx = 0; PatternIdx < PatternSet->PatternCount; PatternIdx++)
        {
            PatternData = &PatternSet->PatternArray[PatternIdx];            
            WritePatternData(PatternSet, PatternData);
        }
    }
}

uint32_t DLPC347X_INT_PAT_GeneratePatternDataBlock(
    DLPC347X_INT_PAT_DMD_e                     DMD,
    uint32_t                                   PatternSetCount,
    DLPC347X_INT_PAT_PatternSet_s*             PatternSetArray,
    uint32_t                                   PatternOrderTableCount,
    DLPC347X_INT_PAT_PatternOrderTableEntry_s* PatternOrderTable,
    DLPC347X_INT_PAT_WritePatternDataCallback  WritePatternDataCallback)
{
    uint32_t Status = SetDMDInfo(DMD);
    if (Status != DLPC_SUCCESS)
    {
        return Status;
    }

    s_PatternSetCount          = PatternSetCount;
    s_PatternSetArray          = PatternSetArray;
    s_PatternOrderTableCount   = PatternOrderTableCount;
    s_PatternOrderTable        = PatternOrderTable;
    s_WritePatternDataCallback = WritePatternDataCallback;

    WritePatternBlockHeader();
    WritePatternOrderTable();
    WritePatternSets();

    return DLPC_SUCCESS;
}

uint32_t DLPC347X_INT_PAT_GetPatternDataBlockSize(
    DLPC347X_INT_PAT_DMD_e                     DMD,
    uint32_t                                   PatternSetCount,
    DLPC347X_INT_PAT_PatternSet_s*             PatternSetArray,
    uint32_t                                   PatternOrderTableCount,
    DLPC347X_INT_PAT_PatternOrderTableEntry_s* PatternOrderTable
)
{
    uint32_t Status = SetDMDInfo(DMD);
    if (Status != DLPC_SUCCESS)
    {
        return UINT32_MAX;
    }

    s_PatternSetCount        = PatternSetCount;
    s_PatternSetArray        = PatternSetArray;
    s_PatternOrderTableCount = PatternOrderTableCount;
    s_PatternOrderTable      = PatternOrderTable;

    return GetPatternDataBlockSize();
}
