@echo off
title rename for visual studio(pap)
color 02
cls
echo select your need change model.
echo (1)all (2)gateway (3)pf_simple (4)pf_core (5)sharememory (6)login (7)center (8) plugin_pak
set /p select=your select?
echo revert to standard?
set revert=-1
set /p revert=(yes/no)?
set all="pf_core|pf_simple|gateway|sharememory|login|center|plugin_pak"
if 1 == %select% php ../php/vcbuild.php %all% %revert%
if 2 == %select% php ../php/vcbuild.php "gateway" %revert%
if 3 == %select% php ../php/vcbuild.php "pf_simple" %revert%
if 4 == %select% php ../php/vcbuild.php "pf_core" %revert%
if 5 == %select% php ../php/vcbuild.php "sharememory" %revert%
if 6 == %select% php ../php/vcbuild.php "login" %revert%
if 7 == %select% php ../php/vcbuild.php "center" %revert%
if 8 == %select% php ../php/vcbuild.php "plugin_pak" %revert%

pause
