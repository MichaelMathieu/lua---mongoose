require 'torch'
require 'dok'
require 'image'
require 'xlua'

local help_desc = [[
      Mongoose 9 Dof reader for firmware found on
      http://myrhev.net/?page_id=263
]]

mongoose = {}

-- load C lib
require 'libmongoose'

function mongoose.new(tty)
   local mgdata = torch.Tensor(20):zero()
   local mg = libmongoose.initMongoose('/dev/ttyUSB0')
   if mg == nil then
      error("Could not open tty "..tty.." . Please check that the path is right and that you have the correct permission (you might have to join the group dialout).")
   end
   return {mg, mgdata}
end

function mongoose.release(mg)
   libmongoose.releaseMongoose(mg[1])
end

function mongoose.fetchData(mg)
   mg[2].libmongoose.fetchMongooseData(mg[1], mg[2])
end

function mongoose.getRotmat(mg)
   if mg[2][1] ~= 0 then
      return mg[2][{{11,19}}]
   else
      return nil
   end
end

function mongoose.getAccel(mg)
   if mg[2][1] ~= 0 then
      return mg[2][{{2,4}}]
   else
      return nil
   end
end

function mongoose.getTime(mg)
   if mg[2][1] ~= 0 then
      return mg[2][1]
   else
      return nil
   end
end