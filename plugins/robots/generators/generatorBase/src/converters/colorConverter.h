#pragma once

#include "enumConverterBase.h"

namespace generatorBase {
namespace converters {

/// Converts 'Color' enum in WaitForColor blocks into generator-specific code.
/// Resulting code must be specified in templates from "colors" folder.
class ColorConverter : public EnumConverterBase
{
public:
	explicit ColorConverter(QString const &pathToTemplates);
};

}
}
