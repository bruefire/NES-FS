
world.scale = 60
plr = world.player
objs = world.objs

bgn = 15
span = 10
dst = 0.1062
line = 5
PIE = 3.1415926535
objs[bgn-1].SetLocRelative(plr, 0, -1.0, 0.0, 0.0)
for line in range(0, line) :
	for idx in range(bgn + span*line, bgn + span*(line+1)) :
		objs[idx].SetLocRelative(objs[bgn+span*line-1], dst*(idx - (bgn + span*line) + 1), 0.0, 0.0, 1.0)
	objs[bgn + span*(line+1)-1].SetRotRelative(0.5*PIE, 0, 0)

	
line = 5
bgn = 15 + span * line
plr.SetRotRelative(0.5*PIE, 0.0, 0.0)
objs[bgn-1].SetLocRelative(plr, 0, -1.0, 0.0, 0.0)
for line in range(0, line) :
	for idx in range(bgn + span*line, bgn + span*(line+1)) :
		objs[idx].SetLocRelative(objs[bgn+span*line-1], dst*(idx - (bgn + span*line) + 1), 0.0, 0.0, 1.0)
	objs[bgn + span*(line+1)-1].SetRotRelative(0.5*PIE, 0, 0)

	
line = 5
bgn = 15 + span * line * 2
plr.SetRotRelative(0.5*PIE, 0.0, 0.0)
objs[bgn-1].SetLocRelative(plr, 0, -1.0, 0.0, 0.0)
for line in range(0, line) :
	for idx in range(bgn + span*line, bgn + span*(line+1)) :
		objs[idx].SetLocRelative(objs[bgn+span*line-1], dst*(idx - (bgn + span*line) + 1), 0.0, 0.0, 1.0)
	objs[bgn + span*(line+1)-1].SetRotRelative(0.5*PIE, 0, 0)

	
line = 5
bgn = 15 + span * line * 3
plr.SetRotRelative(0.5*PIE, 0.0, 0.0)
objs[bgn-1].SetLocRelative(plr, 0, -1.0, 0.0, 0.0)
for line in range(0, line) :
	for idx in range(bgn + span*line, bgn + span*(line+1)) :
		objs[idx].SetLocRelative(objs[bgn+span*line-1], dst*(idx - (bgn + span*line) + 1), 0.0, 0.0, 1.0)
	objs[bgn + span*(line+1)-1].SetRotRelative(0.5*PIE, 0, 0)
	
	
	
line = 5
bgn = 15 + span * line * 4
plr.SetRotRelative(0, 0.5*PIE, 0.0)
objs[bgn-1].SetLocRelative(plr, 0, -1.0, 0.0, 0.0)
for line in range(0, line) :
	for idx in range(bgn + span*line, bgn + span*(line+1)) :
		objs[idx].SetLocRelative(objs[bgn+span*line-1], dst*(idx - (bgn + span*line) + 1), 0.0, 0.0, 1.0)
	objs[bgn + span*(line+1)-1].SetRotRelative(0.5*PIE, 0, 0)

	
line = 5
bgn = 15 + span * line * 5
plr.SetRotRelative(0.5*PIE, 0.0, 0.0)
objs[bgn-1].SetLocRelative(plr, 0, -1.0, 0.0, 0.0)
for line in range(0, line) :
	for idx in range(bgn + span*line, bgn + span*(line+1)) :
		objs[idx].SetLocRelative(objs[bgn+span*line-1], dst*(idx - (bgn + span*line) + 1), 0.0, 0.0, 1.0)
	objs[bgn + span*(line+1)-1].SetRotRelative(0.5*PIE, 0, 0)

	
line = 5
bgn = 15 + span * line * 6
plr.SetRotRelative(0.5*PIE, 0.0, 0.0)
objs[bgn-1].SetLocRelative(plr, 0, -1.0, 0.0, 0.0)
for line in range(0, line) :
	for idx in range(bgn + span*line, bgn + span*(line+1)) :
		objs[idx].SetLocRelative(objs[bgn+span*line-1], dst*(idx - (bgn + span*line) + 1), 0.0, 0.0, 1.0)
	objs[bgn + span*(line+1)-1].SetRotRelative(0.5*PIE, 0, 0)
	
line = 5
bgn = 15 + span * line * 7
plr.SetRotRelative(0.5*PIE, 0.0, 0.0)
objs[bgn-1].SetLocRelative(plr, 0, -1.0, 0.0, 0.0)
for line in range(0, line) :
	for idx in range(bgn + span*line, bgn + span*(line+1)) :
		objs[idx].SetLocRelative(objs[bgn+span*line-1], dst*(idx - (bgn + span*line) + 1), 0.0, 0.0, 1.0)
	objs[bgn + span*(line+1)-1].SetRotRelative(0.5*PIE, 0, 0)
	