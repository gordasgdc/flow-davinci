// SPDX-License-Identifier: MIT
// Flow DaVinci: verificare și descărcare ghidată a actualizărilor (macOS).
// Rezultatul este mereu un obiect JSON pe stdout; manifestul trebuie să fie semnat (gdc.signed/1, Ed25519).
// Nu instalează nimic și nu atinge Resolve: instalarea o face pachetul .pkg tranzacțional, deschis de utilizator.
import Foundation
import CryptoKit

let homeDirectory = ProcessInfo.processInfo.environment["HOME"] ?? NSHomeDirectory()

#if FLOW_UPDATE_TESTING
// Doar în varianta de test (nu se livrează): cheie, manifest și stare injectabile.
let env = ProcessInfo.processInfo.environment
let publicKeyBase64 = env["FLOW_UPDATE_TEST_PUBKEY"] ?? ""
let manifestURLString = env["FLOW_UPDATE_TEST_URL"] ?? ""
let installedInfoPath = env["FLOW_UPDATE_TEST_INSTALLED"] ?? ""
let stateDirectory = env["FLOW_UPDATE_TEST_STATE"] ?? NSTemporaryDirectory()
let allowedArtifactPrefix = env["FLOW_UPDATE_TEST_ARTIFACT_PREFIX"] ?? ""
let testing = true
#else
let publicKeyBase64 = "StnroDRQYwQTvPHHfon+u/x/daxaK0/6vCmL9FhFlE4="
let manifestURLString = "https://gordas.dev/flow-davinci/update-manifest.json"
let installedInfoPath = "/Library/Application Support/GDC/FlowDaVinci/release-manifest.json"
let stateDirectory = homeDirectory + "/Library/Application Support/GDC/FlowDaVinci"
let allowedArtifactPrefix = "https://gordas.dev/flow-davinci/downloads/"
let testing = false
#endif

let keyID = "gdc-flow-davinci-release-20261028"
let productID = "gdc-flow-davinci"
let teamID = "8AR6XP8MG7"
let maximumManifestBytes = 65536

struct Failure: Error { let code: String; let message: String }

func emit(_ object: [String: Any]) -> Never {
    let data = (try? JSONSerialization.data(withJSONObject: object, options: [.sortedKeys])) ?? Data("{}".utf8)
    FileHandle.standardOutput.write(data)
    FileHandle.standardOutput.write(Data("\n".utf8))
    exit(0)
}

func fail(_ code: String, _ message: String) -> Never {
    emit(["status": "failed", "reason": code, "message": message])
}

func versionParts(_ value: String) -> [Int]? {
    let parts = value.split(separator: ".", omittingEmptySubsequences: false).map { Int($0) }
    guard parts.count >= 1, parts.count <= 4, !parts.contains(where: { $0 == nil || $0! < 0 }) else { return nil }
    return parts.map { $0! }
}

func isNewer(_ a: [Int], than b: [Int]) -> Bool {
    for i in 0..<max(a.count, b.count) {
        let x = i < a.count ? a[i] : 0, y = i < b.count ? b[i] : 0
        if x != y { return x > y }
    }
    return false
}

func installed() throws -> (version: String, build: Int) {
    guard let data = FileManager.default.contents(atPath: installedInfoPath),
          let json = try? JSONSerialization.jsonObject(with: data) as? [String: Any],
          let version = json["version"] as? String, versionParts(version) != nil else {
        throw Failure(code: "installed-unknown", message: "Nu am putut citi versiunea instalată.")
    }
    return (version, (json["build_number"] as? Int) ?? 0)
}

func fetch(_ urlString: String, limit: Int) throws -> Data {
    guard let url = URL(string: urlString), let scheme = url.scheme,
          scheme == "https" || (testing && (scheme == "http" || scheme == "file")) else {
        throw Failure(code: "bad-url", message: "Adresa de actualizare nu este validă.")
    }
    if scheme == "file" { return (try? Data(contentsOf: url)) ?? Data() }
    let configuration = URLSessionConfiguration.ephemeral
    configuration.timeoutIntervalForRequest = 15
    configuration.requestCachePolicy = .reloadIgnoringLocalCacheData
    let session = URLSession(configuration: configuration)
    let done = DispatchSemaphore(value: 0)
    var result: Data?, status = 0, problem: Error?
    session.dataTask(with: url) { data, response, error in
        result = data; problem = error; status = (response as? HTTPURLResponse)?.statusCode ?? 0; done.signal()
    }.resume()
    if done.wait(timeout: .now() + 25) == .timedOut { throw Failure(code: "offline", message: "Serverul nu a răspuns la timp.") }
    if problem != nil { throw Failure(code: "offline", message: "Nu m-am putut conecta la serverul de actualizări.") }
    guard status == 200, let data = result else { throw Failure(code: "http-\(status)", message: "Serverul a răspuns cu eroare (\(status)).") }
    guard data.count <= limit else { throw Failure(code: "too-large", message: "Răspunsul serverului este prea mare.") }
    return data
}

