// SPDX-License-Identifier: MIT
#pragma once
// CMake supplies exactly one backend include directory. Backends provide:
// PaneFloatingWindow: QWidget with finishDeferredResize() and interactiveDragActive().
// MainWindowSnap: setWindow(QWidget *), safely detached on destruction.
// WindowGroupActivation: setWindow(QWidget *), event-driven group raising.
// applyPaneDarkCaption(QWidget *, QWidget *) and inheritPaneTaskbarIdentity(...).
// All window operations are performed on the Qt GUI thread.
#include <windowbackend.h>
