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