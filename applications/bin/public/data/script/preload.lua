require("public/data/script/test")
local test = luatest()
mytest()
print(test["basekey1"]["key7"])
local tb = file.opentab("public/data/setting/hairstyle.txt")
print(tb[0].cost_itemid)
local tb1 = file.openini("public/data/setting/hostset.ini");
print(tb1.WorldSet["1_File"])
