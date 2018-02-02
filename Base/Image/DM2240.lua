title = "DM2240 - Week 14 Scripting"
width = 800
height = 600

--Keyboard inputs
moveForward = "W"
moveBackward = "S"
moveLeft = "A"
moveRight = "D"

--Player Start Position
PlayerStartingPos = {0,0,10}

function SaveToLuaFile(outputString, overwrite)
   print("SaveToLuaFile...")
   local f;                     --The file
   if overwrite == 1 then       --Wipe the contents with new data
      f = assert(io.open("Image/DM2240_HighScore.lua", "w"))
   elseif overwrite == 0 then   --Append with new data
      f = assert(io.open("Image/DM2240_HighScore.lua" , "a"))
   end
    --Write to the file
	f:write(outputString)
	--Close the file
	f:close()
	print("OK")
end

function CalculateDistanceSquared(x1, y1, z1, x2, y2, z2)
   local distanceSquare = (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1) + (z2 - z1) * (z2 - z1)
   print (distanceSquare)
   return distanceSquare
end 

--Waypoints
Waypoint_A1 = {x=10.0, y=10.0, z=50.0}
Waypoint_A2 = {x=10.0, y=0.0, z=50.0}
Waypoint_A3 = {x=10.0, y=0.0, z=0.0}