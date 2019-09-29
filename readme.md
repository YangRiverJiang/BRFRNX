#### This project is for generating brief GNSS information for a RINEX 3.02 file including:
###### 1. Properties like duration, sampling-rate, total epoch.
###### 2. GNSS constellation visibility (GPS GLO GAL BDS QZS IRN LEO SBS), average satellite amount and std, min, max for each constellation.
###### 3. Observation types summerizing and missing signal statistics for each satellite.
###### 4. Missing-epoch statistics (if any).

#### This program is provided AS IS.
###### If any bugs or function-adding suggestions please come to me at: yang.jiang1@ucalgary.ca

###### BRFRNX.exe             --  Compiled binery executable (Windows Microsoft Visual Studio 2017 x86 Release)
###### main.cpp               --  Source code of BRFRINX.exe

#### Usage on Windows
###### CMD: BRFRNX.exe "./test_data/bamf2300.obs"
###### The program would automatically generate the report file "bamf2300.brief.txt" which is the same as the sample offered.
###### And open it using default text editor software while finished.
###### A shortcut usage is to drag the RINEX file onto a BRFRNX.exe.

#### Notice
###### 1. Currently only RINEX ver 3.02 is supported. Working with other RINEX3 format may be glitching. For RINEX2 files please convert it in advance. One possible great converting software is RTKCONV.exe from RTKLIB: https://github.com/tomojitakasu/RTKLIB_bin/raw/rtklib_2.4.3/bin/rtkconv.exe
###### 2. Memory cleanning is denied in current implementation because I'm just being lazy.
###### 3. For more self-designed RINEX pre-processing development please contact me.