struct Offer {
    let version: String, build: Int, sequence: Int, mandatory: Bool, notes: String
    let url: String, size: Int, sha256: String
}

func isoDate(_ value: Any?) -> Date? {
    guard let text = value as? String else { return nil }
    return ISO8601DateFormatter().date(from: text)
}

/// Descarcă și verifică manifestul; întoarce oferta (poate fi egală cu versiunea instalată) sau aruncă.
func loadOffer() throws -> Offer {
    let data = try fetch(manifestURLString, limit: maximumManifestBytes)
    guard let envelope = try? JSONSerialization.jsonObject(with: data) as? [String: Any],
          envelope["format"] as? String == "gdc.signed/1", envelope["context"] as? String == "release-manifest",
          envelope["key_id"] as? String == keyID,
          let payloadText = envelope["payload"] as? String, let payload = Data(base64Encoded: payloadText),
          let signatureText = envelope["signature"] as? String, let signature = Data(base64Encoded: signatureText), signature.count == 64,
          let keyRaw = Data(base64Encoded: publicKeyBase64), let key = try? Curve25519.Signing.PublicKey(rawRepresentation: keyRaw) else {
        throw Failure(code: "invalid-response", message: "Răspunsul serverului nu este un manifest de actualizare valid.")
    }
    var message = Data("gdc.signed/1\nrelease-manifest\n".utf8); message.append(payload)
    guard key.isValidSignature(signature, for: message) else {
        throw Failure(code: "bad-signature", message: "Semnătura manifestului nu este validă; actualizarea nu este oferită.")
    }
    guard let m = try? JSONSerialization.jsonObject(with: payload) as? [String: Any],
          m["schema"] as? String == "gdc.release-manifest/1", m["product_id"] as? String == productID,
          m["platform"] as? String == "macos",
          let version = m["version"] as? String, versionParts(version) != nil,
          let sequence = m["sequence"] as? Int, let issued = isoDate(m["issued_at"]), let expires = isoDate(m["expires_at"]),
          let artifact = m["artifact"] as? [String: Any], artifact["kind"] as? String == "pkg",
          let url = artifact["url"] as? String, let size = artifact["size"] as? Int, size > 0,
          let sha = artifact["sha256"] as? String, sha.count == 64,
          artifact["team_id"] as? String == teamID, artifact["bundle_id"] as? String == "dev.gordas.flowdavinci" else {
        throw Failure(code: "invalid-response", message: "Manifestul nu conține câmpurile așteptate.")
    }
    guard url.hasPrefix(allowedArtifactPrefix), !allowedArtifactPrefix.isEmpty else {
        throw Failure(code: "bad-artifact-url", message: "Adresa pachetului nu aparține gordas.dev/flow-davinci.")
    }
    let now = Date()
    guard expires > now, issued < now.addingTimeInterval(86400) else {
        throw Failure(code: "expired", message: "Manifestul de actualizare a expirat sau are o dată invalidă.")
    }
    let statePath = stateDirectory + "/update-state.json"
    let last = ((try? JSONSerialization.jsonObject(with: Data(contentsOf: URL(fileURLWithPath: statePath))) as? [String: Any])?["last_sequence"] as? Int) ?? 0
    guard sequence >= last else {
        throw Failure(code: "rollback", message: "Manifestul este mai vechi decât unul văzut deja; este ignorat.")
    }
    try? FileManager.default.createDirectory(atPath: stateDirectory, withIntermediateDirectories: true)
    try? JSONSerialization.data(withJSONObject: ["last_sequence": sequence]).write(to: URL(fileURLWithPath: statePath))
    let notes = (m["notes"] as? [String: String])?["ro"] ?? ""
    return Offer(version: version, build: (m["build"] as? Int) ?? 0, sequence: sequence, mandatory: (m["mandatory"] as? Bool) ?? false,
                 notes: notes, url: url, size: size, sha256: sha.lowercased())
}

