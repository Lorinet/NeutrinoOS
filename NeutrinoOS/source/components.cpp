#include "components.h"
#include "syscall.h"
void components::registerComponents()
{
#if defined(COMPONENT_EFFIGY) || defined(COMPONENT_TIWAZ)
	winmgr_api::initialize();
	syscall::interfaces.add(winmgr_api::id, &winmgr_api::instance);
#endif
#if defined(COMPONENT_NFS)
	nfs_api::initialize();
	syscall::interfaces.add(nfs_api::id, &nfs_api::instance);
#endif
#if defined(COMPONENT_IOMANAGER)
	iomanager_api::initialize();
	syscall::interfaces.add(iomanager_api::id, &iomanager_api::instance);
#endif
}