# Separate final task: OpenGate identity and repository migration

Requested by Jordan on 2026-10-22. Keep this task separate from Windows parity;
do not interrupt or combine migration execution with parity implementation.
The planned final release is OpenGate 0.0.2 revision 4. Preparation is authorized;
the concrete repository cutover requires Jordan's approval after verification.
No cutover, release publication or repository renaming has been performed.

## Goal and sequence

Publish OpenGate as a standalone GitHub repository created normally, not with
GitHub's Fork action. Satisfy Wolfgang's recorded request while preserving the
complete history, original code/attribution, required branches/tags, releases,
configuration and recoverability. Before migration, read the Wolfgang follow-up
in `opengate-task-audit-20261020.md`; do not infer additional upstream requirements.

1. Verify OpenGate/CineWaltz copyright for original contributions and retain
   inherited Wolfgang Woehl MIT notices. Audit artwork, obsolete code and
   installation coexistence with Unbound. No global compatibility-identifier
   renames or code deletion without reference/build evidence.
2. Back up remote refs, local branches/uncommitted work, tags, release descriptions,
   all assets/checksums, stargazers and relevant repository configuration.
   Coordinate with active Windows parity work before cutover.
3. Prepare a standalone repository with complete commit history and required
   refs. Restore releases; configure Actions, permissions and secrets through
   secure mechanisms. Git history does not transfer GitHub configuration.
4. Verify repository fork status, build configuration and installer download
   hashes. Preserve the accepted Mac package byte-for-byte. Prepare a concrete
   cutover/rollback inventory for Jordan's approval.
5. Proposed approved cutover: rename the existing fork `OpenGate-legacy`, rename
   the verified standalone repository `OpenGate`, update remotes/links, and
   archive the legacy repository with a migration notice. Verify release URLs
   and CI explicitly; never assume redirects solve them.
6. Notify existing stargazers that migration is happening. Jordan explicitly
   requested notification. Capture the pre-migration list, establish a workable
   notification mechanism before cutover and record delivery limitations rather
   than claiming everyone was reached without evidence.
7. Publish the planned 0.0.2 revision 4 release only after its applicable parity,
   regression, packaging and approval gates. Separate installer assets and
   truthful platform/live status remain required.

Do not delete the old repository, force-push history, detach the fork, or assume
a public fork can be made private. Never expose credentials while transferring
configuration. The legacy fork and accepted Mac package remain recoverable.

Completion requires GitHub to report the new OpenGate repository is not a fork,
working verified downloads, development pointing at the correct repository,
preserved inherited license/copyright, and a recoverable legacy repository.
Removing GitHub's fork relationship does not erase legitimate source attribution.
