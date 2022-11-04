/* 
 * File:   eMB_Utils.c
 * Author: Long
 * 
 * Created on September 15, 2019, 11:06 AM
 */

#ifdef __cplusplus
extern "C" {
#endif



/*===============================================================================================
*                                         INCLUDE FILES
* 1) system and project includes
* 2) needed interfaces from external units
* 3) internal and external interfaces from this unit
===============================================================================================*/

#include "eMB.h"
#include "eMB_Utils.h"



/*===============================================================================================
*                                       DEFINES AND MACROS
===============================================================================================*/

#define BITS_UCHAR                                8U

#define M_DISCRETE_INPUT_START                    0
#define M_DISCRETE_INPUT_NDISCRETES               16
#define M_COIL_START                              0
#define M_COIL_NCOILS                             64
#define M_REG_INPUT_START                         0
#define M_REG_INPUT_NREGS                         100
#define M_REG_HOLDING_START                       0
#define M_REG_HOLDING_NREGS                       100

/* master mode: holding register's all address */
#define M_HD_RESERVE                              0
/* master mode: input register's all address */
#define M_IN_RESERVE                              0
/* master mode: coil's all address */
#define M_CO_RESERVE                              0
/* master mode: discrete's all address */
#define M_DI_RESERVE                              0



/*===============================================================================================
*                                          VARIABLES
===============================================================================================*/

static eMB_ErrorEventType eMB_ErrorEvent;

/* Discrete inputs variables */
uint16_t usMDiscInStart = M_DISCRETE_INPUT_START;

#if (M_DISCRETE_INPUT_NDISCRETES % 8)
uint8_t ucMDiscInBuf[eMB_MASTER_TOTAL_SLAVE_NUM][M_DISCRETE_INPUT_NDISCRETES/8+1];
#else
uint8_t ucMDiscInBuf[eMB_MASTER_TOTAL_SLAVE_NUM][M_DISCRETE_INPUT_NDISCRETES/8];
#endif

/* Coils variables */
uint16_t usMCoilStart = M_COIL_START;
#if (M_COIL_NCOILS % 8)
uint8_t ucMCoilBuf[eMB_MASTER_TOTAL_SLAVE_NUM][M_COIL_NCOILS/8+1];
#else
uint8_t ucMCoilBuf[eMB_MASTER_TOTAL_SLAVE_NUM][M_COIL_NCOILS/8];
#endif

/* Input Register variables */
uint16_t usMRegInStart = M_REG_INPUT_START;
uint16_t usMRegInBuf[eMB_MASTER_TOTAL_SLAVE_NUM][M_REG_INPUT_NREGS];

/* Holding Register variables */
uint16_t usMRegHoldStart = M_REG_HOLDING_START;
uint16_t usMRegHoldBuf[eMB_MASTER_TOTAL_SLAVE_NUM][M_REG_HOLDING_NREGS];



/*===============================================================================================
*                                   FUNCTIONS PROTOTYPES
===============================================================================================*/

/*! \brief Function to set bits in a byte buffer.
 *
 * This function allows the efficient use of an array to implement bitfields.
 * The array used for storing the bits must always be a multiple of two
 * bytes. Up to eight bits can be set or cleared in one operation.
 *
 * \param byteArr A buffer where the bit values are stored. Must be a
 *   multiple of 2 bytes. No length checking is performed and if
 *   offset / 8 is greater than the size of the buffer memory contents
 *   is overwritten.
 * \param offset The starting address of the bits to set. The first
 *   bit has the offset 0.
 * \param bitNum Number of bits to modify. The value must always be smaller
 *   than 8.
 * \param value Thew new values for the bits. The value for the first bit
 *   starting at <code>offset</code> is the LSB of the value
 *   <code>value</code>
 *
 * \code
 * ucBits[2] = {0, 0};
 *
 * // Set bit 4 to 1 (read: set 1 bit starting at bit offset 4 to value 1)
 * eMB_Util_SetBits( ucBits, 4, 1, 1 );
 *
 * // Set bit 7 to 1 and bit 8 to 0.
 * eMB_Util_SetBits( ucBits, 7, 2, 0x01 );
 *
 * // Set bits 8 - 11 to 0x05 and bits 12 - 15 to 0x0A;
 * eMB_Util_SetBits( ucBits, 8, 8, 0x5A);
 * \endcode
 */
void eMB_Util_SetBits(uint8_t * byteArr, uint16_t offset, uint8_t bitNum, uint8_t value);



/*! \brief Function to read bits in a byte buffer.
 *
 * This function is used to extract up bit values from an array. Up to eight
 * bit values can be extracted in one step.
 *
 * \param byteArr A buffer where the bit values are stored.
 * \param offset The starting address of the bits to set. The first
 *   bit has the offset 0.
 * \param bitNum Number of bits to modify. The value must always be smaller
 *   than 8.
 *
 * \code
 * uint8_t ucBits[2] = {0, 0};
 * uint8_t ucResult;
 *
 * // Extract the bits 3 - 10.
 * ucResult = eMB_Util_GetBits( ucBits, 3, 8 );
 * \endcode
 */
uint8_t eMB_Util_GetBits(uint8_t * byteArr, uint16_t offset, uint8_t bitNum);



/*===============================================================================================
*                                   FUNCTIONS IMPLEMENTATIONS
===============================================================================================*/

void eMB_Util_SetBits(uint8_t *byteArr, uint16_t offset, uint8_t bitNum, uint8_t value)
{
  uint16_t usWordBuf;
  uint16_t usMask;
  uint16_t usByteOffset;
  uint16_t usNPreBits;
  uint16_t usValue = value;

  /* Calculate byte offset for first byte containing the bit values starting at offset. */
  usByteOffset = (uint16_t)(offset / BITS_UCHAR);

  /* How many bits precede our bits to set. */
  usNPreBits = (uint16_t)(offset - usByteOffset * BITS_UCHAR);

  /* Move bit field into position over bits to set */
  usValue <<= usNPreBits;

  /* Prepare a mask for setting the new bits. */
  usMask = (uint16_t)((1 << (uint16_t)bitNum) - 1);
  usMask <<= offset - usByteOffset * BITS_UCHAR;

  /* copy bits into temporary storage. */
  usWordBuf = byteArr[usByteOffset];
  usWordBuf |= byteArr[usByteOffset + 1] << BITS_UCHAR;

  /* Zero out bit field bits and then or value bits into them. */
  usWordBuf = (uint16_t)((usWordBuf & (~usMask)) | usValue);

  /* move bits back into storage */
  byteArr[usByteOffset] = (uint8_t)(usWordBuf & 0xFF);
  byteArr[usByteOffset + 1] = (uint8_t)(usWordBuf >> BITS_UCHAR);
}

uint8_t eMB_Util_GetBits(uint8_t *byteArr, uint16_t offset, uint8_t bitNum)
{
  uint16_t usWordBuf;
  uint16_t usMask;
  uint16_t usByteOffset;
  uint16_t usNPreBits;

  /* Calculate byte offset for first byte containing the bit values starting at offset. */
  usByteOffset = (uint16_t)((offset) / BITS_UCHAR);

  /* How many bits precede our bits to set. */
  usNPreBits = (uint16_t)(offset - usByteOffset * BITS_UCHAR);

  /* Prepare a mask for setting the new bits. */
  usMask = (uint16_t)((1 << (uint16_t) bitNum) - 1);

  /* copy bits into temporary storage. */
  usWordBuf = byteArr[usByteOffset];
  usWordBuf |= byteArr[usByteOffset + 1] << BITS_UCHAR;

  /* throw away unneeded bits. */
  usWordBuf >>= usNPreBits;

  /* mask away bits above the requested bitfield. */
  usWordBuf &= usMask;

  return (uint8_t)usWordBuf;
}



eMB_ExceptionType eMB_Util_ErrorToException(eMB_ErrorCodeType errorCode)
{
  eMB_ExceptionType exptStatus;

  switch (errorCode)
  {
    case eMB_ENOERR:
      exptStatus = eMB_EX_NONE;
      break;

    case eMB_ENOREG:
      exptStatus = eMB_EX_ILLEGAL_DATA_ADDRESS;
      break;

    case eMB_ETIMEDOUT:
      exptStatus = eMB_EX_SLAVE_BUSY;
      break;

    default:
      exptStatus = eMB_EX_SLAVE_DEVICE_FAILURE;
      break;
  }

  return exptStatus;
}



/**
 * Modbus read coils callback function.
 *
 * @param usAddress coils address
 * @param usNCoils coils number
 * @param pucRegBuffer coils buffer
 *
 * @return result
 */
eMB_ErrorCodeType eMB_Util_FuncCoilsCallback
(
  uint16_t usAddress,
  uint16_t usNCoils,
  uint8_t *pucRegBuffer
)
{
  eMB_ErrorCodeType errStatus = eMB_ENOERR;
  uint16_t iRegIndex, iRegBitIndex, iNReg;
  uint8_t *pucCoilBuf;
  uint16_t usCoilStart;

  iNReg =  usNCoils / 8 + 1;

  pucCoilBuf = ucMCoilBuf[eMB_FrameGetSlaveAddressCalloutArr() - 1];
  usCoilStart = usMCoilStart;

  /* it already plus one in modbus function method. */
  usAddress--;

  if ((usAddress >= M_COIL_START) &&
      (usAddress + usNCoils <= M_COIL_START + M_COIL_NCOILS))
  {
    iRegIndex = (uint16_t)(usAddress - usCoilStart) / 8U;
    iRegBitIndex = (uint16_t)(usAddress - usCoilStart) % 8U;

    while (iNReg > 1)
    {
      eMB_Util_SetBits(&pucCoilBuf[iRegIndex++], iRegBitIndex, 8, *pucRegBuffer++);
      iNReg--;
    }

    /* last coils */
    usNCoils = usNCoils % 8;

    /* eMB_Util_SetBits has bug when ucNBits is zero */
    if (usNCoils != 0)
    {
      eMB_Util_SetBits(&pucCoilBuf[iRegIndex++], iRegBitIndex, usNCoils, *pucRegBuffer++);
    }
  }
  else
  {
    errStatus = eMB_ENOREG;
  }

  return errStatus;
}

/**
 * Modbus discrete callback function.
 *
 * @param pucRegBuffer discrete buffer
 * @param usAddress discrete address
 * @param usNDiscrete discrete number
 *
 * @return result
 */
eMB_ErrorCodeType eMB_Util_FuncDiscreteInputsCallback
(
  uint16_t usAddress,
  uint16_t usNDiscrete,
  uint8_t *pucRegBuffer
)
{
  eMB_ErrorCodeType errStatus = eMB_ENOERR;
  uint16_t iRegIndex, iRegBitIndex, iNReg;
  uint8_t *pucDiscreteInputBuf;
  uint16_t usDiscreteInputStart;

  iNReg = usNDiscrete / 8 + 1;

  pucDiscreteInputBuf = ucMDiscInBuf[eMB_FrameGetSlaveAddressCalloutArr() - 1];
  usDiscreteInputStart = usMDiscInStart;

  /* it already plus one in modbus function method. */
  usAddress--;

  if ((usAddress >= M_DISCRETE_INPUT_START) &&
      (usAddress + usNDiscrete <= M_DISCRETE_INPUT_START + M_DISCRETE_INPUT_NDISCRETES))
  {
    iRegIndex = (uint16_t)(usAddress - usDiscreteInputStart) / 8;
    iRegBitIndex = (uint16_t)(usAddress - usDiscreteInputStart) % 8;

    /* write current discrete values with new values from the protocol stack. */
    while (iNReg > 1)
    {
      eMB_Util_SetBits(&pucDiscreteInputBuf[iRegIndex++], iRegBitIndex, 8, *pucRegBuffer++);
      iNReg--;
    }

    /* last discrete */
    usNDiscrete = usNDiscrete % 8;

    /* eMB_Util_SetBits has bug when ucNBits is zero */
    if (usNDiscrete != 0)
    {
      eMB_Util_SetBits(&pucDiscreteInputBuf[iRegIndex++], iRegBitIndex, usNDiscrete, *pucRegBuffer++);
    }
  }
  else
  {
    errStatus = eMB_ENOREG;
  }

  return errStatus;
}

/**
 * Modbus holding register callback function.
 *
 * @param pucRegBuffer holding register buffer
 * @param usAddress holding register address
 * @param usNRegs holding register number
 *
 * @return result
 */
eMB_ErrorCodeType eMB_Util_FuncHoldingRegisterCallback
(
  uint16_t usAddress,
  uint16_t usNRegs,
  uint8_t *pucRegBuffer
)
{
  eMB_ErrorCodeType errStatus = eMB_ENOERR;
  uint16_t iRegIndex;
  uint16_t *pusRegHoldingBuf;
  uint16_t usRegHoldStart;

  pusRegHoldingBuf = usMRegHoldBuf[eMB_FrameGetSlaveAddressCalloutArr() - 1];
  usRegHoldStart = usMRegHoldStart;
  
  /* it already plus one in modbus function method. */
  usAddress--;

  if ((usAddress >= M_REG_HOLDING_START) &&
      (usAddress + usNRegs <= M_REG_HOLDING_START + M_REG_HOLDING_NREGS))
  {
    iRegIndex = usAddress - usRegHoldStart;

    while (usNRegs > 0)
    {
      pusRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
      pusRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
      iRegIndex++;
      usNRegs--;
    }
  }
  else
  {
    errStatus = eMB_ENOREG;
  }
  
  return errStatus;
}

/**
 * Modbus input register callback function.
 *
 * @param pucRegBuffer input register buffer
 * @param usAddress input register address
 * @param usNRegs input register number
 *
 * @return result
 */
eMB_ErrorCodeType eMB_Util_FuncInputRegisterCallback
(
  uint16_t usAddress,
  uint16_t usNRegs,
  uint8_t *pucRegBuffer
)
{
  eMB_ErrorCodeType errStatus = eMB_ENOERR;
  uint16_t iRegIndex;
  uint16_t *pusRegInputBuf;
  uint16_t usRegInStart;

  pusRegInputBuf = usMRegInBuf[eMB_FrameGetSlaveAddressCalloutArr() - 1];
  usRegInStart = usMRegInStart;

  /* it already plus one in modbus function method. */
  usAddress--;

  if ((usAddress >= M_REG_INPUT_START) &&
      (usAddress + usNRegs <= M_REG_INPUT_START + M_REG_INPUT_NREGS))
  {
    iRegIndex = usAddress - usRegInStart;

    while (usNRegs > 0)
    {
      pusRegInputBuf[iRegIndex] = *pucRegBuffer++ << 8;
      pusRegInputBuf[iRegIndex] |= *pucRegBuffer++;
      iRegIndex++;
      usNRegs--;
    }
  }
  else
  {
    errStatus = eMB_ENOREG;
  }

  return errStatus;
}



/* Get Modbus Master current error event type. */
eMB_ErrorEventType eMB_Util_GetErrorEvent(void)
{
  return eMB_ErrorEvent;
}

/* Set Modbus Master current error event type. */
void eMB_Util_SetErrorEvent(eMB_ErrorEventType errorType)
{
  eMB_ErrorEvent = errorType;
}



#ifdef __cplusplus
}
#endif
