require('readvcproj')

function update_CMakeLists(cmakefile, vcprog, include_path, is_lib, libtype)
  local newcontent = ""
  local fp = io.open(cmakefile, "r")
  if not fp then return false end
  local filecontent = fp:read("*all")
  local len = string.len(filecontent)
  local begin_position1, begin_position2 = 
    string.find(filecontent, '##update_begin')
  local begintext = string.sub(filecontent, 1, begin_position2 + 1);
  local makefile = get_vcproj_cmake(vcprog, include_path, is_lib, libtype)
  local endposition = string.find(filecontent, '##update_end')
  local endtext = string.sub(filecontent, endposition - 1);
  newcontent = begintext .. '\n\n' .. makefile .. '\n\n' ..endtext
  local wf = io.open(cmakefile, "w")
  wf:write(newcontent)
  wf:close()
end

local billing_cmakelists = "../../../../applications/billing/src/CMakeLists.txt"
local billing_vcproj = "../../../../applications/billing/scripts/billing.vc9.vcproj"
local pf_simple_cmakelists = "../../../../applications/pf_simple/src/CMakeLists.txt"
local pf_simple_vcproj = "../../../../applications/pf_simple/scripts/pf_simple.vc9.vcproj"
local pf_core_cmakelists = "../../../../pf/core/src/CMakeLists.txt"
local pf_core_vcproj = "../../../../pf/core/scripts/pf_core.vc9.vcproj"
local pf_plugin_pak_cmakelists = "../../../../pf/plugin/pak/src/CMakeLists.txt"
local pf_plugin_pak_vcproj = "../../../../pf/plugin/pak/scripts/plugin_pak.vc9.vcproj"
local sharememory_cmakelists = "../../../../applications/sharememory/src/CMakeLists.txt"
local sharememory_vcproj = "../../../../applications/sharememory/scripts/sharememory.vc9.vcproj"
local login_cmakelists = "../../../../applications/login/src/CMakeLists.txt"
local login_vcproj = "../../../../applications/login/scripts/login.vc9.vcproj"
local center_cmakelists = "../../../../applications/center/src/CMakeLists.txt"
local center_vcproj = "../../../../applications/center/scripts/center.vc9.vcproj"

update_CMakeLists(billing_cmakelists, billing_vcproj)
update_CMakeLists(sharememory_cmakelists, sharememory_vcproj)
update_CMakeLists(pf_simple_cmakelists, pf_simple_vcproj)
update_CMakeLists(pf_core_cmakelists, pf_core_vcproj, nil, true, "shared")
update_CMakeLists(pf_plugin_pak_cmakelists, pf_plugin_pak_vcproj, nil, true, "shared")
update_CMakeLists(login_cmakelists, login_vcproj)
update_CMakeLists(center_cmakelists, center_vcproj)
