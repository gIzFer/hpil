


import time
import serial
from enum import Enum
from enum import auto

class HP3468A:
	# for some reason... auto() starts on 1...
	msgCodes = Enum("msgCodes", [("DAB",0), ("END", auto()),\
	("NUL", auto()), ("GTL", auto()), ("SDC", auto()), ("PPD", auto()), ("GET", auto()), ("ELN", auto()), ("PPE", auto()), ("DDL", auto()), ("DDT", auto()),\
	("NOP", auto()), ("LLO", auto()), ("DCL", auto()), ("PPU", auto()), ("EAR", auto()), ("IFC", auto()), ("REN", auto()), ("NRE", auto()), ("AAU", auto()), ("LPD", auto()),\
	("LAD", auto()), ("MLA", auto()), ("UNL", auto()),\
	("TAD", auto()), ("MTA", auto()), ("OTA", auto()), ("UNT", auto()),\
	("SAD", auto()), ("MSA", auto()), ("OSA", auto()),\
	("IDY", auto()),\
	("RFC", auto()),\
	("ETO", auto()), ("ETE", auto()),\
	("NRD", auto()),\
	("SDA", auto()), ("SST", auto()), ("SDI", auto()), ("SAI", auto()), ("TCT", auto()),\
	("AAD", auto()), ("NAA", auto()), ("IAA", auto()),\
	("AEP", auto()), ("IEP", auto()),\
	("ZES", auto()), ("AES", auto()), ("NES", auto()), ("IES", auto()),\
	("AMP", auto()), ("NMP", auto()), ("IMP", auto()), ("RETURN_LAST_RECEIVED", 101), ("GET_CAL_DATA", 102), ("GET_FIRMWARE_VERSION", 103)])

	ser = None
	ifcMaxTries = 10
	ifcTryDelay = 1 # in s
	#status bytes
	#byte 1
	function = None
	range = None
	digits = None
	#byte 2
	calibrationRamEnabled = None
	lineFrequency = None
	autoZeroOn = None
	autoRangeOn = None
	triggerMode = None
	#byte 3
	srq_powerOnOrTestResetEnabled = None
	srq_calibrationFailed = None
	srq_keyboardSRQPressed = None
	srq_hardwareError = None
	srq_syntaxError = None
	srq_invalidSyntaxError = None
	srq_readAvailable = None
	#byte 4
	error_ad = None
	error_microROM = None
	error_microRAM = None
	error_calibrationRam = None
	#byte 5
	adc_dac_value = None
	#calibration data
	rawCalString = None
	calDataRanges = [
		"DC_0V3",
		"DC_3V",
		"DC_30V",
		"DC_300V",
		"AC_V",
		"R_300",
		"R_3k",
		"R_30k",
		"R_300k",
		"R_3M",
		"R_30M",
		"I_3A"
	]
	calData = {}


	class displayModes(Enum):
		normal = '1'
		text = '2'


	class functions(Enum):
		DC_voltage = '1'
		AC_voltage = '2'
		ohms_2_wire = '3'
		ohms_4_wire = '4'
		DC_current = '5'
		AC_current = '6'
		ohms_extended = '7'

	class triggerModes(Enum):
		internal = '1'
		single = '2'


	def __init__(self, usb_id):

		self.ser = serial.Serial("/dev/ttyUSB" + str(usb_id), 115200, timeout=1)
		if(self.ser.isOpen()):
			print("device opened")
		else:
			raise Exception('device not opened')
		time.sleep(2)
		readIn = str(self.ser.read(self.ser.in_waiting))
		print(readIn)
		if(readIn.find("ready") != -1):
			print("device ready")
		else:
			raise Exception('device not ready')

		self.initMeter()

	def initMeter(self):
		triesCount = 0
		success = False
		while(triesCount < self.ifcMaxTries and success == False):
			triesCount+=1
			self.rawTalk(self.msgCodes.IFC)
			time.sleep(self.ifcTryDelay)
			if(len(self.ser.readline()) > 0):
				success = True

		if(success == False):
			raise Exception('No answer. Tried {} times with between try delay of {}s'.format(self.ifcMaxTries, self.ifcTryDelay))

		self.rawTalk(self.msgCodes.RFC)
		self.rawTalk(self.msgCodes.RFC)
		if(len(self.ser.readline()) == 0):
			raise Exception("RFC FAILED")

	def startTransmission(self):
		self.talk(self.msgCodes.REN)
		self.talk(self.msgCodes.LAD, 22)

	def endTransmission(self):
		self.talk(self.msgCodes.NRE)
		self.talk(self.msgCodes.UNL)

	#doesnt read answer back, exists to implement retry system in init
	def rawTalk(self, command, data=0):
		if not isinstance(command, self.msgCodes):
			raise Exception('talk() invalid command (valid: self.msgCodes: {})'.format(list(self.msgCodes.__members__)))
		#print("sending cmd {}, data: {} ({})".format(command.value, data, chr(data)))
		self.ser.write([command.value, data, 10])
		#time.sleep(0.05)

	def talk(self, command, data=0): #rawTalk but with read answer
		self.ser.reset_input_buffer()
		if(command != self.msgCodes.DAB and command != self.msgCodes.END and command != self.msgCodes.RETURN_LAST_RECEIVED):
			self.rawTalk(self.msgCodes.RFC)
			rfcRead = self.ser.readline()
			print("rfcRead: {}".format(rfcRead))

		self.rawTalk(command, data)
		response = self.ser.readline()
		return bytearray(response)



	def setDisplay(self, displayMode_, text=""):
		self.startTransmission()
		if not isinstance(displayMode_, self.displayModes):
			raise Exception('setDisplay() invalid displayMode (valid: displayModes.normal, displayModes.text)')

		if(len(text) > 12):
			print("WARNING: setDisplay() text too long (>12), truncated")
		text = text[:12]

		self.talk(self.msgCodes.DAB, ord('D'))
		if(displayMode_ == self.displayModes.normal):
			self.talk(self.msgCodes.DAB, ord(self.displayModes.normal.value))
		elif(displayMode_ == self.displayModes.text):
			self.talk(self.msgCodes.DAB, ord(self.displayModes.text.value))
			cmd = self.msgCodes.DAB
			for i in range(12):
				if(i == 11):
					cmd = self.msgCodes.END
				if(i < len(text)):
					self.talk(cmd, ord(text[i]))
				else:
					self.talk(cmd, ord(' '))

		self.endTransmission()



	def setDigits(self, digits):
		if(digits < 3 or digits > 5):
			raise Exception("setDigits() invalid digits (valid: 3,4,5)")
		self.startTransmission()
		self.talk(self.msgCodes.DAB, ord('N'))
		self.talk(self.msgCodes.END, ord(str(digits)))
		self.endTransmission()



	def setTriggerMode(self, triggerMode_):
		if not isinstance(triggerMode_, self.triggerModes):
			raise Exception('setTriggerMode() invalid triggerMode (valid: triggerModes.internal, triggerModes.single)')
		self.startTransmission()
		self.talk(self.msgCodes.DAB, ord('T'))
		self.talk(self.msgCodes.END, ord(triggerMode_.value))
		self.endTransmission()



	def setAutoZero(self, value):
		if not isinstance(value, bool):
			raise Exception('setAutoZero() invalid value (valid: True, False)')
		self.startTransmission()
		self.talk(self.msgCodes.DAB, ord('Z'))
		self.talk(self.msgCodes.END, ord('1' if value else '0'))
		self.endTransmission()



	def setFunction(self, function_, range=0):
		if not isinstance(function_, self.functions):
			raise Exception('setFunction() invalid function (valid: DC_voltage, AC_voltage, ohms_2_wire, ohms_4_wire, DC_current, AC_current, ohms_extended)')

		if(function_ == self.functions.DC_voltage or function_ == self.functions.AC_voltage):
			if(range < 0 or range > 4):
				raise Exception('setFunction() invalid range for function {} (valid: 0 (Autorange), 1 (.3V), 2 (3V), 3 (30V), 4 (300V))'.format(function_.name))
		elif(function_ == self.functions.ohms_2_wire or function_ == self.functions.ohms_4_wire):
			if(range < 0 or range > 4):
				raise Exception('setFunction() invalid range for function {} (valid: 0 (Autorange), 1 (300Ω), 2 (3KΩ), 3 (30KΩ), 4 (300KΩ), 5 (3MΩ), 6 (30MΩ))'.format(function_.name))
		elif(function_ == self.functions.DC_current):
			if(range < 0 or range > 1):
				raise Exception('setFunction() invalid range for function {} (valid: 0 (Autorange), 1 (3A)'.format(function_.name))
		elif(function_ == self.functions.AC_current):
			if(range < 0 or range > 2):
				raise Exception('setFunction() invalid range for function {} (valid: 0 (Autorange), 1 (300mA), 2 (3A)'.format(function_.name))
		elif(function_ == self.functions.ohms_extended):
			if(range < 0 or range > 1):
				raise Exception('setFunction() invalid range for function {} (valid: 0 (Autorange), 1 (10MΩ//Rx)'.format(function_.name))


		self.startTransmission()
		self.talk(self.msgCodes.DAB, ord('F'))
		self.talk(self.msgCodes.DAB, ord(function_.value))
		self.talk(self.msgCodes.DAB, ord('R'))
		self.talk(self.msgCodes.END, ord('A' if range == 0 else range))
		self.endTransmission()



	def prepareRead(self):
		self.talk(self.msgCodes.REN)
		self.talk(self.msgCodes.TAD, 22)

	def finishRead(self):
		self.talk(self.msgCodes.NRE)
		self.talk(self.msgCodes.UNT)
		self.talk(self.msgCodes.IFC)

	#requires prepareRead() and finishRead() to be run before and after
	#run with first=True the first time to start data stream
	#using this directly is slightly faster ~1% with 5 digits, ~10% with 4 and ~15% with 3
	#ex:
	#HP3468A.prepareRead()
	#print(HP3468A.quickRead(True))
	#print(HP3468A.quickRead(False))
	#...
	#print(HP3468A.quickRead(False))
	#HP3468A.finishRead()
	def quickRead(self, first):

		newVal = self.talk(self.msgCodes.SDA if first else self.msgCodes.RETURN_LAST_RECEIVED)

		value = chr(newVal[1])


		#fullStrDone = False
		for i in range(0, 12):
			newVal = self.talk(self.msgCodes.RETURN_LAST_RECEIVED)
			if(len(newVal) == 3):
				#print("---")
				#print(newVal)
				#print(newVal[1])
				#print(type(newVal[1]))
				#print(chr(newVal[1]))
				#print("___")
				value += chr(newVal[1])
			#else:
				#fullStrDone = True
				#print("len(newVal) != 3 ({}, len({}))".format(newVal, len(newVal)))

		self.talk(self.msgCodes.RETURN_LAST_RECEIVED)
		return float(value)

	#600ms per reading 5 digits
	def readSingleValue(self):
		self.prepareRead()
		answer = self.quickRead(True)
		self.finishRead()
		return answer

	def getStatus(self):
		self.talk(self.msgCodes.IFC)
		self.talk(self.msgCodes.REN)
		self.talk(self.msgCodes.LAD, 22)
		self.talk(self.msgCodes.DAB, ord('B'))
		self.talk(self.msgCodes.END, ord('1'))
		self.talk(self.msgCodes.UNL)
		self.talk(self.msgCodes.TAD, 22)
		#self.talk(self.msgCodes.RFC)

		newVal = self.talk(self.msgCodes.SDA)
		value = []
		value.append(newVal[1])
		for i in range(0, 4):
			print(i)
			newVal = self.talk(self.msgCodes.RETURN_LAST_RECEIVED)
			print(newVal)
			value.append(newVal[1])

		self.talk(self.msgCodes.NRE)
		self.talk(self.msgCodes.UNT)
		self.talk(self.msgCodes.IFC)
		statusBytes = bytearray(value)


		#byte 1
		self.function = (statusBytes[0] & 0b11100000) >> 5
		self.function = self.functions(str(self.function))
		self.range = (statusBytes[0] & 0b11100) >> 2
		self.digits = 6 - (statusBytes[0] & 0b11)

		#byte 2
		self.calibrationRamEnabled = bool((statusBytes[1] & 0b10000) >> 4)
		self.lineFrequency = bool((statusBytes[1] & 0b1000) >> 3)
		self.lineFrequency = 50 if self.lineFrequency else 60
		self.autoZeroOn = bool((statusBytes[1] & 0b100) >> 2)
		self.autoRangeOn = bool((statusBytes[1] & 0b10) >> 1)
		self.triggerMode = bool(statusBytes[1] & 0b1)
		self.triggerMode = self.triggerModes.internal if self.triggerMode else self.triggerModes.single

		#byte 3
		self.srq_powerOnEnabled = bool(statusBytes[2] & 0b10000000)
		self.srq_calibrationFailed = bool(statusBytes[2] & 0b100000)
		self.srq_keyboardSRQPressed = bool(statusBytes[2] & 0b10000)
		self.srq_hardwareError = bool(statusBytes[2] & 0b1000)
		self.srq_syntaxError = bool(statusBytes[2] & 0b100)
		self.srq_invalidSyntaxError = bool(statusBytes[2] & 0b10)
		self.srq_readAvailable = bool(statusBytes[2] & 0b1)

		#byte 4
		self.error_ad = bool(statusBytes[3] & 0b1000)
		self.error_microROM = bool(statusBytes[3] & 0b100)
		self.error_microRAM = bool(statusBytes[3] & 0b10)
		self.error_calibrationRam = bool(statusBytes[3] & 0b1)
		self.adc_dac_value = statusBytes[4]

	def printStatusByte1(self):
		print("Function: {}".format(self.function.name))
		print("Range: {}".format(self.range))
		print("Digits: {}".format(self.digits))

	def printStatusByte2(self):
		print("Calibration ram enabled: {}".format(self.calibrationRamEnabled))
		print("Line frequency: {}Hz".format(self.lineFrequency))
		print("AutoZero On: {}".format(self.autoZeroOn))
		print("AutoRange On: {}".format(self.autoRangeOn))
		print("Trigger mode: {}".format(self.triggerMode.name))

	def printStatusByte3(self):
		print("SRQ power on or test/reset: {}".format(self.srq_powerOnOrTestResetEnabled))
		print("SRQ calibration procedure failed: {}".format(self.srq_calibrationFailed))
		print("SRQ keyboard SRQ pressed: {}".format(self.srq_keyboardSRQPressed))
		print("SRQ hardware error: {}".format(self.srq_hardwareError))
		print("SRQ syntax error: {}".format(self.srq_syntaxError))
		print("SRQ invalid syntax error: {}".format(self.srq_invalidSyntaxError))
		print("SRQ reading available: {}".format(self.srq_readAvailable))

	def printStatusByte4(self):
		print("Error A/D: {}".format(self.error_ad))
		print("Error microprocessor ROM: {}".format(self.error_microROM))
		print("Error microprocessor RAM: {}".format(self.error_microRAM))
		print("Error calibration RAM: {}".format(self.error_calibrationRam))

	def printStatusByte5(self):
		print("A/D Converter DAC value: {}".format(self.adc_dac_value))

	def printStatus(self):
		self.printStatusByte1()
		self.printStatusByte2()
		self.printStatusByte3()
		self.printStatusByte4()
		self.printStatusByte5()


	#https://web.archive.org/web/20240920172540/https://hpmuseum.org/forum/thread-8061-page-2.html
	def getCal(self):
		self.rawCalString = []
		self.ser.reset_input_buffer()
		self.rawTalk(self.msgCodes.GET_CAL_DATA)

		for i in range(0, 15):
			time.sleep(0.5)
			#print(".", end='', flush=True)
			if(self.ser.in_waiting > 1):
				break
		#print()
		for i in range(0, 17):
			self.rawCalString.append(self.ser.readline().decode('cp437'))

		#print(self.rawCalString, end='', flush=True)

		#parse cal
		for i in range(2, 14):
			line = self.rawCalString[i]
			#print(line)
			line = line.split(":")[1].split(" ")
			line = list(filter(None, line))
			#print(line)
			calPair = {}
			calPair["offset"] = int(line[-2])
			calPair["gain"] = float(line[-1])
			self.calData[self.calDataRanges[i-2]] = calPair

	def printCal(self):
		print("Range\t:\toffset\tgain")
		for name,value in self.calData.items():
			print("{}\t:\t{}\t{}".format(name, value["offset"], value["gain"]))


	def triggerReading(self):
		self.talk(self.msgCodes.GET)

	#wont work until implemented simultaneous reading while talking on firmware
	#def getSimpleStatus(self):
	#	status = bytearray(self.talk(self.msgCodes.SST)[1])[0]
	#	print(bin(status))




















