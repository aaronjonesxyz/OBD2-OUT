// logging.h - SD Card logging to CSV
#ifndef LOGGING_H
#define LOGGING_H

#include <SD.h>

class LoggerClass {
  public:
    void init();
    void logEntry();

  private:
    uint8_t fnum;
    File file;
    char filename[12];
    uint8_t lines = 0;
};

#endif