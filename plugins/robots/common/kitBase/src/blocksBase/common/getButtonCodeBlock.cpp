/* Copyright 2007-2015 QReal Research Group
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License. */

#include "kitBase/blocksBase/common/getButtonCodeBlock.h"

#include <kitBase/robotModel/robotParts/button.h>
#include <kitBase/robotModel/robotModelUtils.h>

using namespace kitBase::blocksBase::common;
using namespace kitBase::robotModel;

GetButtonCodeBlock::GetButtonCodeBlock(RobotModelInterface &robotModel)
	: WaitBlock(robotModel)
{
}

void GetButtonCodeBlock::run()
{
	for (const PortInfo &port : mRobotModel.availablePorts()) {
		robotParts::Button *button = RobotModelUtils::findDevice<robotParts::Button>(mRobotModel, port.name());
		if (button) {
			mButtons << port.name();
		}
	}

	mActiveWaitingTimer.start();
}

void GetButtonCodeBlock::timerTimeout()
{
	for (const QString &buttonPort : mButtons) {
		robotParts::Button *button = RobotModelUtils::findDevice<robotParts::Button>(mRobotModel, buttonPort);
		if (evalCode<bool>(button->port().reservedVariable())) {
			evalCode(stringProperty("Variable") + " = " + QString::number(button->code()));
			stop();
		}
	}
}

DeviceInfo GetButtonCodeBlock::device() const
{
	return DeviceInfo();
}
