#===========================#
# Neutrino Operating System #
#===========================#
# Clock app
# 2021.10.05.
# By Linfinity Technologies
#

include neutrino
import clockview

!('link neutrino.lnx')
!('link userlib.lnx')

def suspend_if_running():
	NtrSuspendProcess()
	!('lj suspend_if_running')

def clock_main():
	hwnd = clockview_create_view()
	hours = NtrGetTime($TIMESEL_HOUR)
	minutes = NtrGetTime($TIMESEL_MINUTE)
	show_time()
	def time_callback():
		hours = NtrGetTime($TIMESEL_HOUR)
		minutes = NtrGetTime($TIMESEL_MINUTE)
		show_time()
	def show_time():
		hourString = str(hours)
		if hours < 10:
			hourString = "0" @ hourString
		minuteString = str(minutes)
		if minutes < 10:
			minuteString = "0" @ minuteString
		WMSetUserElementProperty(hwnd, 1, "Text:" @ hourString @ "\:" @ minuteString)
		WMUpdateView(hwnd)
	NtrAttachEventHandler($EVENT_TIME_CHANGE_MINUTES, time_callback)
	NtrEnableEvents()
	suspend_if_running()

clock_main()