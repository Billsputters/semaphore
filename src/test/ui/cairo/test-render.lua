--[[
This file is part of semaphore.
Licensed under the MIT License,
               http://www.opensource.org/licenses/mit-license.php
Copyright 2008 James Shaw <js102@zepler.net>
]]--

require('luaunit')
require('map')
require('signal')
require('tile')
require('ui.render-style')
require('ui.cairo.render')
local cairo = require("lcairo")

TestCairoRender = {}

	function TestCairoRender:testSet()
		local map = Map:new(5, 3)
		map:set(1, 1, Track:new{vector=Vector:new(W, E)})
		map:set(2, 1, Track:new{vector=Vector:new(W, NE)})
		map:set(3, 2, Track:new{vector=Vector:new(SW, NE)})
		map:set(4, 3, Track:new{vector=Vector:new(SW, E)})
		map:set(5, 3, Track:new{vector=Vector:new(W, E)})

		local scale=128

		local sfc = cairo.image_surface_create(cairo.FORMAT_RGB24, map.w*scale, map.h*scale)
		local ctx = cairo.create(sfc)
		cairo.scale(ctx, scale, scale)

		local render = CairoRender:new{map=map, style=RenderStyle}
		render:render(ctx)


		render:draw_main_signal(ctx, 2, 1, Signal:new{
			vector = Vector:new(SW, NE),
			type = Signal.MAIN_MANUAL,
			aspect = Signal.GREEN
		})

		render:draw_main_signal(ctx, 0, 0, Signal:new{
			vector = Vector:new(W, E),
			type = Signal.MAIN_AUTO,
			aspect = Signal.AMBER
		})

		render:draw_main_signal(ctx, 4, 2, Signal:new{
			vector = Vector:new(W, E),
			type = Signal.MAIN_MANUAL,
			aspect = Signal.RED
		})
		cairo.surface_write_to_png(sfc, 'map.png')
	end