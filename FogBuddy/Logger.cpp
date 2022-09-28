#include "Logger.h"

namespace Logger {
	// A trick so everything that includes this module shares the same logging level
	Level* getLogLevel() {
		static Level level;
		return &level;
	}
}