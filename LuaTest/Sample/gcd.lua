
-- Compute the extended version of GCD
function gcd(a,b)
	if a == 0 then
		return b, 0, 1
	end
	v, x1, y1 = gcd(b % a, a)
	return v, (y1 - (b / a) * x1), x1
end
