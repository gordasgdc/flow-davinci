# OpenGate repository separation checkpoint

GitHub now defaults to `main` at the published-prototype ancestry plus its cleaned
landing page. No experimental runtime branches were merged into main. The 0.0.1
release/tag and four assets are unchanged. The OpenGate spelling and installer
link are prominent; inherited documentation is moved off the landing page.

The GitHub fork relationship remains. GitHub's documented Leave fork network
operation is permanent and warns that metadata can be lost. It requires the
settings UI; the current automation session cannot open an enabled browser surface.
Do not delete/recreate the repository or attempt an undocumented API.

Read-only inventory: public fork, ~1.7 MB, zero child forks, stars or subscribers,
zero open issues; one historical issue/PR. One prerelease with four assets.
Source history remains intact. Original MIT copyright/license and inherited
Windows sources/artwork are retained; macOS placement controls use the separately
drawn OpenGate gate-and-arrow glyph. Old compatibility names are not artwork use.

Local preservation directory (not committed or public):
`~/Developer/OpenGate/build/macos/github-preservation-20261020/`
contains release assets, release/issue/PR/branch JSON, and a verified complete-history
bundle of all local refs. Published installer SHA-256 verified against GitHub.

Next settings action, after reviewing metadata implications: GitHub Settings →
General → Danger Zone → Leave fork network. Preserve the existing OpenGate URL,
verify all branches/tags and release assets afterward, and restore release metadata
from the backup if GitHub removes it. Do not claim the project is detached until
GitHub's `isFork` becomes false. Support is an alternative if metadata must survive
without recreation. No messages have been sent to Wolfgang or GitHub Support.

Official reference: https://docs.github.com/en/pull-requests/how-tos/work-with-forks/detaching-a-fork
