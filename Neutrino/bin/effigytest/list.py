# list View Layout
# Auto-generated with Neutrino UI Design Tool
# import list

!('link userlib.lnx')
!('link neutrino.lnx')
def list_create_view():
	text = "ID:0;Position X:-1;Position Y:-1;Width:-1;Height:-1;Title:Window;TitleBar:1;MaximizeButton:1;Hidden:0;Maximized:0;StickyDraw:0;WakeOnInteraction:0;|"
	id = WMCreateWindow(text)
	WMSetActiveWindow(id)
	WMUpdateView()
	return id

def waste_time():
	NtrSuspendProcess()
	!('lj waste_time')

list_create_view()
waste_time()