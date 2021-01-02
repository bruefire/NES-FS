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

# function names called on c++ side
from enum import IntEnum
class FuncObject(IntEnum) :
	SetScale = 0
	GetScale = 1
	SetVelocity = 2
	GetVelocity = 3


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
		return h3sim.Func_NoParam(self, int(FuncObject.GetScale))
	@scale.setter
	def scale(self, val) :
		h3sim.Func_Pram1d(self, int(FuncObject.SetScale), val)
		
	@property
	def velocity(self) :
		return h3sim.Func_NoParam(self, int(FuncObject.GetVelocity))
	@velocity.setter
	def velocity(self, val) :
		h3sim.Func_Pram1d(self, int(FuncObject.SetVelocity), val)


