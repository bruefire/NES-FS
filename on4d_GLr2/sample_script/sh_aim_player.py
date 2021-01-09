
plr = world.player
objs = world.objs
loop = 0
sbIdx = 11
sIdx = sbIdx

def _TrackObjDirection() :
    global loop
    global sIdx
    global sbIdx
    global world
    for enemy in objs[1:11] :
        if enemy.idx == world.player.idx :
            continue
        enemy.TrackObjDirection(world.player)
        
        if enemy.GetDistance(world.player) < 20 :
            enemy.SetLocRelative(enemy, -0.6, 0.0, 0.0, 1.0)
        
        if loop == 100 :
            objs[sIdx].SetLocRelative(enemy, 0, 0.0, 0.0, 1.0)
            objs[sIdx].velocity = 0.5
            sIdx += 1
            if sIdx == 100 :
                sIdx = sbIdx
           
    if loop == 100 :     
        loop = 0
    loop += 1

updateCallback.append(_TrackObjDirection)
