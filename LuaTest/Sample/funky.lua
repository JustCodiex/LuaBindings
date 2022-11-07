function add(a,b) 
	return a + b;
end

function sum(...)
	local args = { ... };
	local n = 0;
	for i = 1, #args do
		n = n + args[i];
	end
	return n;
end
