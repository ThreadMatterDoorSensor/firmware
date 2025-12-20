/*
 * Copyright (c) 2025 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include "app_task.h"

#include "app/matter_init.h"
#include "app/task_executor.h"
#include "board/board.h"
#include "clusters/identify.h"

#include "lib/core/CHIPError.h"
#include <app-common/zap-generated/attributes/Accessors.h>
#include <app/clusters/boolean-state-server/CodegenIntegration.h>

#include <zephyr/logging/log.h>

LOG_MODULE_DECLARE(app, CONFIG_CHIP_APP_LOG_LEVEL);

using namespace ::chip;
using namespace ::chip::app;
using namespace ::chip::DeviceLayer;

namespace
{
constexpr chip::EndpointId kContactSensorEndpointId = 1;

Nrf::Matter::IdentifyCluster sIdentifyCluster(kContactSensorEndpointId);

#ifdef CONFIG_CHIP_ICD_UAT_SUPPORT
#define UAT_BUTTON_MASK DK_BTN3_MSK
#endif
} /* namespace */

void AppTask::UpdateContactState(bool isOpen)
{
	Nrf::PostTask([isOpen] {
		auto booleanState =
			app::Clusters::BooleanState::FindClusterOnEndpoint(
				kContactSensorEndpointId);
		if (booleanState == nullptr) {
			LOG_ERR("Updating contact sensor state failed");
			return;
		}
		booleanState->SetStateValue(!isOpen);
	});
}

void AppTask::ButtonEventHandler(Nrf::ButtonState state,
				 Nrf::ButtonMask hasChanged)
{
#ifdef CONFIG_CHIP_ICD_UAT_SUPPORT
	if ((UAT_BUTTON_MASK & state & hasChanged)) {
		LOG_INF("ICD UserActiveMode has been triggered.");
		Server::GetInstance().GetICDManager().OnNetworkActivity();
	}
#endif
}

CHIP_ERROR AppTask::Init()
{
	/* Initialize Matter stack */
	ReturnErrorOnFailure(Nrf::Matter::PrepareServer());

	if (!Nrf::GetBoard().Init(ButtonEventHandler)) {
		LOG_ERR("User interface initialization failed.");
		return CHIP_ERROR_INCORRECT_STATE;
	}

	/* Register Matter event handler that controls the connectivity status
	 * LED based on the captured Matter network state. */
	ReturnErrorOnFailure(Nrf::Matter::RegisterEventHandler(
		Nrf::Board::DefaultMatterEventHandler, 0));

	ReturnErrorOnFailure(sIdentifyCluster.Init());

	return Nrf::Matter::StartServer();
}

CHIP_ERROR AppTask::StartApp()
{
	ReturnErrorOnFailure(Init());

	while (true) {
		Nrf::DispatchNextTask();
	}

	return CHIP_NO_ERROR;
}
