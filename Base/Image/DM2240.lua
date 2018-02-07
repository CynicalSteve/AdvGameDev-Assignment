function SaveToGameFile(outputString, overwrite)
   print("SaveToLuaFile...")
   local f;						-- The file
   if overwrite == 1 then		-- Wipe the contents with new data
      f = assert(io.open("Image/GameSaveData.lua", "w"))
   elseif overwrite == 0 then	-- Append with new data
      f = assert(io.open("Image/GameSaveData.lua", "a"))
   end
   -- Write to the file
   f:write(outputString)
   -- Close the file
   f:close()
   print("OK")
end

function CalculateDistanceSquared(x1,y1,z1,x2,y2,z2)
	local distanceSquare = (x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1) + (z2 - z1)*(z2 - z1)
	print(distanceSquare)
	return distanceSquare
end

function GetMinMax(...)
	local minValue = -1
	local maxValue = -1
	local avgValue = -1
	local numValues = 0
	for i,v in ipairs(arg) do
		if minValue == -1 then
			minValue = v
		elseif v < minValue then
			minValue = v
		end

		if maxValue == -1 then
			maxValue = v
		elseif v > maxValue then
			maxValue = v
		end
		
		avgValue = avgValue + v
		numValues = numValues + 1
    end
	avgValue = avgValue / numValues
	print(minValue, maxValue, avgValue, numValues)
	return minValue, maxValue, avgValue, numValues
end

title = "DM2240 - Week 14 Scripting"

-- Keyboard Inputs
moveForward  = "W"
moveBackward = "S"
moveLeft     = "A"
moveRight    = "D"

SaveGame = "L"

PrevWeapon = "Q"
NextWeapon= "E"

-- Way points
Waypoint_A_1 = {x=10.0, y=0.0, z=50.0}
Waypoint_A_2 = {x=10.0, y=0.0, z=-50.0}
Waypoint_A_3 = {x=-10.0, y=0.0, z=0.0}

--Ground Entity
GroundPos={0, -10, 0}
GroundScale={100.0, 100.0, 100.0}
GroundGrids={10.0, 1.0, 10.0}

--Game Objects
AxesName="reference"
CrossHairName = "crosshair"

quad={Name="quad", length=1.0, Texture="Image//calibri.tga"}
quadColor="white"

text={Name="text", row=16, column=16, Texture="Image//calibri.tga"}
textMaterialRGB={r=1.0,g=0.0,b=0}

HQAlien={Name="HQAlien", ObjDir="OBJ//alien.obj", Texture="Image//alien.tga"}
LQAlien={Name="LQAlien", ColorName="white", length=5.0, Texture="Image//alienFront.tga"}

ring={Name="ring", ColorName="violet", slices=36, innerRadius=1, outerRadius=1}
lightball={Name="lightball", ColorName="white", stacks=18.0, slices=36.0};

sphere={Name="sphere", ColorName="red", stacks=18, slices=36, radius=0.5}
cube={Name="cube", ColorName="yellow"}
cone={Name="cone", slices=36, radius=10, height=10}
coneColor={r=0.5, g=1, b=0.3}

DarkGrass={Name="GRASS_DARKGREEN", ColorName="white", Texture="Image//grass_darkgreen.tga"}
LightGrass={Name="GEO_GRASS_LIGHTGREEN", ColorName="white", Texture="Image//grass_lightgreen.tga"}

CubeSG={Name="cubeSG"}
CubeSGColor={r=1, g=0.64, b=0}

SkyboxFront={Name="SKYBOX_FRONT", ColorName="white", Texture="Image//SkyBox//skybox_front.tga"}
SkyboxBack={Name="SKYBOX_BACK", ColorName="white", Texture="Image//SkyBox//skybox_back.tga"}
SkyboxLeft={Name="SKYBOX_LEFT", ColorName="white", Texture="Image//SkyBox//skybox_left.tga"}
SkyboxRight={Name="SKYBOX_RIGHT", ColorName="white", Texture="Image//SkyBox//skybox_right.tga"}
SkyboxTop={Name="SKYBOX_TOP", ColorName="white", Texture="Image//SkyBox//skybox_top.tga"}
SkyboxBottom={Name="SKYBOX_BOTTOM", ColorName="white", Texture="Image//SkyBox//skybox_bottom.tga"}

Laser={Name="laser", length=10}
GridMesh={Name="GRIDMESH", ColorName="white", length=10}
GridMeshSize={xGridSize=100, zGridSize=100, xNumofGrid=10, zNumofGrid=10}
GridLoD={distance_High2Mid = 40000, distance_Mid2Low=160000}

Safehouse={Name="Safehouse", ObjDir="OBJ//safehouse.obj", Texture="Image//safehouse.tga"}
CloverBuilding={Name="CloverBuilding", ObjDir="OBJ//cloverbuilding.obj", Texture="Image//cloverbuilding.tga"}
JigsawTower={Name="JigsawTower", ObjDir="OBJ//JigsawTower.obj", Texture="Image//jigsawbuilding.tga"}
Pavement={Name="GEO_PAVEMENT", ColorName="white", Texture="Image//pavement.tga"}