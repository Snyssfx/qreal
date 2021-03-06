# Copyright 2007-2015 QReal Research Group
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

TARGET = qrgui-hotkey-manager

include(../../global.pri)

TEMPLATE = lib

links(qrgui-preferences-dialog)
includes(qrgui)

QT += widgets

DEFINES += QRGUI_HOTKEY_MANAGER_LIBRARY

TRANSLATIONS = $$PWD/../../qrtranslations/ru/qrgui_hotKeyManager_ru.ts

HEADERS += \
	$$PWD/hotKeyManagerDeclSpec.h \
	$$PWD/hotKeyManagerPage.h \
	$$PWD/hotKeyManager.h \
	$$PWD/shortcutEdit.h \

SOURCES += \
	$$PWD/hotKeyManagerPage.cpp \
	$$PWD/hotKeyManager.cpp \
	$$PWD/shortcutEdit.cpp \

FORMS += \
	$$PWD/hotKeyManagerPage.ui \

RESOURCES += \
	$$PWD/hotKeyManager.qrc \
