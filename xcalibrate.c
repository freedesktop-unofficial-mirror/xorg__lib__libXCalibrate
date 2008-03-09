/*
 * Copyright © 2003 Philip Blundell
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Philip Blundell not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Philip Blundell makes no
 * representations about the suitability of this software for any purpose.  It
 * is provided "as is" without express or implied warranty.
 *
 * PHILIP BLUNDELL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL PHILIP BLUNDELL BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#include <X11/Xlibint.h>
#include <X11/Xutil.h>
#include <X11/Xfuncproto.h>

#include <X11/extensions/extutil.h>
#include <X11/extensions/xcalibratewire.h>
#include <X11/extensions/xcalibrateproto.h>
#include "xcalibrate.h"

#define XCalibrateCheckExtension(dpy,i,val) \
  XextCheckExtension (dpy, i, XCalibrateExtensionName, val)

XExtensionInfo XCalibrateExtensionInfo;
char XCalibrateExtensionName[] = XCALIBRATE_NAME;

static int
XCalibrateCloseDisplay (Display *dpy, XExtCodes *codes);
    
static Bool
XCalibrateWireToEvent(Display *dpy, XEvent *event, xEvent *wire);

static Status
XCalibrateEventToWire(Display *dpy, XEvent *event, xEvent *wire);

static /* const */ XExtensionHooks xcalibrate_extension_hooks = {
    NULL,				/* create_gc */
    NULL,				/* copy_gc */
    NULL,				/* flush_gc */
    NULL,				/* free_gc */
    NULL,				/* create_font */
    NULL,				/* free_font */
    XCalibrateCloseDisplay,		/* close_display */
    XCalibrateWireToEvent,		/* wire_to_event */
    XCalibrateEventToWire,		/* event_to_wire */
    NULL,				/* error */
    NULL,				/* error_string */
};

typedef struct _XCalibrateInfo
{
  int major_version;
  int minor_version;
} XCalibrateInfo;

XExtDisplayInfo *
XCalibrateFindDisplay (Display *dpy)
{
    XExtDisplayInfo *dpyinfo;
    XCalibrateInfo	    *xci;

    dpyinfo = XextFindDisplay (&XCalibrateExtensionInfo, dpy);
    if (!dpyinfo)
    {
	dpyinfo = XextAddDisplay (&XCalibrateExtensionInfo, dpy, 
				  XCalibrateExtensionName,
				  &xcalibrate_extension_hooks,
				  XCalibrateNumberEvents, 0);
	xci = Xmalloc (sizeof (XCalibrateInfo));
	if (!xci)
	    return 0;
	xci->major_version = -1;
	dpyinfo->data = (char *) xci;
    }
    return dpyinfo;
}
    
static int
XCalibrateCloseDisplay (Display *dpy, XExtCodes *codes)
{
    return XextRemoveDisplay (&XCalibrateExtensionInfo, dpy);
}

static Bool
XCalibrateWireToEvent(Display *dpy, XEvent *event, xEvent *wire)
{
    XExtDisplayInfo *info = XCalibrateFindDisplay(dpy);

    XCalibrateCheckExtension(dpy, info, False);

    switch ((wire->u.u.type & 0x7F) - info->codes->first_event)
      {
      case X_XCalibrateRawTouchscreen:
	{
	  XCalibrateRawTouchscreenEvent *aevent;
	  xXCalibrateRawTouchscreenEvent *awire;
	  awire = (xXCalibrateRawTouchscreenEvent *) wire;
	  aevent = (XCalibrateRawTouchscreenEvent *) event;
	  aevent->type = awire->type & 0x7F;
	  aevent->serial = _XSetLastRequestRead(dpy,
						(xGenericReply *) wire);
	  aevent->send_event = (awire->type & 0x80) != 0;
	  aevent->display = dpy;
	  aevent->x = awire->x;
	  aevent->y = awire->y;
	  aevent->pressure = awire->pressure;
	  return True;
	}
      }
    return False;
}

