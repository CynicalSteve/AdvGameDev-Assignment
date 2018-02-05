MoveUp = "W"
MoveDown = "S"

ScoreIncrease="D"
ScoreDecrease="A"

PlayGameButton={posY=600,scaleX=400,scaleY=150}
OptionsButton={posY=450, scaleX=400,scaleY=150}
HighscoreButton={posY=300, scaleX=400,scaleY=150}
ExitButton={posY=150,scaleX=400,scaleY=150}

Resolution={posY=600, scaleX=400,scaleY=150}
SaveExitButton={posY=450, scaleX=400,scaleY=150}
CancelExitButton={posY=300, scaleX=400,scaleY=150}

SaveScoreButton={posY=300, scaleX=400,scaleY=150}
ExitMenuButton={posY=150, scaleX=400,scaleY=150}

function SaveResolution(outputString, overwrite)
   print("SaveToLuaFile...")
   local f;						-- The file
   if overwrite == 1 then		-- Wipe the contents with new data
      f = assert(io.open("Image/userConfig.lua", "w"))
   elseif overwrite == 0 then	-- Append with new data
      f = assert(io.open("Image/UserConfig.lua", "a"))
   end
   -- Write to the file
   f:write(outputString)
   -- Close the file
   f:close()
   print("OK")
end

function SaveHighscore(outputString, overwrite)
   print("SaveToLuaFile...")
   local f;						-- The file
   if overwrite == 1 then		-- Wipe the contents with new data
      f = assert(io.open("Image/DM2240_HighScore.lua", "w"))
   elseif overwrite == 0 then	-- Append with new data
      f = assert(io.open("Image/DM2240_HighScore.lua", "a"))
   end
   -- Write to the file
   f:write(outputString)
   -- Close the file
   f:close()
   print("OK")
end