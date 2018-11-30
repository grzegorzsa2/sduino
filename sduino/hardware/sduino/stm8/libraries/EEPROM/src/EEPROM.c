/*
  EEPROM.h - EEPROM library
  Plain-C version for SDuino by Michael Mayer 2018.

  This is a rewrite from scratch. The basic API is inspired by the
  Arduino EEPROM library, but none of the operator overloading functions
  can be ported in any sensible way to C.

  Original Copyright (c) 2006 David A. Mellis.  All right reserved.
  New version by Christopher Andrews 2015.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <string.h>
#include "EEPROM.h"


/* --- Arduino-like interface -------------------------------------------- */

void EEPROM_write( int idx, uint8_t val )
{
	eeprom_unlock();
	if (eeprom_unlocked())
	{
		// write only after a successful unlock.
		EERef(idx) = val;
		// re-lock the EEPROM again.
		FLASH->IAPSR &= FLASH_FLAG_DUL;
	}
}



/* --- more flexible interface ------------------------------------------- */

void eeprom_unlock(void)
{
	if (!eeprom_unlocked())
	{
		// EEPROM still locked. Unlock first.
		FLASH->DUKR = FLASH_RASS_KEY2;
		FLASH->DUKR = FLASH_RASS_KEY1;
	}
}


void eeprom_lock(void)
{
	// re-lock the EEPROM again.
	FLASH->IAPSR &= FLASH_FLAG_DUL;
}



/**
 * write data into EEPROM area
 *
 * The EEPROM area is unlocked (if needed) and re-locked after the write.
 * Data is written byte-wise, word programming or block programming is not
 * supported.
 *
 * @returns: number of bytes successfully written to EEPROM
 */
uint16_t eeprom_write(uint16_t idx, uint8_t *ptr, uint16_t len)
{
	uint16_t written = 0;

	// make sure not to write data beyond the end of the EEPROM area
	// (this could accidentally hit the option byte area)
	if (idx >= EEPROM_end()) return 0;
	if (len+idx > EEPROM_end()) {
		len = EEPROM_end() - idx;
	}
	idx += (uint16_t)FLASH_DATA_START_PHYSICAL_ADDRESS;

	eeprom_unlock();
	if (eeprom_unlocked())
	{
		// write only after a successful unlock.
		while (len--)
		{
			*((uint8_t *) idx++) = *ptr++;
			written++;
		}
		// re-lock the EEPROM again.
		FLASH->IAPSR &= FLASH_FLAG_DUL;
	}

	return written;
}


/**
 * read data from EEPROM area
 *
 * The EEPROM area is unlocked (if needed) and re-locked after the write.
 * Data is written byte-wise, word programming or block programming is not
 * supported.
 *
 * @returns: number of bytes successfully written to EEPROM
 */
uint16_t eeprom_read(uint16_t idx, uint8_t *ptr, uint16_t len)
{
	// make sure not to read data beyond the end of the EEPROM area
	if (idx > E2END) return 0;
	if (len+idx > EEPROM_end()) {
		len = EEPROM_end() - idx;
	}
	memcpy(ptr, (void*)(((uint16_t)FLASH_DATA_START_PHYSICAL_ADDRESS)+idx), len);

	return len;
}



