
plr = world.player
objs = world.objs

def _TrackObjDirection() :
    for enemy in objs[1:11] :
        enemy.TrackObjDirection(plr)

updateCallback.append(_TrackObjDirection)