func describe(_ offer: Offer, installed: (version: String, build: Int)) -> [String: Any] {
    let newer = isNewer(versionParts(offer.version)!, than: versionParts(installed.version)!)
        || (versionParts(offer.version)! == versionParts(installed.version)! && offer.build > installed.build)
    var out: [String: Any] = ["installed_version": installed.version, "installed_build": installed.build]
    if !newer {
        out["status"] = "up_to_date"; out["message"] = "Ai versiunea la zi."
        return out
    }
    out["status"] = "available"; out["version"] = offer.version; out["build"] = offer.build; out["notes"] = offer.notes
    out["mandatory"] = offer.mandatory; out["size"] = offer.size
    return out
}

func sha256Hex(of path: String) -> String? {
    guard let handle = FileHandle(forReadingAtPath: path) else { return nil }
    defer { try? handle.close() }
    var hasher = SHA256()
    while let chunk = try? handle.read(upToCount: 1 << 20), !chunk.isEmpty { hasher.update(data: chunk) }
    return hasher.finalize().map { String(format: "%02x", $0) }.joined()
}

func run(_ tool: String, _ arguments: [String]) -> (Int32, String) {
    let process = Process(), pipe = Pipe()
    process.executableURL = URL(fileURLWithPath: tool); process.arguments = arguments
    process.standardOutput = pipe; process.standardError = pipe
    do { try process.run() } catch { return (-1, "") }
    let output = String(data: pipe.fileHandleForReading.readDataToEndOfFile(), encoding: .utf8) ?? ""
    process.waitUntilExit()
    return (process.terminationStatus, output)
}

func download(_ offer: Offer) throws -> String {
    let directory = homeDirectory + "/Library/Caches/dev.gordas.flowdavinci/updates"
    try FileManager.default.createDirectory(atPath: directory, withIntermediateDirectories: true)
    let target = directory + "/FlowDaVinci-\(offer.version)-macOS.pkg"
    let data = try fetch(offer.url, limit: offer.size + 1)
    guard data.count == offer.size else { throw Failure(code: "size-mismatch", message: "Dimensiunea pachetului descărcat nu corespunde manifestului.") }
    let partial = target + ".partial"
    try data.write(to: URL(fileURLWithPath: partial))
    guard sha256Hex(of: partial) == offer.sha256 else {
        try? FileManager.default.removeItem(atPath: partial)
        throw Failure(code: "hash-mismatch", message: "Suma SHA-256 a pachetului nu corespunde manifestului; fișierul a fost șters.")
    }
    if !testing {
        let (signedStatus, signedOutput) = run("/usr/sbin/pkgutil", ["--check-signature", partial])
        guard signedStatus == 0, signedOutput.contains("Developer ID Installer"), signedOutput.contains("(\(teamID))") else {
            try? FileManager.default.removeItem(atPath: partial)
            throw Failure(code: "bad-package-signature", message: "Pachetul nu este semnat cu identitatea Developer ID așteptată; fișierul a fost șters.")
        }
        let (gatekeeper, _) = run("/usr/sbin/spctl", ["--assess", "--type", "install", partial])
        guard gatekeeper == 0 else {
            try? FileManager.default.removeItem(atPath: partial)
            throw Failure(code: "not-notarized", message: "Pachetul nu este acceptat de Gatekeeper (notarizare lipsă); fișierul a fost șters.")
        }
    }
    try? FileManager.default.removeItem(atPath: target)
    try FileManager.default.moveItem(atPath: partial, toPath: target)
    return target
}

let arguments = CommandLine.arguments.dropFirst()
guard let command = arguments.first, command == "check" || command == "download" else {
    FileHandle.standardError.write(Data("Utilizare: flow-update check|download\n".utf8)); exit(2)
}
do {
    let current = try installed()
    let offer = try loadOffer()
    var out = describe(offer, installed: current)
    if command == "download" {
        guard out["status"] as? String == "available" else { emit(out) }
        out["path"] = try download(offer); out["status"] = "downloaded"
        out["message"] = "Pachetul a fost descărcat și verificat (SHA-256, semnătură, notarizare)."
    }
    emit(out)
} catch let error as Failure {
    fail(error.code, error.message)
} catch {
    fail("error", "Verificarea a eșuat.")
}
