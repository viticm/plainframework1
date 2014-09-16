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

local gateway_cmakelists = "../../../../applications/gateway/src/CMakeLists.txt"
local gateway_vcproj = "../../../../applications/gateway/scripts/gateway.vc9.vcproj"
local pf_simple_cmakelists = "../../../../applications/pf_simple/src/CMakeLists.txt"
local pf_simple_vcproj = "../../../../applications/pf_simple/scripts/pf_simple.vc9.vcproj"
local pf_core_cmakelists = "../../../../pf/core/src/CMakeLists.txt"
local pf_core_vcproj = "../../../../pf/core/scripts/pf_core.vc9.vcproj"


update_CMakeLists(gateway_cmakelists, gateway_vcproj)
update_CMakeLists(pf_simple_cmakelists, pf_simple_vcproj)
update_CMakeLists(pf_core_cmakelists, pf_core_vcproj, nil, true, "shared")
