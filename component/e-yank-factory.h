/*
 * e-yank-factory.c: Executive Summary Factory.
 *
 * Copyright (C) 2001 Ximian, Inc.
 *
 * Authors:  Iain Holmes <iain@ximian.com>
 */

#ifndef __E_YANK_FACTORY_H__
#define __E_YANK_FACTORY_H__

BonoboControl *e_yank_factory_new_control (const char *uri,
					      const GNOME_Evolution_Shell shell);

#endif
