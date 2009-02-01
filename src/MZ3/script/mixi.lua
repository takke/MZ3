--------------------------------------------------
-- MZ3 API : mixi
--------------------------------------------------
module("mixi", package.seeall)

function hoge()
	print "fuga"
	mz3.hoge();
end

function after_parse()
end

--mz3.sethook("mixi", "after_parse", after_parse);
