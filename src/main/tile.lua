--[[
This file is part of semaphore.
Licensed under the MIT License,
               http://www.opensource.org/licenses/mit-license.php
Copyright 2008 James Shaw <js102@zepler.net>
]]--

require 'logging'
require 'logging.console'

local logger = logging.console()

require 'compass'

BLANK = {}

Track = {}

	function Track.__tostring(o)
		return tostring(o.vector) .. ' Track'
	end
	
	function Track:new(vector)
		local o = {
			vector = vector,
			occupier = nil -- train occupying the tile
		}
		setmetatable(o, self)
		self.__index = self
		return o
	end

	function Track:occupy(train)
		local vector = Track.calculate_vector(self.vector, train:direction())
		if vector then
			if self.occupier then
				-- TODO: move into helper function and reuse in Junction class (using inheritance)
				logger:info("train '" .. tostring(train) .. "' has crashed into " .. tostring(self.occupier))
				self.occupier:crash()
				train:crash()
			else
				self.occupier = train
				return vector
			end
		else
			-- TODO: move into helper function and reuse in Junction class (using inheritance)
			logger:error("train '" .. tostring(train.name) .. "' has crashed because track wasn't connected")
			train:crash()
		end
	end

	function Track:unoccupy(train)
		local is_occupied = self.occupier == train
		if is_occupied then
			logger:debug("Train '" .. train.name .. "' no longer occupies " .. tostring(self))
			self.occupier = nil
		else
			logger:error("Can't unoccupy " .. tostring(self) .. " because train '" .. train.name .. "' doesn't occupy it")
		end
		return is_occupied
	end

	function Track.calculate_vector(track_vector, train_direction)
		if track_vector[Vector.ENTRY] == train_direction.inverse then
			return track_vector
		end
		if track_vector[Vector.EXIT]  == train_direction.inverse then
			return track_vector:get_inverse()
		end
	end


JunctionTrack = {}

	function JunctionTrack:new(active, inactive)
		local o = {
			active = active,
			inactive = inactive,
			next = nil
		}
		setmetatable(o, self)
		self.__index = self
		return o
	end

	function JunctionTrack:is_switched()
		for i, v in ipairs(self.active) do
			if not v:is_straight() then
				return true
			end
		end
		return false
	end

Junction = {}

	function Junction:new(track, derail_protect)
		local o = {
			track = track,
			occupier = nil,
			derail_protect = derail_protect
		}
		setmetatable(o, self)
		self.__index = self
		return o
	end

	function Junction:switch_points()
		if self.occupier then
			logger:warn("Can't switch points because tile is occupied by train" .. tostring(self.occupier))
			return false
		else
			self.track = self.track.next
			return true
		end
	end

	function Junction:occupy(train)
		local vector = self:get_track(self.track.active, train:direction())
		if vector then
			if self.occupier then
				logger:info("train '" .. tostring(train) .. "' has crashed into " .. tostring(self.occupier))
				self.occupier:crash()
				train:crash()
			else
				self.occupier = train
				return vector
			end
		else
			logger:error("train '" .. tostring(train) .. "' has crashed because track wasn't connected")
			train:crash()
		end

		if self.derail_protect and train.speed == TrainType.SLOW then
			logger:debug("Derail protect is switching points for entering train " .. tostring(train))
			-- TODO
		end
	end

	function Junction:get_track(track_list, direction)
		for i, track in ipairs(track_list) do
			local vector = Track.calculate_vector(track, direction)
			if vector then return vector end
		end
	end

Flyover = {}

BufferStop = {}

