#pragma once

#include "DefineFiles/DefineFile.h"
#include <vector>

//#include "Device/ScsiIo.h"
#include "ScsiCmd.h"

#include "ScsiIoIf.h"

using namespace std;

class ScsiIf
{
public:
    ScsiIf(HANDLE handle, estring deviceName, estring description);
    ScsiIf();
 
    ~ScsiIf(void);

    HANDLE m_handle;
    estring m_deviceName;
    estring m_description;

    
    //base function
    void init_disk();
    void send_cmd(BYTE cdb[16], ULONG dataXferLen, BYTE direction, eu8_p buffer);
    void send_cmd(ScsiCmd cmd, eu8_p buffer);


    // ufi cmd
    void write_10(eu32 lba, eu16 secCnt, eu8_p buffer);
    void read_10(eu32 lba, eu16 secCnt, eu8_p buffer);
    void inquiry(eu8_p buffer);
    void read_capacity(eu8_p buffer);
    
    
private:
	
    

    ScsiIoIf m_scsiIoIf;
  



};

