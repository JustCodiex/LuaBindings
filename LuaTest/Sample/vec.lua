-- Create our initial vectors
local a = vector3(1.5, 2.3, 7.5)
local b = vector3(7.1, 2.2, 6.5)

-- Compute sum vector
local c = a + b

-- Print
print(c.x, c.y, c.z)

-- Compute with c + b
local d = c + b

-- Print
print(d.x, d.y, d.z)
