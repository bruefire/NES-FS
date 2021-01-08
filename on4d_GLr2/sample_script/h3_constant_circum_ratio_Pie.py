import math

world.scale = 60
plr = world.player
objs = world.objs

radb = 0.5
lineb = 33
bgnb = 12

rad = radb
mtp = 1
bgn = 12
bgn0 = bgn-1
span = 1
line = lineb * mtp
PIE = 3.1415926535
objs[bgn0].SetLocRelative(plr, 0, -1.0, 0.0, 0.0)
for line in range(0, line) :
	objs[bgn0].SetRotRelative(1/math.sinh(rad)*0.0992, 0.0, 0.0)
	for idx in range(bgn + span*line, bgn + span*(line+1)) :
		objs[idx].SetLocRelative(objs[bgn0], rad*(idx - (bgn + span*line) + 1), 0.0, 0.0, 1.0)

mtp = 2
rad = radb * mtp
bgn = bgnb + lineb
line = lineb * mtp
objs[bgn0].SetLocRelative(plr, 0, -1.0, 0.0, 0.0)
for line in range(0, line) :
	objs[bgn0].SetRotRelative(1/math.sinh(rad)*0.0992, 0.0, 0.0)
	for idx in range(bgn + span*line, bgn + span*(line+1)) :
		objs[idx].SetLocRelative(objs[bgn0], rad*(idx - (bgn + span*line) + 1), 0.0, 0.0, 1.0)

mtp = 3
rad = radb * mtp
bgn = bgnb + lineb * 3
line = lineb * mtp
objs[bgn0].SetLocRelative(plr, 0, -1.0, 0.0, 0.0)
for line in range(0, line) :
	objs[bgn0].SetRotRelative(1/math.sinh(rad)*0.0992, 0.0, 0.0)
	for idx in range(bgn + span*line, bgn + span*(line+1)) :
		objs[idx].SetLocRelative(objs[bgn0], rad*(idx - (bgn + span*line) + 1), 0.0, 0.0, 1.0)

mtp = 4
rad = radb * mtp
bgn = bgnb + lineb * 6
line = lineb * mtp
objs[bgn0].SetLocRelative(plr, 0, -1.0, 0.0, 0.0)
for line in range(0, line) :
	objs[bgn0].SetRotRelative(1/math.sinh(rad)*0.0992, 0.0, 0.0)
	for idx in range(bgn + span*line, bgn + span*(line+1)) :
		objs[idx].SetLocRelative(objs[bgn0], rad*(idx - (bgn + span*line) + 1), 0.0, 0.0, 1.0)

dst = 0.102
bgn = bgnb + lineb * 10
line = 19
objs[bgn-1].SetLocRelative(plr, 0, -1.0, 0.0, 0.0)
objs[bgn-1].SetRotRelative(0.5 * PIE, 0.0, 0.0)
for line in range(0, line) :
	for idx in range(bgn + span*line, bgn + span*(line+1)) :
		objs[idx].SetLocRelative(objs[bgn+span*line-1], dst*(idx - (bgn + span*line) + 1), 0.0, 0.0, 1.0)
objs[bgn0].SetLocRelative(plr, 0, -1.0, 0.0, 0.0)