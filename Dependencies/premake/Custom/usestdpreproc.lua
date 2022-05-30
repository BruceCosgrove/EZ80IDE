require('vstudio')

local p = premake
local m = p.vstudio.vc2010

p.api.register {
	name = "usestdpreproc",
	scope = "config",
	kind = "boolean"
}

function useStdPreProc(cfg)
	if _ACTION >= "vs2017" then
		if cfg.usestdpreproc ~= nil then
			m.element("UseStandardPreprocessor", nil, iif(cfg.usestdpreproc, "true", "false"))
		end
	end
end

p.override(m.elements, "clCompile", function(base, cfg)
	local calls = base(cfg)
	table.insertafter(calls, m.languageStandardC, useStdPreProc)
	return calls
end)
