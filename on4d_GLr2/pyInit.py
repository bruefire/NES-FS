# english only
import sys
import h3sim

class CatchOutErr :
	def __init__(self) :
		self.value = ''
	def write(self, txt) :
		self.value += txt
catchOutErr = CatchOutErr()
sys.stdout = catchOutErr
sys.stderr = catchOutErr

class ObjData :
	def __init__(self) :
		self.idx = None
	def SetIdx(self, val) :
		self.idx = val
	def SetLocRelative(self, trg, dst, x, y, z) :
		h3sim.SetLocRelative(self, trg, dst, x, y, z)
	def SetRotRelative(self, x, y, z) :
		h3sim.SetRotRelative(self, x, y, z)


