local xml = require('LuaXml')
local cmake_result = {}
local path_result = {}

function split(fullstring, separator)
  local find_startindex = 1
  local splitindex = 1
  local splitarray = {}
  while true do
    local find_lastindex = string.find(fullstring, separator, find_startindex)
    if not find_lastindex then
      splitarray[splitindex] = 
        string.sub(fullstring, find_startindex, string.len(fullstring))
      break
    end
    splitarray[splitindex] = string.sub(fullstring, 
                                        find_startindex, 
                                        find_lastindex - 1)
    find_startindex = find_lastindex + string.len(separator)
    splitindex = splitindex + 1
  end
  return splitarray
end

function addfilter(parentname, filter)
  parentname = string.gsub(parentname, " ", "")
  local filterlist_name = parentname .. "_LIST"
  local paths = split(parentname, "_")
  local sourcegroup_line = ""
  for i = 1, #paths - 1 do
    sourcegroup_line = sourcegroup_line .. paths[i] .. "\\\\"
  end
  sourcegroup_line = sourcegroup_line .. paths[#paths]
  filterlist_name = string.upper(filterlist_name)
  local filterline = "SET (" .. filterlist_name .. "\n"
  local filelines = ""
  if type(filter) == "table" then
    for i = 1, #filter do
      if filter[i]:tag() == "Filter" then
        addfilter(parentname .. "_" .. filter[i].Name, filter[i])
      elseif filter[i]:tag() == "File" then
        filelines = filelines .. addfile(filter[i]) .. "\n"
      end
    end
  end
  filterline = filterline .. filelines .. ")\n"
  local tablesize = #cmake_result
  cmake_result[tablesize + 1] = {}
  cmake_result[tablesize + 1].filterlist_name = filterlist_name
  cmake_result[tablesize + 1].filterline = filterline
  cmake_result[tablesize + 1].sourcegroup_line = sourcegroup_line
  return filterlist_name, filterline
end

function addfile(file)
  local fileline = ""
  if type(file) ~= "table" then return fileline end
  if string.find(file.RelativePath,"CMakeLists.txt") ~= nil then 
    return fileline 
  end
  if string.sub(file.RelativePath, 1, 2) == ".\\" then
    file.RelativePath = string.sub(file.RelativePath, 3)
  end
  file.RelativePath = string.gsub(file.RelativePath, " ", "")
  local files = split(file.RelativePath, "\\")
  for i = 1, #files - 1 do
    fileline = fileline .. files[i] .. "/"
  end
  fileline = "\t" .. fileline
  local filepath = ""
  for i = 1, #files - 2 do
    filepath  = filepath .. files[i] .. "\\\\"
  end

  if #files > 1 then
    filepath  = filepath .. files[#files - 1]
  end
  path_result[filepath] = filepath
  fileline = fileline .. files[#files]
  return fileline
end

function fill_cmake(vcprojname)
  local xfile = xml.load(vcprojname)
  local files = xfile:find("Files")
  local filterlist_name = vcprojname .. "_LIST"
  filterlist_name = string.upper(filterlist_name)
  local filterline = "SET (" .. filterlist_name .. "\n"
  local filelines = ""
  if type(files) == "table" then
    for i = 1, #files do
      if files[i]:tag() == "Filter" then
        addfilter(files[i].Name, files[i])
      elseif files[i]:tag() == "File" then
        filelines = filelines .. addfile(files[i]).. "\n"
      end
    end
    filterline = filterline .. filelines .. ")\n"
    if string.len(filelines) > 0 then
      local tablesize = #cmake_result
      cmake_result[tablesize + 1] = {}
      cmake_result[tablesize + 1].filterlist_name = filterlist_name
      cmake_result[tablesize + 1].filterline = filterline
      cmake_result[tablesize + 1].sourcegroup_line = ""
    end
  end
end

-- get cmake from vs vcproj file
-- @param string vcprojname
-- @param table | string  include_path
function get_vcproj_cmake(vcprojname, include_path, is_lib, libtype)
  local cmake_text = ""
  cmake_result = {}
  path_result = {}
  fill_cmake(vcprojname)
  --[[
  for key, value in pairs(path_result) do
    if(string.len(value) > 0) then
      cmake_text = cmake_text .. "INCLUDE_DIRECTORIES(" ..value .. ")\n"
    end
  end
  --]]

  -- spiecal inlucde dir
  if "string" == type(include_path) then
      cmake_text = cmake_text .. "INCLUDE_DIRECTORIES(" ..include_path .. ")\n"
  elseif "table" == type(include_path) then
      for _, val in pairs(include_path) do
          cmake_text = cmake_text .. "INCLUDE_DIRECTORIES(" ..val .. ")\n"
      end
  end
  cmake_text = cmake_text .. "\n\n"
  for i = 1, #cmake_result do
    if(string.len(cmake_result[i].filterline) > 0) then
      cmake_text = cmake_text .. cmake_result[i].filterline .. "\n"
    end
  end
  cmake_text = cmake_text .. "\n\n"
  cmake_text = cmake_text .. "if (WIN32)\n"
  for i = 1, #cmake_result do
    if(string.len(cmake_result[i].filterline) > 0) then
      cmake_text = cmake_text .. "\tsource_group(" .. 
                   cmake_result[i].sourcegroup_line .. 
                   " FILES ${" .. cmake_result[i].filterlist_name .. "})\n"
    end
  end
  cmake_text = cmake_text .. "endif()\n"
  cmake_text = cmake_text .. "\n\n"
  local exefiles = split(vcprojname, "/")
  local firstpos = string.find(exefiles[#exefiles], "%.")
  local projectname = string.sub(exefiles[#exefiles], 1, firstpos - 1);
  if is_lib then
    cmake_text = cmake_text.."SET(LIBRARY_OUTPUT_PATH ${TF_CURRENT_BINARY_PATH})".."\n"
    cmake_text = cmake_text .. "ADD_LIBRARY(" .. projectname .. " ".. string.upper(libtype) .. "\n"
  else
    cmake_text = cmake_text .. "ADD_EXECUTABLE(" .. projectname .. "\n"
  end
  for i = 1, #cmake_result do
    if(string.len(cmake_result[i].filterline) > 0) then
      cmake_text = cmake_text .. "\t${" .. 
                   cmake_result[i].filterlist_name .. "}\n"
    end
  end
  cmake_text = cmake_text .. ")"
  return cmake_text
end
