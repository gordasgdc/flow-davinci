// SPDX-License-Identifier: MIT
import AppKit

let directory = ProcessInfo.processInfo.environment["UNBOUND_LAUNCH_DIRECTORY"].map { URL(fileURLWithPath: $0) }
    ?? FileManager.default.homeDirectoryForCurrentUser.appendingPathComponent("Library/Logs/FlowDaVinci")

func record(_ event: String, details: [String: Any] = [:]) {
    do {
        try FileManager.default.createDirectory(at: directory, withIntermediateDirectories: true)
        var row = details
        row["component"] = "Flow DaVinci"; row["vendor"] = "Cristi Gordas (GDC)"
        row["event"] = event
        row["time"] = ISO8601DateFormatter().string(from: Date())
        row["pid"] = ProcessInfo.processInfo.processIdentifier
        let path = directory.appendingPathComponent("launches.jsonl")
        if !FileManager.default.fileExists(atPath: path.path) { FileManager.default.createFile(atPath: path.path, contents: nil) }
        let file = try FileHandle(forWritingTo: path)
        defer { try? file.close() }
        try file.seekToEnd()
        try file.write(contentsOf: JSONSerialization.data(withJSONObject: row) + Data([10]))
    } catch { NSLog("Flow DaVinci logging: %@", error.localizedDescription) }
}

@discardableResult func requestControls() throws -> String {
    try FileManager.default.createDirectory(at: directory, withIntermediateDirectories: true)
    let token = "app-" + UUID().uuidString
    try token.write(to: directory.appendingPathComponent("show-controls.request"), atomically: true, encoding: .utf8)
    record("app.controls.request", details: ["request": token])
    return token
}

if CommandLine.arguments.contains("--self-test") {
    do {
        let first = try requestControls(), second = try requestControls()
        let actual = try String(contentsOf: directory.appendingPathComponent("show-controls.request"), encoding: .utf8)
        guard first != second && actual == second else { exit(1) }
        print("PASS: repeatable launcher requests and diagnostic logging")
        exit(0)
    } catch { fputs("Launcher self-test failed: \(error)\n", stderr); exit(1) }
}

final class Launcher: NSObject, NSApplicationDelegate {
    private var window: NSWindow!
    private let status = NSTextField(wrappingLabelWithString: "Deschide Resolve și un proiect pentru a folosi controalele panourilor.")

