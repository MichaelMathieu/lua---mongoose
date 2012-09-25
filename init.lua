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
   local mgdata = torch.Tensor(19):zero()
   local mg = mgdata.libmongoose.initMongoose('/dev/ttyUSB0', mgdata)
   if mg == nil then
      error("Could not open tty "..tty.." . Please check that the path is right and that you have the correct permission (you might have to join the group dialout).")
   end
   return {mg, mgdata}
end

function mongoose.release(mg)
   mg[2].libmongoose.releaseMongoose(mg[1])
end

function mongoose.fetchData(mg)
   mg[2].libmongoose.fetchMongooseData(mg[1])
end

function mongoose.getRotmat(mg)
   if mg[2][19] ~= 0 then
      return mg[2][{{9,18}}]:resize(3,3)
   else
      return nil
   end
end