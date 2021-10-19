# FilesView View Layout
# Auto-generated with Neutrino UI Design Tool
# import FilesView

!('link userlib.lnx')
def FilesView_create_view():
	text = "ID:0;Position X:-1;Position Y:-1;Width:300;Height:200;Title:Files;TitleBar:1;MaximizeButton:1;Hidden:0;Maximized:0;StickyDraw:0;WakeOnInteraction:0;|ID:1;Type:ListView;Position X:0;Position Y:0;Width:0;Height:0;Font:Helvetica 8;Items:Hello,Fucking,World,This,Sucks,Balls,Because,I,Dont,Like,NeutrinoOS,;|"
	id = WMCreateWindow(text)
	WMSetActiveWindow(id)
	WMUpdateView()
	return id
