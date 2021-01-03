# english only
import sys
import h3sim

# for output to stdout
class CatchOutErr :
	def __init__(self) :
		self.value = ''
	def write(self, txt) :
		self.value += txt
catchOutErr = CatchOutErr()
sys.stdout = catchOutErr
sys.stderr = catchOutErr

# functions called on c++ side
from enum import IntEnum
class FuncObject(IntEnum) :
	SetObjScale = 0
	GetObjScale = 1
	SetObjVelocity = 2
	GetObjVelocity = 3
	SetObjRotVelocity = 4
	GetObjRotVelocity = 5
	SetPt3x = 6
	SetPt3y = 7
	SetPt3z = 8
	SetWorldScale = 9
	GetWorldScale = 10


class World :
	@property
	def scale(self) :
		h3sim.SetCommonFunc(int(FuncObject.GetWorldScale))
		return h3sim.ExecCommonFunc(self)
	@scale.setter
	def scale(self, val) :
		h3sim.SetCommonFunc(int(FuncObject.SetWorldScale))
		h3sim.ExecCommonFunc(self, val)
		
	@property
	def player(self) :
		return h3sim.GetPlayerObj()


class Dvec3 :
	def __init__(self) :
		self.owIdx = None
		self.type = None
		
	@property
	def x(self) :
		return self.__x
	@x.setter
	def x(self, val) :
		if self.owIdx is None :
			self.__x = val
		else :
			h3sim.SetCommonFunc(int(FuncObject.SetPt3x))
			h3sim.ExecCommonFunc(self.owIdx, self.type, val)

	@property
	def y(self) :
		return self.__y
	@y.setter
	def y(self, val) :
		if self.owIdx is None :
			self.__y = val
		else :
			h3sim.SetCommonFunc(int(FuncObject.SetPt3y))
			h3sim.ExecCommonFunc(self.owIdx, self.type, val)

	@property
	def z(self) :
		return self.__z
	@z.setter
	def z(self, val) :
		if self.owIdx is None :
			self.__z = val
		else :
			h3sim.SetCommonFunc(int(FuncObject.SetPt3z))
			h3sim.ExecCommonFunc(self.owIdx, self.type, val)
		
	def SetX(self, val) :
		self.__x = val
	def SetY(self, val) :
		self.__y = val
	def SetZ(self, val) :
		self.__z = val


class ObjData :
	def __init__(self) :
		self.idx = None

	def SetIdx(self, val) :
		self.idx = val

	def SetLocRelative(self, trg, dst, x, y, z) :
		h3sim.SetLocRelative(self, trg, dst, x, y, z)

	def SetRotRelative(self, x, y, z) :
		h3sim.SetRotRelative(self, x, y, z)

	@property
	def scale(self) :
		h3sim.SetCommonFunc(int(FuncObject.GetObjScale))
		return h3sim.ExecCommonFunc(self)
	@scale.setter
	def scale(self, val) :
		h3sim.SetCommonFunc(int(FuncObject.SetObjScale))
		h3sim.ExecCommonFunc(self, val)
		
	@property
	def velocity(self) :
		h3sim.SetCommonFunc(int(FuncObject.GetObjVelocity))
		return h3sim.ExecCommonFunc(self)
	@velocity.setter
	def velocity(self, val) :
		h3sim.SetCommonFunc(int(FuncObject.SetObjVelocity))
		h3sim.ExecCommonFunc(self, val)
		
	@property
	def rotVelocity(self) :
		h3sim.SetCommonFunc(int(FuncObject.GetObjRotVelocity))
		return h3sim.ExecCommonFunc(self)
	@rotVelocity.setter
	def rotVelocity(self, val) :
		h3sim.SetCommonFunc(int(FuncObject.SetObjRotVelocity))
		h3sim.ExecCommonFunc(self, val)


		
world = World()
world.objs = h3sim.GetObjData()
