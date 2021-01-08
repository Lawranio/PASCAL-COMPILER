program lab;
    var
        b: array [1..5] of integer;
        e, a: integer;
begin
	for a := 1 downto 5 do
	   e := e + a;
	   for a := 1 to 5 do
	      a := 1;
	
	b[2] := e + a - 1 * (5 + 4);
	begin
	   b[3] := b[4] + b[5];
	end;
end.           