    func applicationDidFinishLaunching(_ notification: Notification) {
        record("app.launch")
        window = NSWindow(contentRect: NSRect(x: 0, y: 0, width: 420, height: 240),
                          styleMask: [.titled, .closable, .miniaturizable], backing: .buffered, defer: false)
        window.title = "Flow DaVinci"
        window.isReleasedWhenClosed = false
        let show = NSButton(title: "Afișează controalele panourilor", target: self, action: #selector(showControls))
        let logs = NSButton(title: "Deschide jurnalele de diagnostic", target: self, action: #selector(openLogs))
        let about = NSButton(title: "Despre Flow DaVinci", target: self, action: #selector(showAbout))
        let updates = NSButton(title: "Verifică actualizările…", target: self, action: #selector(checkUpdates))
        let credit = NSTextField(wrappingLabelWithString: "Flow DaVinci — versiune pentru testare. Adaptare inițiată de Cristi Gordaș (GDC), bazată pe OpenGate de CineWaltz și Unbound de Wolfgang Woehl (MIT).")
        let stack = NSStackView(views: [status, show, logs, about, updates, credit])
        stack.orientation = .vertical
        stack.alignment = .leading
        stack.spacing = 16
        stack.translatesAutoresizingMaskIntoConstraints = false
        window.contentView!.addSubview(stack)
        NSLayoutConstraint.activate([
            stack.leadingAnchor.constraint(equalTo: window.contentView!.leadingAnchor, constant: 20),
            stack.trailingAnchor.constraint(equalTo: window.contentView!.trailingAnchor, constant: -20),
            stack.topAnchor.constraint(equalTo: window.contentView!.topAnchor, constant: 20)
        ])
        window.center()
        window.makeKeyAndOrderFront(nil)
        requestAndOpen()
    }

    @objc func showControls() {
        record("app.button", details: ["action": "show-controls"])
        requestAndOpen()
    }
    private func requestAndOpen() {
        do {
            try requestControls()
            if NSRunningApplication.runningApplications(withBundleIdentifier: "com.blackmagic-design.DaVinciResolve").isEmpty {
                let app = URL(fileURLWithPath: "/Applications/DaVinci Resolve/DaVinci Resolve.app")
                NSWorkspace.shared.openApplication(at: app, configuration: NSWorkspace.OpenConfiguration()) { _, error in
                    DispatchQueue.main.async {
                        if let error = error {
                            self.status.stringValue = error.localizedDescription
                            record("app.resolve.launch.failed", details: ["error": error.localizedDescription])
                        } else { record("app.resolve.launch") }
                    }
                }
            }
            status.stringValue = "Controalele au fost cerute. Asistentul instalat în Resolve le va afișa când este gata."
        } catch {
            status.stringValue = error.localizedDescription
            record("app.controls.failed", details: ["error": error.localizedDescription])
        }
    }
    @objc func showAbout() {
        record("app.button", details: ["action": "about"])
        let info = Bundle.main.infoDictionary ?? [:]
        let alert = NSAlert()
        alert.messageText = "Flow DaVinci"
        alert.informativeText = "Versiunea \(info["CFBundleShortVersionString"] as? String ?? "necunoscută") · build \(info["CFBundleVersion"] as? String ?? "—")\nVersiune pentru testare\n\nAdaptare inițiată și coordonată de Cristi Gordaș (GDC). Program gratuit, licență MIT; notificările de drepturi de autor ale autorilor originali se păstrează în pachet.\n\nOpenGate by CineWaltz, construit pe Unbound de Wolfgang Woehl (licență MIT).\n\nProiect independent, neafiliat cu Blackmagic Design. DaVinci Resolve este marcă a Blackmagic Design."
        alert.addButton(withTitle: "OK")
        alert.runModal()
    }
    private func updateHelperResult(_ command: String) -> [String: Any] {
        let helper = Bundle.main.executableURL!.deletingLastPathComponent().appendingPathComponent("flow-update")
        let process = Process(), pipe = Pipe()
        process.executableURL = helper; process.arguments = [command]; process.standardOutput = pipe
        do { try process.run() } catch { return ["status": "failed", "reason": "helper"] }
        let data = pipe.fileHandleForReading.readDataToEndOfFile()
        process.waitUntilExit()
        return (try? JSONSerialization.jsonObject(with: data) as? [String: Any]) ?? ["status": "failed", "reason": "helper"]
    }
    @objc func checkUpdates() {
        record("app.button", details: ["action": "check-updates"])
        status.stringValue = "Se verifică actualizările…"
        DispatchQueue.global().async {
            let result = self.updateHelperResult("check")
            DispatchQueue.main.async { self.presentUpdate(result) }
        }
    }
    private func presentUpdate(_ result: [String: Any]) {
        status.stringValue = "Deschide Resolve și un proiect pentru a folosi controalele panourilor."
        let alert = NSAlert()
        switch result["status"] as? String {
        case "up_to_date":
            alert.messageText = "Flow DaVinci este la zi."
        case "available":
            alert.messageText = "Este disponibilă o versiune nouă: \(result["version"] as? String ?? "") (build \(result["build"] as? Int ?? 0))."
            alert.informativeText = ((result["notes"] as? String) ?? "") + "\n\nPachetul este descărcat, verificat (SHA-256, semnătură, notarizare) și apoi deschis. Închide DaVinci Resolve înainte de instalare; setările și presetările tale se păstrează."
            alert.addButton(withTitle: "Descarcă și verifică")
            alert.addButton(withTitle: "Mai târziu")
            if alert.runModal() == .alertFirstButtonReturn { downloadUpdate() }
            return
        default:
            alert.alertStyle = .warning
            let reason = result["reason"] as? String
            alert.messageText = reason == "offline" ? "Verificarea nu a reușit: nu există conexiune la serverul de actualizări." : "Verificarea nu a reușit: răspunsul serverului nu este valid."
        }
        alert.runModal()
    }
    private func downloadUpdate() {
        status.stringValue = "Se descarcă și se verifică pachetul…"
        DispatchQueue.global().async {
            let result = self.updateHelperResult("download")
            DispatchQueue.main.async {
                self.status.stringValue = "Deschide Resolve și un proiect pentru a folosi controalele panourilor."
                let alert = NSAlert()
                if result["status"] as? String == "downloaded", let path = result["path"] as? String {
                    alert.messageText = "Actualizarea a fost descărcată și verificată. Închide DaVinci Resolve, apoi deschide instalatorul."
                    alert.addButton(withTitle: "Deschide instalatorul")
                    alert.addButton(withTitle: "Mai târziu")
                    if alert.runModal() == .alertFirstButtonReturn { NSWorkspace.shared.open(URL(fileURLWithPath: path)) }
                } else {
                    alert.alertStyle = .warning
                    alert.messageText = (result["message"] as? String) ?? "Descărcarea a eșuat."
                    alert.runModal()
                }
            }
        }
    }
    @objc func openLogs() {
        record("app.button", details: ["action": "open-logs"])
        NSWorkspace.shared.open(directory)
    }
    func applicationShouldHandleReopen(_ sender: NSApplication, hasVisibleWindows flag: Bool) -> Bool {
        record("app.reopen")
        window.makeKeyAndOrderFront(nil)
        requestAndOpen()
        return true
    }
    func applicationWillTerminate(_ notification: Notification) { record("app.quit") }
}

let app = NSApplication.shared
let delegate = Launcher()
app.setActivationPolicy(.regular)
app.delegate = delegate
app.run()
