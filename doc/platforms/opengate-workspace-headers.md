# Workspace header candidate

Every OpenGate floating host can receive one compact Panels accessory in its existing native title bar. It does not create another window, another traffic-light set, or a new vertical row. Native Edit UI's main window is excluded. Compact/hidden titlebar styles retain the existing top-menu recovery route rather than forcing a title bar back on.

Each header lists only its own panel doors. The primary Timeline group (or largest remaining visible surface) also exposes All Panels using the exact existing Show Panel actions, plus floating-window doors. Secondary groups remain independent magnetic surfaces. Whole-group hide is stored separately from member hidden states, and calling a member clears the group's hidden state before revealing/selecting that member. Group snapshots persist that separate window door.

The native Workspace Toolbar's scoped `labelTitle` under `centerFrame` is the project-name source. Its text is observed and reflected into existing host titles, with a short local group suffix for secondary windows. When the native source cannot be resolved, the existing group title remains; source resolution is logged rather than inventing a project name. No project API, selection or project state is mutated. Project open/rename/switch remains a live acceptance requirement.

When Workspace Toolbar is grouped, its redundant per-panel header is hidden; it retains its native controls and detach control. The surrounding window keeps its single native header and pin light.

Logs cover project-source resolution/title updates, header scope, panel-door requests and group-window visibility. The hidden header fixture passes project text changes, local versus global scope, shared action identity, unchanged host height, original traffic-light count and non-overlapping header accessory. Native group fixtures exercise preservation of member doors when the whole group closes/opens. Live behavior is pending installation.