static Status
XCalibrateEventToWire(Display *dpy, XEvent *event, xEvent *wire)
{
    XExtDisplayInfo *info = XCalibrateFindDisplay(dpy);

    XCalibrateCheckExtension(dpy, info, False);

    switch ((event->type & 0x7F) - info->codes->first_event)
      {
      case X_XCalibrateRawTouchscreen:
	{
	  XCalibrateRawTouchscreenEvent *aevent;
	  xXCalibrateRawTouchscreenEvent *awire;
	  awire = (xXCalibrateRawTouchscreenEvent *) wire;
	  aevent = (XCalibrateRawTouchscreenEvent *) event;
	  awire->type = aevent->type | (aevent->send_event ? 0x80 : 0);
	  awire->x = aevent->x;
	  awire->y = aevent->y;
	  awire->pressure = aevent->pressure;
	  return True;
	}
      }
    return False;
}

Bool 
XCalibrateQueryExtension (Display *dpy, int *event_basep, int *error_basep)
{
    XExtDisplayInfo *info = XCalibrateFindDisplay (dpy);

    if (XextHasExtension(info)) 
    {
	*event_basep = info->codes->first_event;
	*error_basep = info->codes->first_error;
	return True;
    } 
    else
	return False;
}

Status 
XCalibrateQueryVersion (Display *dpy,
		    int	    *major_versionp,
		    int	    *minor_versionp)
{
    XExtDisplayInfo		*info = XCalibrateFindDisplay (dpy);
    xXCalibrateQueryVersionReply	rep;
    xXCalibrateQueryVersionReq	*req;
    XCalibrateInfo			*xci;

    XCalibrateCheckExtension (dpy, info, 0);

    xci = (XCalibrateInfo *) info->data;

    /*
     * only get the version information from the server if we don't have it
     * already
     */
    if (xci->major_version == -1) 
      {
	LockDisplay (dpy);
	GetReq (XCalibrateQueryVersion, req);
	req->reqType = info->codes->major_opcode;
	req->xCalibrateReqType = X_XCalibrateQueryVersion;
	req->majorVersion = XCALIBRATE_MAJOR_VERSION;
	req->minorVersion = XCALIBRATE_MINOR_VERSION;
	if (!_XReply (dpy, (xReply *) &rep, 0, xTrue)) 
	  {
	    UnlockDisplay (dpy);
	    SyncHandle ();
	    return 0;
	  }
	xci->major_version = rep.majorVersion;
	xci->minor_version = rep.minorVersion;
      }
    *major_versionp = xci->major_version;
    *minor_versionp = xci->minor_version;
    UnlockDisplay (dpy);
    SyncHandle ();
    return 1;
}

Status 
XCalibrateSetRawMode (Display *dpy, Bool enable)
{
  XExtDisplayInfo		*info = XCalibrateFindDisplay (dpy);
  xXCalibrateRawModeReq *req;
  xXCalibrateRawModeReply	rep;
  LockDisplay (dpy);
  GetReq (XCalibrateRawMode, req);
  req->reqType = info->codes->major_opcode;
  req->xCalibrateReqType = X_XCalibrateRawMode;
  req->on = enable;
  if (!_XReply (dpy, (xReply *) &rep, 0, xFalse)) 
    {
      UnlockDisplay (dpy);
      SyncHandle ();
      return 1;
    }

  UnlockDisplay (dpy);
  SyncHandle ();
  return 0;
}

Status 
XCalibrateScreenToCoord (Display *dpy, int *x, int *y)
{
  XExtDisplayInfo		*info = XCalibrateFindDisplay (dpy);
  xXCalibrateScreenToCoordReq *req;
  xXCalibrateScreenToCoordReply	rep;
  LockDisplay (dpy);
  GetReq (XCalibrateScreenToCoord, req);
  req->reqType = info->codes->major_opcode;
  req->xCalibrateReqType = X_XCalibrateScreenToCoord;
  req->x = *x;
  req->y = *y;
  if (!_XReply (dpy, (xReply *) &rep, 0, xFalse)) 
    {
      UnlockDisplay (dpy);
      SyncHandle ();
      return 1;
    }
  *x = rep.x;
  *y = rep.y;
  UnlockDisplay (dpy);
  SyncHandle ();
  return 0;
}

