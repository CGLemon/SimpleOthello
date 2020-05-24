#include "config.h"

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <cstdarg>
#include <thread>
#include <mutex>

#include "Utils.h"

std::mutex IOmutex;

void Utils::myprintf(const char *fmt, ...) {
    if (cfg_quiet) return;
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
	
    if (logfile_handle && log_filename) {
        std::lock_guard<std::mutex> lock(IOmutex);
        va_start(ap, fmt);
        vfprintf(log_filename, fmt, ap);
        va_end(ap);
    }
	
}


void Utils::log_input(std::string input) {
    if (logfile_handle && log_filename) {
        std::lock_guard<std::mutex> lock(IOmutex);
        fprintf(log_filename, ">>%s\n", input.c_str());
    }
}
