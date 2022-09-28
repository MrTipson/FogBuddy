#include "Logger.h"

namespace Logger {
	Level* getLogLevel() {
		static Level level;
		return &level;
	}
}