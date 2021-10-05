# clockview View Layout
# Auto-generated with Neutrino UI Design Tool
# import clockview

!('link userlib.lnx')
def clockview_create_view():
	text = "ID:0;Type:WindowInfo;MaximizeButton:0;Width:128;Height:64;Title:Clock;|ID:1;Type:Label;Position X:10;Position Y:10;Width:0;Height:0;Text:00\:00;Font:Logisoso 32;Border:0;|"
	id = WMCreateWindow(text)
	WMSetActiveWindow(id)
	WMUpdateView()
	return id
