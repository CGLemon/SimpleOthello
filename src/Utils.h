#ifndef UTILS_H_DEFINED
#define UTILS_H_DEFINED

#include "config.h"
#include "cfg.h"

#include <string>
#include <atomic>



namespace Utils {
	static bool logfile_handle = false;
	static FILE * log_filename;

    void myprintf(const char *fmt, ...);
    void log_input(std::string input);
}

#endif
