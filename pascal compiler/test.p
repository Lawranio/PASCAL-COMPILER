program kekw;
var
a, b, c : integer; 
d: integer;
begin
a := 10;
b := 1;
d := 0;
for c := 0 to 4 do begin
	b := 1 + c; 
	if b > 3 then a:= 2225;
	else d := d + 10;
end;
end.   