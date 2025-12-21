/*
 * Copyright (c) 2025 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#pragma once

#include "board/board.h"

#include <platform/CHIPDeviceLayer.h>

struct Identify;

class AppTask
{
      public:
	static AppTask &Instance()
	{
		static AppTask sAppTask;
		return sAppTask;
	};

	CHIP_ERROR StartApp();
	static void UpdateContactState(bool isOpen);
	static void UpdateBatVoltage(int32_t voltage_mv);

      private:
	CHIP_ERROR Init();

	static void ButtonEventHandler(Nrf::ButtonState state,
				       Nrf::ButtonMask hasChanged);
};
