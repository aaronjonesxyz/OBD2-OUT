// logging.h - SD Card logging to CSV
#ifndef SDCARD_CS
#define SDCARD_CS PB12
#endif

class LoggerClass {
  public:
    LoggerClass( int aPids, int nPids );

    int logEntry( int curPIDVals[20] );

  private:
    int fnum;
    File file;
    char filename[12];
    int * _activePIDs;
    int _numPids;
};